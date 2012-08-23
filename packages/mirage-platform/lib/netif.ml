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
    backend_id: int;
    backend: string;
    mac: string;
    mutable active: bool;
}

type id = string

external get_vifs: unit -> id list = "caml_get_vifs"
external plug_vif: id -> bool * int * string = "caml_plug_vif"
external unplug_vif: id -> unit = "caml_unplug_vif"
external get_mbufs : int -> Io_page.t list = "caml_get_mbufs"

let devices : (id, t) Hashtbl.t = Hashtbl.create 1

let enumerate () =
  let vifs = get_vifs () in
  let rec read_vif l acc = match l with
    | []      -> return acc
    | (x::xs) ->
      lwt sid = return x in
      read_vif xs (sid :: acc)
  in
  read_vif vifs []

let plug id =
  try
    return (Hashtbl.find devices id)
  with Not_found ->
    let backend = id in
    let active,backend_id,mac = plug_vif id in
    let t = { backend_id; backend; mac; active } in
    Hashtbl.add devices id t;
    return t

let unplug id =
 try
   let t = Hashtbl.find devices id in
   t.active <- false;
   Hashtbl.remove devices id;
   unplug_vif id
 with Not_found -> ()

let create f =
  let th,_ = Lwt.task () in
  Lwt.on_cancel th (fun _ -> Hashtbl.iter (fun id _ -> unplug id) devices);
  lwt ids = enumerate () in
  let pt = Lwt_list.iter_p (fun id ->
    lwt t = plug id in
    f id t) ids in
  th <?> pt

let write ifc page =
  Console.log (sprintf "Netif.write %s: not implemented yet" ifc.backend);
  return ()

let writev ifc pages =
  Console.log (sprintf "Netif.writev %s: not implemented yet" ifc.backend);
  return ()

let rx_poll ifc fn =
  let mbufs = get_mbufs ifc.backend_id in
  Lwt_list.iter_s (fun m ->
    try_lwt fn m
    with exn ->
      return (printf "Exception on receive: %s\n%!" (Printexc.to_string exn)))
  mbufs

let listen ifc fn =
  let rec poll_t () =
    rx_poll ifc fn  >>
    Time.sleep 1000 >>
    poll_t ()
  in
  match ifc.active with
    | true  -> poll_t ()
    | false -> return ()

let mac ifc =
  let s = String.create 6 in
  Scanf.sscanf ifc.mac "%02x:%02x:%02x:%02x:%02x:%02x"
    (fun a b c d e f ->
      s.[0] <- Char.chr a;
      s.[1] <- Char.chr b;
      s.[2] <- Char.chr c;
      s.[3] <- Char.chr d;
      s.[4] <- Char.chr e;
      s.[5] <- Char.chr f;
    );
  s

let ethid ifc = string_of_int ifc.backend_id

let get_writebuf ifc =
  let page = Io_page.get () in
  return page
