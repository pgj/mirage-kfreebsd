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

open Lwt
open Printf


type t = {
    backend_id : int;
    backend    : string;
}

type id = string

let plug id =
  Console.log (sprintf "Netif.plug %s: not implemented yet" id);
  lwt backend_id = return 0 in
  lwt backend = return id in
  let t = { backend_id; backend } in
  return t

let unplug id =
  Console.log (sprintf "Netif.unplug %s: not implemented yet" id);
  ()

let create fn =
  Console.log (sprintf "Netif.create: not implemented yet");
  return ()

let write nf page =
  Console.log (sprintf "Netif.write %s: not implemented yet" nf.backend);
  return ()

let writev nf pages =
  Console.log (sprintf "Netif.writev %s: not implemented yet" nf.backend);
  return ()

let listen nf fn =
  Console.log (sprintf "Netif.listen %s: not implemented yet" nf.backend);
  return ()

let enumerate () =
  Console.log (sprintf "Netif.enumerate: not implemented yet");
  return []

let mac nf =
  Console.log (sprintf "Netif.mac %s: not implemented yet" nf.backend);
  ""

let ethid nf =
  string_of_int nf.backend_id

let get_writebuf nf =
  let page = Io_page.get() in
  return page
