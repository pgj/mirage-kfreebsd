(*
 * Copyright (c) 2011-2012 Anil Madhavapeddy <anil@recoil.org>
 * Copyright (c) 2012 Gabor Pali
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *)

open Bigarray

type t = (char, int8_unsigned_elt, c_layout) Array1.t

external alloc_pages: int -> t = "caml_alloc_pages"

let page_size = 4096

let get ?(pages_per_block = 1) () = alloc_pages pages_per_block

let rec get_n ?(pages_per_block = 1) n = match n with
  | 0 -> []
  | n -> get () :: (get_n ~pages_per_block (n - 1))

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
