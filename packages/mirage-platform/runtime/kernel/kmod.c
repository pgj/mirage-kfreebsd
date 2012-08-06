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

#include <sys/param.h>
#include <sys/proc.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/errno.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/kthread.h>
#include <sys/sched.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/sysctl.h>

#include "caml/mlvalues.h"
#include "caml/callback.h"
#include "caml/memory.h"

CAMLprim value caml_block_kernel(value v_timeout);


static char mir_rtparams[64] = "";
static int mir_debug = 3;

#define MIR_DEBUG(l, x)			\
	do {				\
		if (mir_debug >= l) {	\
			(x);		\
		}			\
	} while (0)

static char* argv[] = { "mirage", NULL };

MALLOC_DEFINE(M_MIRAGE, "mirage", "Mirage run-time");

static SYSCTL_NODE(_kern, OID_AUTO, mirage, CTLFLAG_RD, NULL, "mirage");

enum thread_state {
	THR_NONE,
	THR_RUNNING,
	THR_STOPPED
};

static enum thread_state mirage_kthread_state = THR_NONE;

static struct thread *mirage_kthread = NULL;

static void
mirage_kthread_body(void *arg __unused)
{
	value *v_main;
	int caml_completed = 0;

	mirage_kthread_state = THR_RUNNING;
	MIR_DEBUG(1, printf("--> mirage_kthread_body()\n"));
	MIR_DEBUG(2, printf("mirage: kernel thread actived, caml run-time starts\n"));
	caml_startup(argv);
	MIR_DEBUG(2, printf("mirage: caml run-time finished, searching the main function\n"));
	v_main = caml_named_value("OS.Main.run");

	if (v_main == NULL) {
		printf("[MIRAGE] Function 'OS.Main.run' could not be found.\n");
		goto done;
	}

	MIR_DEBUG(2, printf("mirage: main function found, kicking off the main loop\n"));
	for (; (caml_completed == 0) && (mirage_kthread_state == THR_RUNNING);) {
		caml_completed = Bool_val(caml_callback(*v_main, Val_unit));
	}
	MIR_DEBUG(2, printf("mirage: main loop exited = (%d,%d)\n",
	    caml_completed, (int) mirage_kthread_state));

done:
	MIR_DEBUG(2, printf("mirage: kernel thread exiting\n"));
	if (mirage_kthread_state == THR_STOPPED)
		wakeup(&mirage_kthread_state);
	mirage_kthread_state = THR_NONE;
	kthread_exit();
	MIR_DEBUG(1, printf("<-- mirage_kthread_body()\n"));
}

static int
mirage_kthread_init(void)
{
	int error;

	error = 0;
	MIR_DEBUG(1, printf("--> mirage_kthread_init()\n"));
	error = kthread_add(mirage_kthread_body, NULL, NULL, &mirage_kthread,
	    RFSTOPPED, 40, "mirage");
	mirage_kthread_state = THR_STOPPED;
	if (error != 0) {
		printf("[MIRAGE] Could not create herding kernel thread.\n");
		goto done;
	}

done:
	MIR_DEBUG(1, printf("<-- mirage_kthread_init()\n"));
	return error;
}

static int
mirage_kthread_deinit(void)
{
	MIR_DEBUG(1, printf("--> mirage_kthread_deinit()\n"));
	if (mirage_kthread_state == THR_RUNNING) {
		mirage_kthread_state = THR_STOPPED;
		tsleep((void *) &mirage_kthread_state, 0,
		    "mirage_kthread_deinit", 0);
		pause("mirage_kthread_deinit", 1);
	}
	MIR_DEBUG(1, printf("<-- mirage_kthread_deinit()\n"));
	return 0;
}

static void
mirage_kthread_launch(void)
{
	MIR_DEBUG(1, printf("--> mirage_kthread_launch()\n"));
	thread_lock(mirage_kthread);
	sched_add(mirage_kthread, SRQ_BORING);
	sched_class(mirage_kthread, PRI_TIMESHARE);
	sched_prio(mirage_kthread, PRI_MAX_IDLE);
	thread_unlock(mirage_kthread);
	MIR_DEBUG(1, printf("<-- mirage_kthread_launch()\n"));
}

static int
sysctl_kern_mirage_run(SYSCTL_HANDLER_ARGS)
{
	int error, i;

	error = sysctl_wire_old_buffer(req, sizeof(int));
	if (error == 0) {
		i = 0;
		error = sysctl_handle_int(oidp, &i, 0, req);
	}
	if (error != 0 || req->newptr == NULL)
		return (error);
	{
		if (mirage_kthread_state == THR_NONE) {
			mirage_kthread_init();
			mirage_kthread_launch();
		}
		else {
			MIR_DEBUG(1, printf("mirage: a kernel thread has been already launched.\n"));
		}
	}
	return (0);
}

SYSCTL_PROC(_kern_mirage, OID_AUTO, run, CTLTYPE_INT | CTLFLAG_RW, 0,
    sizeof(int), sysctl_kern_mirage_run, "I", "start module");

SYSCTL_INT(_kern_mirage, OID_AUTO, debug, CTLFLAG_RW, &mir_debug, 0,
    "debug level");

SYSCTL_STRING(_kern_mirage, OID_AUTO, rtparams, CTLFLAG_RW, &mir_rtparams,
    sizeof(mir_rtparams), "parameters for the run-time");

static int
event_handler(struct module *module, int event, void *arg) {
	int retval;

	retval = 0;

	switch (event) {
	case MOD_LOAD:
		printf("[MIRAGE] Kernel module is about to load.\n");
		break;
	case MOD_UNLOAD:
		printf("[MIRAGE] Kernel module is about to unload.\n");
		retval = mirage_kthread_deinit();
		break;
	default:
		retval = EOPNOTSUPP;
		break;
	}

	return retval;
}

static moduledata_t mirage_conf = {
    "mirage"
,   event_handler
,   NULL
};

DECLARE_MODULE(mirage, mirage_conf, SI_SUB_KLD, SI_ORDER_ANY);

static int block_timo;

CAMLprim value
caml_block_kernel(value v_timeout)
{
	CAMLparam1(v_timeout);
	block_timo = Int_val(v_timeout);
	MIR_DEBUG(2, printf("mirage: Blocking kernel for %d us\n", block_timo));
	pause("caml_block_kernel", (block_timo * hz) / 1000000);
	CAMLreturn(Val_unit);
}
