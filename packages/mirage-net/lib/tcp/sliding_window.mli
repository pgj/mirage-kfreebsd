(*
 * Copyright (c) 2012 Anil Madhavapeddy <anil@recoil.org>
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

type t

val create : isn:Sequence.t -> mss:Sequence.t -> t

val get_l : t -> Sequence.t
val get_m : t -> Sequence.t
val get_r : t -> Sequence.t

val add_l : t -> Sequence.t -> unit
val add_m : t -> Sequence.t -> unit
val add_r : t -> Sequence.t -> unit

val set_l : t -> Sequence.t -> unit
val set_m : t -> Sequence.t -> unit
val set_r : t -> Sequence.t -> unit

val get_l_m : t -> Sequence.t
val get_l_r : t -> Sequence.t
val get_m_r : t -> Sequence.t

val to_string : t -> string
