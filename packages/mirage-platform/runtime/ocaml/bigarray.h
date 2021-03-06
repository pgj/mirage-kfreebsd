/***********************************************************************/
/*                                                                     */
/*                           Objective Caml                            */
/*                                                                     */
/*         Manuel Serrano and Xavier Leroy, INRIA Rocquencourt         */
/*                                                                     */
/*  Copyright 2000 Institut National de Recherche en Informatique et   */
/*  en Automatique.  All rights reserved.  This file is distributed    */
/*  under the terms of the GNU Library General Public License, with    */
/*  the special exception on linking described in file ../../LICENSE.  */
/*                                                                     */
/***********************************************************************/

/* $Id: bigarray.h 9153 2008-12-03 18:09:09Z doligez $ */

#ifndef CAML_BIGARRAY_H
#define CAML_BIGARRAY_H

#ifndef CAML_NAME_SPACE
#include "compatibility.h"
#endif
#include "config.h"
#include "mlvalues.h"

typedef signed char caml_ba_int8;
typedef unsigned char caml_ba_uint8;
#if SIZEOF_SHORT == 2
typedef short caml_ba_int16;
typedef unsigned short caml_ba_uint16;
#else
#error "No 16-bit integer type available"
#endif

#define CAML_BA_MAX_NUM_DIMS 16

enum caml_ba_kind {
#ifdef _KERNEL
#else
  CAML_BA_FLOAT32,             /* Single-precision floats */
  CAML_BA_FLOAT64,             /* Double-precision floats */
#endif
  CAML_BA_SINT8,               /* Signed 8-bit integers */
  CAML_BA_UINT8,               /* Unsigned 8-bit integers */
  CAML_BA_SINT16,              /* Signed 16-bit integers */
  CAML_BA_UINT16,              /* Unsigned 16-bit integers */
  CAML_BA_INT32,               /* Signed 32-bit integers */
  CAML_BA_INT64,               /* Signed 64-bit integers */
  CAML_BA_CAML_INT,            /* Caml-style integers (signed 31 or 63 bits) */
  CAML_BA_NATIVE_INT,       /* Platform-native long integers (32 or 64 bits) */
#ifdef _KERNEL
#else
  CAML_BA_COMPLEX32,           /* Single-precision complex */
  CAML_BA_COMPLEX64,           /* Double-precision complex */
#endif
  CAML_BA_KIND_MASK = 0xFF     /* Mask for kind in flags field */
};

enum caml_ba_layout {
  CAML_BA_C_LAYOUT = 0,           /* Row major, indices start at 0 */
  CAML_BA_FORTRAN_LAYOUT = 0x100, /* Column major, indices start at 1 */
  CAML_BA_LAYOUT_MASK = 0x100  /* Mask for layout in flags field */
};

enum caml_ba_managed {
  CAML_BA_EXTERNAL = 0,        /* Data is not allocated by Caml */
  CAML_BA_MANAGED = 0x200,     /* Data is allocated by Caml */
  CAML_BA_MAPPED_FILE = 0x400, /* Data is a memory mapped file */
#ifdef _KERNEL
  CAML_BA_MBUF = 0x800,        /* Data is a FreeBSD mbuf(9) */
  CAML_BA_MANAGED_MASK = 0xE00 /* Mask for "managed" bits in flags field */
#else
  CAML_BA_MANAGED_MASK = 0x600 /* Mask for "managed" bits in flags field */
#endif
};

struct caml_ba_proxy {
  intnat refcount;              /* Reference count */
  void * data;                  /* Pointer to base of actual data */
  uintnat size;                 /* Size of data in bytes (if mapped file) */
};

struct caml_ba_array {
  void * data;                /* Pointer to raw data */
#ifdef _KERNEL
  void * data2;               /* Interoperating with mbuf(9) or NULL */
#endif
  intnat num_dims;            /* Number of dimensions */
  intnat flags;  /* Kind of element array + memory layout + allocation status */
  struct caml_ba_proxy * proxy; /* The proxy for sub-arrays, or NULL */
  intnat dim[1] /*[num_dims]*/; /* Size in each dimension */
};

#define Caml_ba_array_val(v) ((struct caml_ba_array *) Data_custom_val(v))

#define Caml_ba_data_val(v) (Caml_ba_array_val(v)->data)

#if defined(IN_OCAML_BIGARRAY)
#define CAMLBAextern CAMLexport
#else
#define CAMLBAextern CAMLextern
#endif

CAMLBAextern value
    caml_ba_alloc(int flags, int num_dims, void * data, intnat * dim);
CAMLBAextern value caml_ba_alloc_dims(int flags, int num_dims, void * data,
                                 ... /*dimensions, with type intnat */);
CAMLBAextern uintnat caml_ba_byte_size(struct caml_ba_array * b);

#endif
