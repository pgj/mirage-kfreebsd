(*-
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
 *)

open Bigarray

type t = (char, int8_unsigned_elt, c_layout) Array1.t

external alloc_pages: int -> t array = "caml_alloc_pages"

let page_size = 4096

let get () = Array.get (alloc_pages 1) 0

let rec get_n = function
  | 0 -> []
  | n -> get () :: (get_n (n - 1))

let sub t off len = Array1.sub t off len

let length t = Array1.dim t

let to_pages t =
  assert ((length t) mod page_size == 0);
  let rec loop off acc =
    if (off < (length t))
      then loop (off + page_size) (sub t off page_size :: acc)
      else acc
  in
  List.rev (loop 0 [])

let string_blit src t =
  for i = 0 to ((String.length src) - 1) do
    t.{i} <- src.[i]
  done

let to_string t =
  let result = String.create (length t) in
  for i = 0 to ((length t) - 1) do
    result.[i] <- t.{i}
  done;
  result

let blit src dest = Array1.blit src dest
