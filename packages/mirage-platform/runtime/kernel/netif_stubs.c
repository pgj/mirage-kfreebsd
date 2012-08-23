/*-
 * Copyright (c) 2012 Gabor Pali
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/libkern.h>
#include <sys/lock.h>
#include <sys/mutex.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/if_arp.h>
#include <net/vnet.h>

#include "caml/mlvalues.h"
#include "caml/memory.h"
#include "caml/alloc.h"
#include "caml/fail.h"
#include "caml/bigarray.h"

struct mbuf_entry {
	LIST_ENTRY(mbuf_entry)	me_next;
	struct mbuf		*me_m;
};

struct plugged_if {
	TAILQ_ENTRY(plugged_if)	pi_next;
	u_short	pi_index;
	u_short	pi_llindex;
	int	pi_flags;
	char	pi_lladdr[32];
	char	pi_xname[IFNAMSIZ];
	struct  mtx			pi_rx_lock;
	LIST_HEAD(, mbuf_entry)		pi_rx_head;
};

TAILQ_HEAD(plugged_ifhead, plugged_if) pihead =
    TAILQ_HEAD_INITIALIZER(pihead);

int plugged = 0;


/* Currently only Ethernet interfaces are returned. */
CAMLprim value caml_get_vifs(value v_unit);
CAMLprim value caml_plug_vif(value id);
CAMLprim value caml_unplug_vif(value id);
CAMLprim value caml_get_mbufs(value id);

void netif_ether_input(struct ifnet *ifp, struct mbuf **mp);
int  netif_ether_output(struct ifnet *ifp, struct mbuf **mp);


static struct plugged_if *
find_pi_by_index(u_short val)
{
	struct plugged_if *pip;

	TAILQ_FOREACH(pip, &pihead, pi_next)
		if (pip->pi_index == val)
			return pip;
	return NULL;
}

static struct plugged_if *
find_pi_by_name(const char *val)
{
	struct plugged_if *pip;

	TAILQ_FOREACH(pip, &pihead, pi_next)
		if (strncmp(pip->pi_xname, val, IFNAMSIZ) == 0)
			return pip;
	return NULL;
}

CAMLprim value
caml_get_vifs(value v_unit)
{
	CAMLparam1(v_unit);
	CAMLlocal2(result, r);
	struct ifnet *ifp;
	struct ifaddr *ifa;
	struct sockaddr_dl *sdl;

	result = Val_emptylist;
	IFNET_RLOCK_NOSLEEP();
	TAILQ_FOREACH(ifp, &V_ifnet, if_link) {
		IF_ADDR_RLOCK(ifp);
		TAILQ_FOREACH(ifa, &ifp->if_addrhead, ifa_link) {
			sdl = (struct sockaddr_dl *) ifa->ifa_addr;
			if (sdl != NULL && sdl->sdl_family == AF_LINK &&
			    sdl->sdl_type == IFT_ETHER) {
				/* We have a MAC address, add the interface. */
				r = caml_alloc(2, 0);
				Store_field(r, 0,
				    caml_copy_string(ifp->if_xname));
				Store_field(r, 1, result);
				result = r;
				break;
			}
		}
		IF_ADDR_RUNLOCK(ifp);
	}
	IFNET_RUNLOCK_NOSLEEP();
	CAMLreturn(result);
}

CAMLprim value
caml_plug_vif(value id)
{
	CAMLparam1(id);
	CAMLlocal1(result);
	struct ifnet *ifp;
	struct ifaddr *ifa;
	struct sockaddr_dl *sdl;
	struct plugged_if *pip;
	int found;
	u_char lladdr[8];

	pip = malloc(sizeof(struct plugged_if), M_MIRAGE, M_NOWAIT | M_ZERO);

	if (pip == NULL)
		caml_failwith("Out of memory");

	found = 0;
	IFNET_WLOCK();
	TAILQ_FOREACH(ifp, &V_ifnet, if_link) {
		IF_ADDR_RLOCK(ifp);
		if (strncmp(ifp->if_xname, String_val(id), IFNAMSIZ) == 0) {
			/* "Enable" the fake NetGraph node. */
			IFP2AC(ifp)->ac_netgraph = (void *) 1;
			pip->pi_index = ifp->if_index;
			pip->pi_flags = ifp->if_flags;
			bcopy(ifp->if_xname, pip->pi_xname, IFNAMSIZ);
			TAILQ_FOREACH(ifa, &ifp->if_addrhead, ifa_link) {
				sdl = (struct sockaddr_dl *) ifa->ifa_addr;
				if (sdl != NULL &&
				    sdl->sdl_family == AF_LINK &&
				    sdl->sdl_type == IFT_ETHER) {
					pip->pi_llindex = LLINDEX(sdl);
					bcopy(LLADDR(sdl), lladdr, sizeof(lladdr));
					found = 1;
					break;
				}
			}
			IF_ADDR_RUNLOCK(ifp);
			break;
		}
		IF_ADDR_RUNLOCK(ifp);
	}
	IFNET_WUNLOCK();

	if (!found) {
		free(pip, M_MIRAGE);
		caml_failwith("Invalid interface");
	}

	sprintf(pip->pi_lladdr, "%02x:%02x:%02x:%02x:%02x:%02x", lladdr[0],
	    lladdr[1], lladdr[2], lladdr[3], lladdr[4], lladdr[5]);

	mtx_init(&pip->pi_rx_lock, "plugged_if_rx", NULL, MTX_DEF);
	LIST_INIT(&pip->pi_rx_head);

	if (plugged == 0)
		TAILQ_INIT(&pihead);

	TAILQ_INSERT_TAIL(&pihead, pip, pi_next);
	plugged++;

	result = caml_alloc(3, 0);
	Store_field(result, 0, Val_bool(pip->pi_flags & IFF_UP));
	Store_field(result, 1, Val_int(pip->pi_llindex));
	Store_field(result, 2, caml_copy_string(pip->pi_lladdr));
	CAMLreturn(result);
}

