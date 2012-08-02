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

#include <sys/time.h>

#include "caml/mlvalues.h"
#include "caml/alloc.h"

CAMLprim value kern_gettimeofday(value v_unit);
CAMLprim value kern_gmtime(value t);


CAMLprim value
kern_gettimeofday(value v_unit)
{
	struct timeval atv;

	microtime(&atv);
	return Val_long((long) atv.tv_sec * 1000000 + (long) atv.tv_usec);
}

#define	SPD	(24 * 60 * 60)

static const short dpm[13] =
  { 0
  , 31
  , 31 + 28
  , 31 + 28 + 31
  , 31 + 28 + 31 + 30
  , 31 + 28 + 31 + 30 + 31
  , 31 + 28 + 31 + 30 + 31 + 30
  , 31 + 28 + 31 + 30 + 31 + 30 + 31
  , 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31
  , 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30
  , 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31
  , 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30
  , 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31
  };

static int
is_leap(int year)
{
	return (!(year % 4) && ((year % 100) || !(year % 400)));
}

CAMLprim value
kern_gmtime(value t)
{
	time_t clock, x, k;
	int i, mday;
	value res;

	res = caml_alloc_small(9, 0);
	clock = (time_t) Long_val(t) / 1000000;
	x = clock % SPD;
	Field(res,0) = Val_int(x % 60); /* second */
	x /= 60;
	Field(res,1) = Val_int(x % 60); /* minute */
	Field(res,2) = Val_int(x / 60); /* hour */
	x = clock / SPD;
	Field(res,6) = Val_int((4 + x) % 7); /* wday */

	for (i = 1970; ; ++i) {
		k = is_leap(i) ? 366 : 365;
		if (x >= k)
			x -= k;
		else
			break;
	}

	Field(res,5) = Val_int(i - 1900); /* year */
	Field(res,7) = Val_int(x); /* yday */
	mday = 1;

	if (is_leap(i) && (x > 58)) {
		if (x == 59)
			mday = 2;
		x -= 1;
	}

	for (i = 11; i && (dpm[i] > x); --i);
	Field(res,4) = Val_int(i); /* mon */
	mday += x - dpm[i];
	Field(res,3) = Val_int(mday);
	Field(res,8) = Val_false;
	return res;
}
