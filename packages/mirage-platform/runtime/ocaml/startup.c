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

/* $Id: startup.c 10315 2010-04-27 07:55:08Z xleroy $ */

/* Start-up code */

#ifdef _KERNEL
#include <machine/setjmp.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif
#include "callback.h"
#include "backtrace.h"
#include "custom.h"
#include "fail.h"
#include "freelist.h"
#include "gc.h"
#include "gc_ctrl.h"
#include "memory.h"
#include "misc.h"
#include "mlvalues.h"
#include "osdeps.h"
#include "printexc.h"
#include "stack.h"
#include "sys.h"
#include "natdynlink.h"
#ifdef HAS_UI
#include "ui.h"
#endif

static void parse_camlrunparam(void);
static void scanmult (char *opt, uintnat *var);

extern int caml_parser_trace;
CAMLexport header_t caml_atom_table[256];
char * caml_code_area_start, * caml_code_area_end;

/* Initialize the atom table and the static data and code area limits. */

struct segment { char * begin; char * end; };

extern struct segment caml_data_segments[];
extern struct segment caml_code_segments[];

static void init_atoms(void)
{
  int i;

  for (i = 0; i < 256; i++) {
    caml_atom_table[i] = Make_header(0, i, Caml_white);
  }
  if (caml_page_table_add(In_static_data,
                          caml_atom_table, caml_atom_table + 256) != 0)
    caml_fatal_error("Fatal error: not enough memory for the initial page table");

  for (i = 0; caml_data_segments[i].begin != 0; i++) {
    if (caml_page_table_add(In_static_data,
                            caml_data_segments[i].begin,
                            caml_data_segments[i].end) != 0)
      caml_fatal_error("Fatal error: not enough memory for the initial page table");
  }

  caml_code_area_start = caml_code_segments[0].begin;
  caml_code_area_end = caml_code_segments[0].end;
  for (i = 1; caml_code_segments[i].begin != 0; i++) {
    if (caml_code_segments[i].begin < caml_code_area_start)
      caml_code_area_start = caml_code_segments[i].begin;
    if (caml_code_segments[i].end > caml_code_area_end)
      caml_code_area_end = caml_code_segments[i].end;
  }
}

/* Configuration parameters and flags */

static uintnat percent_free_init = Percent_free_def;
static uintnat max_percent_free_init = Max_percent_free_def;
static uintnat minor_heap_init = Minor_heap_def;
static uintnat heap_chunk_init = Heap_chunk_def;
static uintnat heap_size_init = Init_heap_def;
static uintnat max_stack_init = Max_stack_def;

/* Parse the CAMLRUNPARAM variable */
/* The option letter for each runtime option is the first letter of the
   last word of the ML name of the option (see [stdlib/gc.mli]).
   Except for l (maximum stack size) and h (initial heap size).
*/
/* Note: option l is irrelevant to the native-code runtime. */

/* If you change these functions, see also their copy in byterun/startup.c */

static void scanmult (char *opt, uintnat *var)
{
  char mult = ' ';
  int val;
  sscanf (opt, "=%u%c", &val, &mult);
  sscanf (opt, "=0x%x%c", &val, &mult);
  switch (mult) {
  case 'k':   *var = (uintnat) val * 1024; break;
  case 'M':   *var = (uintnat) val * 1024 * 1024; break;
  case 'G':   *var = (uintnat) val * 1024 * 1024 * 1024; break;
  default:    *var = (uintnat) val; break;
  }
}

#ifdef _KERNEL
extern char mir_rtparams[64];
#endif

static void parse_camlrunparam(void)
{
#ifdef _KERNEL
  char *opt;
  uintnat p;

  opt = mir_rtparams;
#else
  char *opt = getenv ("OCAMLRUNPARAM");
  uintnat p;

  if (opt == NULL) opt = getenv ("CAMLRUNPARAM");
#endif

  if (opt != NULL){
    while (*opt != '\0'){
      switch (*opt++){
      case 's': scanmult (opt, &minor_heap_init); break;
      case 'i': scanmult (opt, &heap_chunk_init); break;
      case 'h': scanmult (opt, &heap_size_init); break;
      case 'l': scanmult (opt, &max_stack_init); break;
      case 'o': scanmult (opt, &percent_free_init); break;
      case 'O': scanmult (opt, &max_percent_free_init); break;
      case 'v': scanmult (opt, &caml_verb_gc); break;
      case 'b': caml_record_backtrace(Val_true); break;
      case 'p': caml_parser_trace = 1; break;
      case 'a': scanmult (opt, &p); caml_set_allocation_policy (p); break;
      }
    }
  }
}

/* These are termination hooks used by the systhreads library */
struct longjmp_buffer caml_termination_jmpbuf;
void (*caml_termination_hook)(void *) = NULL;

extern value caml_start_program (void);
#ifdef _KERNEL
#else
extern void caml_init_ieee_floats (void);
#endif
extern void caml_init_signals (void);

void caml_main(char **argv)
{
  char * exe_name;
#ifdef __linux__
  static char proc_self_exe[256];
#endif
  value res;
  char tos;

#ifdef _KERNEL
#else
  caml_init_ieee_floats();
#endif
  caml_init_custom_operations();
#ifdef DEBUG
  caml_verb_gc = 63;
#endif
  caml_top_of_stack = &tos;
  parse_camlrunparam();
  caml_init_gc (minor_heap_init, heap_size_init, heap_chunk_init,
                percent_free_init, max_percent_free_init);
  init_atoms();
  caml_init_signals();
#ifndef _KERNEL
  caml_debugger_init (); /* force debugger.o stub to be linked */
#endif
  exe_name = argv[0];
  if (exe_name == NULL) exe_name = "";
#ifdef __linux__
  if (caml_executable_name(proc_self_exe, sizeof(proc_self_exe)) == 0)
    exe_name = proc_self_exe;
  else
    exe_name = caml_search_exe_in_path(exe_name);
#elif _KERNEL
  exe_name = "mirage.ko";
#else
  exe_name = caml_search_exe_in_path(exe_name);
#endif
  caml_sys_init(exe_name, argv);
  if (sigsetjmp(caml_termination_jmpbuf.buf, 0)) {
    if (caml_termination_hook != NULL) caml_termination_hook(NULL);
    return;
  }
  res = caml_start_program();
  if (Is_exception_result(res))
    caml_fatal_uncaught_exception(Extract_exception(res));
}

void caml_startup(char **argv)
{
  caml_main(argv);
}
