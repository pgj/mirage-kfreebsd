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
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/sdt.h>

#include "caml/misc.h"
#include "caml/mlvalues.h"
#include "caml/memory.h"
#include "caml/alloc.h"
#include "caml/fail.h"
#include "caml/bigarray.h"

SDT_PROVIDER_DECLARE(mirage);

SDT_PROBE_DEFINE(mirage, kernel, alloc_pages, entry, entry);
SDT_PROBE_ARGTYPE(mirage, kernel, alloc_pages, entry, 0, "size_t");
SDT_PROBE_DEFINE(mirage, kernel, alloc_pages, return, return);
SDT_PROBE_DEFINE(mirage, kernel, io_page, contigmalloc, contigmalloc);
SDT_PROBE_ARGTYPE(mirage, kernel, io_page, contigmalloc, 0, "size_t");
SDT_PROBE_ARGTYPE(mirage, kernel, io_page, contigmalloc, 1, "unsigned long");

CAMLprim value caml_alloc_pages(value n_pages);

CAMLprim value
caml_alloc_pages(value n_pages)
{
	CAMLparam1(n_pages);
	CAMLlocal2(page, result);
	int i;
	size_t len;
	unsigned long block;

	len = Int_val(n_pages);
	SDT_PROBE(mirage, kernel, alloc_pages, entry, len, 0, 0, 0, 0);
	block = (unsigned long) contigmalloc(PAGE_SIZE * len, M_MIRAGE,
	    M_NOWAIT, 0, 0xffffffff, PAGE_SIZE, 0ul);
	SDT_PROBE(mirage, kernel, io_page, contigmalloc, PAGE_SIZE * len, block, 0, 0, 0);
	if (block == 0)
		caml_failwith("contigmalloc");
	result = caml_alloc(len, 0);
	for (i = 0; i < len; i++) {
		page = caml_ba_alloc_dims(CAML_BA_UINT8 | CAML_BA_C_LAYOUT
		    | CAML_BA_MANAGED, 1, (void *) block, (long) PAGE_SIZE);
		Store_field(result, i, page);
		block += (PAGE_SIZE / sizeof(unsigned long));
	};
	SDT_PROBE(mirage, kernel, alloc_pages, return, 0, 0, 0, 0, 0);
	CAMLreturn(result);
}
