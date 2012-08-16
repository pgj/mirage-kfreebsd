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
#include <sys/mbuf.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/systm.h>
#include <sys/libkern.h>
#include <sys/sdt.h>

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

#define MBUF_LEN(mb)	((mb)->m_pkthdr.len)

SDT_PROVIDER_DECLARE(mirage);

SDT_PROBE_DEFINE(mirage, kernel, netif, plug_vif, plug_vif);
SDT_PROBE_ARGTYPE(mirage, kernel, netif, plug_vif, 0, "int");
SDT_PROBE_ARGTYPE(mirage, kernel, netif, plug_vif, 1, "int");


struct sring_hdr {
	uint16_t	sr_cur;
	uint16_t	sr_avail;
	uint16_t	sr_size;
	uint16_t	sr_slotsize;
	uint8_t		sr_pad[56];
};

struct sslot {
	uint16_t	ss_len;
};

struct plugged_if {
	TAILQ_ENTRY(plugged_if)	pi_next;
	u_short	pi_index;
	u_short	pi_llindex;
	int	pi_flags;
	char	pi_lladdr[32];
	char	pi_xname[IFNAMSIZ];
	void	*pi_rx;
};

TAILQ_HEAD(plugged_ifhead, plugged_if) pihead =
    TAILQ_HEAD_INITIALIZER(pihead);

int plugged = 0;


/* Currently only Ethernet interfaces are returned. */
CAMLprim value caml_get_vifs(value v_unit);
CAMLprim value caml_plug_vif(value id, value rxring);
CAMLprim value caml_unplug_vif(value id);

void netif_ether_input(struct ifnet *ifp, struct mbuf **mp);
int  netif_ether_output(struct ifnet *ifp, struct mbuf **mp);


CAMLprim value
caml_get_vifs(value v_unit)
{
	CAMLparam1(v_unit);
	CAMLlocal2(result, r);
	struct ifnet *ifp;
	struct ifaddr *ifa;
	struct sockaddr_dl *sdl;

	result = Val_emptylist
	CURVNET_SET(TD_TO_VNET(curthread));;
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
	CURVNET_RESTORE();
	CAMLreturn(result);
}

CAMLprim value
caml_plug_vif(value id, value rxring)
{
	CAMLparam2(id, rxring);
	CAMLlocal1(result);
	struct ifnet *ifp;
	struct ifaddr *ifa;
	struct sockaddr_dl *sdl;
	struct plugged_if *pip;
	int found;
	u_char lladdr[8];

	pip = malloc(sizeof(struct plugged_if), M_MIRAGE, M_NOWAIT);

	if (pip == NULL)
		caml_failwith("Out of memory");

	found = 0;
	CURVNET_SET(TD_TO_VNET(curthread));
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
	CURVNET_RESTORE();

	if (!found) {
		free(pip, M_MIRAGE);
		caml_failwith("Invalid interface");
	}

	sprintf(pip->pi_lladdr, "%02x:%02x:%02x:%02x:%02x:%02x", lladdr[0],
	    lladdr[1], lladdr[2], lladdr[3], lladdr[4], lladdr[5]);

	pip->pi_rx = Caml_ba_data_val(rxring);

	SDT_PROBE(mirage, kernel, netif, plug_vif,
	    ((struct sring_hdr *) pip->pi_rx)->sr_size,
	    ((struct sring_hdr *) pip->pi_rx)->sr_slotsize, 0, 0, 0);

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
	int found;

	found = 0;
	TAILQ_FOREACH(pip, &pihead, pi_next) {
		if (strncmp(pip->pi_xname, String_val(id), IFNAMSIZ) == 0) {
			found = 1;
			break;
		}
	}

	if (!found)
		CAMLreturn(Val_unit);

	CURVNET_SET(TD_TO_VNET(curthread));
	IFNET_WLOCK();
	TAILQ_FOREACH(ifp, &V_ifnet, if_link) {
		if (strncmp(ifp->if_xname, String_val(id), IFNAMSIZ) == 0) {
			/* "Disable" the fake NetGraph node. */
			IFP2AC(ifp)->ac_netgraph = (void *) 0;
			break;
		}
	}
	IFNET_WUNLOCK();
	CURVNET_RESTORE();

	TAILQ_REMOVE(&pihead, pip, pi_next);
	free(pip, M_MIRAGE);
	plugged--;

	CAMLreturn(Val_unit);
}

/* Listening to incoming Ethernet frames. */
void
netif_ether_input(struct ifnet *ifp, struct mbuf **mp)
{
	u_int len;
	struct plugged_if *pip;
	struct sring_hdr *h;
	struct sslot *s;
	char *r;
	int i, sz;

	if (plugged == 0)
		return;

	TAILQ_FOREACH(pip, &pihead, pi_next) {
		if (ifp->if_index == pip->pi_index) {
			h = pip->pi_rx;
			sz = (h->sr_size - 1);
			i = (h->sr_cur + h->sr_avail) & sz;
			r = (char *) pip->pi_rx + (h->sr_size *
			    sizeof(struct sslot)) + (i * h->sr_slotsize);
			len = MBUF_LEN(*mp);
			s = (struct sslot *) ((char *) pip->pi_rx +
			    sizeof(struct sring_hdr));
			s[i].ss_len = len;
			m_copydata(*mp, 0, len, r);
			h->sr_avail++;
			m_freem(*mp);
			*mp = NULL;
			break;
		}
	}
}

/* Generating outgoing Ethernet data. */
int
netif_ether_output(struct ifnet *ifp, struct mbuf **mp)
{
	if (plugged == 0)
		return 0;

	return 0;
}