CAMLprim value
caml_unplug_vif(value id)
{
	CAMLparam1(id);
	struct plugged_if *pip;
	struct ifnet *ifp;
	struct mbuf_entry *e1;
	struct mbuf_entry *e2;

	pip = find_pi_by_name(String_val(id));
	if (pip == NULL)
		CAMLreturn(Val_unit);

	IFNET_WLOCK();
	TAILQ_FOREACH(ifp, &V_ifnet, if_link) {
		if (strncmp(ifp->if_xname, String_val(id), IFNAMSIZ) == 0) {
			/* "Disable" the fake NetGraph node. */
			IFP2AC(ifp)->ac_netgraph = (void *) 0;
			break;
		}
	}
	IFNET_WUNLOCK();

	mtx_lock(&pip->pi_rx_lock);
	e1 = LIST_FIRST(&pip->pi_rx_head);
	while (e1 != NULL) {
		e2 = LIST_NEXT(e1, me_next);
		m_freem(e1->me_m);
		free(e1, M_MIRAGE);
		e1 = e2;
	}
	LIST_INIT(&pip->pi_rx_head);
	mtx_unlock(&pip->pi_rx_lock);

	TAILQ_REMOVE(&pihead, pip, pi_next);
	mtx_destroy(&pip->pi_rx_lock);
	free(pip, M_MIRAGE);
	plugged--;

	CAMLreturn(Val_unit);
}

/* Listening to incoming Ethernet frames. */
void
netif_ether_input(struct ifnet *ifp, struct mbuf **mp)
{
	struct plugged_if *pip;
	struct mbuf_entry *e;

	if (plugged == 0)
		return;

	pip = find_pi_by_index(ifp->if_index);
	if (pip == NULL)
		return;

	e = (struct mbuf_entry *) malloc(sizeof(struct mbuf_entry), M_MIRAGE,
	    M_NOWAIT);
	e->me_m = *mp;
	mtx_lock(&pip->pi_rx_lock);
	LIST_INSERT_HEAD(&pip->pi_rx_head, e, me_next);
	mtx_unlock(&pip->pi_rx_lock);
	*mp = NULL;
}

CAMLprim value
caml_get_mbufs(value id)
{
	CAMLparam1(id);
	CAMLlocal2(result, r);
	struct plugged_if *pip;
	struct mbuf_entry *e1;
	struct mbuf_entry *e2;
	struct mbuf *m;
	struct mbuf *n;

	result = Val_emptylist;

	if (plugged == 0)
		CAMLreturn(result);

	pip = find_pi_by_index(Int_val(id));
	if (pip == NULL)
		CAMLreturn(result);

	mtx_lock(&pip->pi_rx_lock);
	e1 = LIST_FIRST(&pip->pi_rx_head);
	while (e1 != NULL) {
		for (m = e1->me_m; m != NULL; m = m->m_nextpkt) {
			for (n = m; n != NULL; n = n->m_next) {
				r = caml_alloc(2, 0);
				Store_field(r, 0,
				    caml_ba_alloc_dims(CAML_BA_UINT8
				    | CAML_BA_C_LAYOUT | CAML_BA_MBUF, 1,
				    (void *) n, (long) n->m_len));
				Store_field(r, 1, result);
				result = r;
			}
		}
		e2 = LIST_NEXT(e1, me_next);
		free(e1, M_MIRAGE);
		e1 = e2;
	}
	LIST_INIT(&pip->pi_rx_head);
	mtx_unlock(&pip->pi_rx_lock);

	CAMLreturn(result);
}

/* Generating outgoing Ethernet data. */
int
netif_ether_output(struct ifnet *ifp, struct mbuf **mp)
{
	if (plugged == 0)
		return 0;

	return 0;
}
