/***********************************************************************/
/*                                                                     */
/*                           Objective Caml                            */
/*                                                                     */
/*         Xavier Leroy and Damien Doligez, INRIA Rocquencourt         */
/*                                                                     */
/*  Copyright 1996 Institut National de Recherche en Informatique et   */
/*  en Automatique.  All rights reserved.  This file is distributed    */
/*  under the terms of the GNU Library General Public License, with    */
/*  the special exception on linking described in file ../LICENSE.     */
/*                                                                     */
/***********************************************************************/

/* $Id: misc.c 8822 2008-02-29 12:56:15Z doligez $ */

#ifdef _KERNEL
#else
#include <stdio.h>
#endif
#include "config.h"
#include "misc.h"
#include "memory.h"

#ifdef DEBUG

int caml_failed_assert (char * expr, char * file, int line)
{
#ifdef _KERNEL
  printf ("file %s; line %d ### Assertion failed: %s\n",
           file, line, expr);
  panic("caml_failed_assert");
#else
  fprintf (stderr, "file %s; line %d ### Assertion failed: %s\n",
           file, line, expr);
  exit (100);
#endif
  return 1; /* not reached */
}

void caml_set_fields (char *bp, unsigned long start, unsigned long filler)
{
  mlsize_t i;
  for (i = start; i < Wosize_bp (bp); i++){
    Field (Val_bp (bp), i) = (value) filler;
  }
}

#endif /* DEBUG */

uintnat caml_verb_gc = 0;

void caml_gc_message (int level, char *msg, uintnat arg)
{
  if (level < 0 || (caml_verb_gc & level) != 0){
#ifdef _KERNEL
    printf(msg, arg);
#else
    fprintf (stderr, msg, arg);
#endif
  }
}

CAMLexport void caml_fatal_error (char *msg)
{
#ifdef _KERNEL
  printf("%s", msg);
  panic("caml_fatal_error");
#else
  fprintf (stderr, "%s", msg);
  exit(2);
#endif
}

CAMLexport void caml_fatal_error_arg (char *fmt, char *arg)
{
#ifdef _KERNEL
  printf(fmt, arg);
  panic("caml_fatal_error_arg");
#else
  fprintf (stderr, fmt, arg);
  exit(2);
#endif
}

CAMLexport void caml_fatal_error_arg2 (char *fmt1, char *arg1,
                                       char *fmt2, char *arg2)
{
#ifdef _KERNEL
  printf(fmt1, arg1);
  printf(fmt2, arg2);
  panic("caml_fatal_error_arg2");
#else
  fprintf (stderr, fmt2, arg2);
  exit(2);
#endif
}

char *caml_aligned_malloc_for_minor (asize_t size, int modulo, void **block)
{
  char *raw_mem;
  uintnat aligned_mem;
                                                  Assert (modulo < Page_size);
  raw_mem = (char *) __malloc (size + Page_size);
  if (raw_mem == NULL) return NULL;
  *block = raw_mem;
  raw_mem += modulo;                /* Address to be aligned */
  aligned_mem = (((uintnat) raw_mem / Page_size + 1) * Page_size);
#ifdef DEBUG
  {
    uintnat *p;
    uintnat *p0 = (void *) *block,
            *p1 = (void *) (aligned_mem - modulo),
            *p2 = (void *) (aligned_mem - modulo + size),
            *p3 = (void *) ((char *) *block + size + Page_size);

    for (p = p0; p < p1; p++) *p = Debug_filler_align;
    for (p = p1; p < p2; p++) *p = Debug_uninit_align;
    for (p = p2; p < p3; p++) *p = Debug_filler_align;
  }
#endif
  return (char *) (aligned_mem - modulo);
}

char *caml_aligned_malloc_for_major (asize_t size, int modulo, void **block)
{
  char *raw_mem;
  uintnat aligned_mem;
                                                  Assert (modulo < Page_size);
  raw_mem = (char *) __malloc (size + Page_size);
  if (raw_mem == NULL) return NULL;
  *block = raw_mem;
  raw_mem += modulo;                /* Address to be aligned */
  aligned_mem = (((uintnat) raw_mem / Page_size + 1) * Page_size);
#ifdef DEBUG
  {
    uintnat *p;
    uintnat *p0 = (void *) *block,
            *p1 = (void *) (aligned_mem - modulo),
            *p2 = (void *) (aligned_mem - modulo + size),
            *p3 = (void *) ((char *) *block + size + Page_size);

    for (p = p0; p < p1; p++) *p = Debug_filler_align;
    for (p = p1; p < p2; p++) *p = Debug_uninit_align;
    for (p = p2; p < p3; p++) *p = Debug_filler_align;
  }
#endif
  return (char *) (aligned_mem - modulo);
}

void caml_ext_table_init(struct ext_table * tbl, int init_capa)
{
  tbl->size = 0;
  tbl->capacity = init_capa;
  tbl->contents = caml_stat_alloc(sizeof(void *) * init_capa);
}

int caml_ext_table_add(struct ext_table * tbl, void * data)
{
  int res;
  if (tbl->size >= tbl->capacity) {
    tbl->capacity *= 2;
    tbl->contents =
      caml_stat_resize(tbl->contents, sizeof(void *) * tbl->capacity);
  }
  res = tbl->size;
  tbl->contents[res] = data;
  tbl->size++;
  return res;
}

void caml_ext_table_free(struct ext_table * tbl, int free_entries)
{
  int i;
  if (free_entries)
    for (i = 0; i < tbl->size; i++) caml_stat_free(tbl->contents[i]);
  caml_stat_free(tbl->contents);
}
