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

type sring = {
    buf: Io_page.t;
    header_size: int;
    slot_size: int;
    nr_ents: int;
}

let sring_hdr_size = 64 (* sizeof(struct sring_hdr) *)

cstruct sring_hdr {
    uint16_t sr_cur;
    uint16_t sr_avail;
    uint16_t sr_size;
    uint16_t sr_slotsize;
    uint8_t  sr_pad[56]
} as little_endian

let sslot_size = 2 (* sizeof(struct sslot) *)

cstruct sslot {
    uint16_t ss_len
} as little_endian

let nr_ents sring = sring.nr_ents

let avail_packet sring =
  let hdr = Io_page.sub sring.buf 0 sring.header_size in
  get_sring_hdr_sr_avail hdr

let slot sring i =
  let idx = i land (sring.nr_ents - 1) in
  let off = sring.header_size + (idx * sslot_size) in
  let s = Io_page.sub sring.buf off sslot_size in
  let size = get_sslot_ss_len s in
  let off = sring.header_size + (sring.nr_ents * sslot_size) +
    (idx * sring.slot_size) in
  Io_page.sub sring.buf off size

let top_packet sring = slot sring (get_sring_hdr_sr_cur (sring.buf))

let pop_packet sring =
  let b = sring.buf in
  let i = get_sring_hdr_sr_cur b in
  let n = get_sring_hdr_sr_avail b in
  set_sring_hdr_sr_cur b ((i + 1) land (sring.nr_ents - 1));
  set_sring_hdr_sr_avail b (n - 1);
  return ()

let of_buf buf slot_size =
  let power_of_2 x =
    let x = x lor (x lsr 1) in
    let x = x lor (x lsr 2) in
    let x = x lor (x lsr 4) in
    let x = x lor (x lsr 8) in
    let x = x lor (x lsr 16) in
    let x = x lor (x lsr 32) in
    x - (x lsr 1)
  in
  let header_size = sring_hdr_size in
  let total = Io_page.length buf in
  let free_bytes = total - header_size in
  let nr_ents = power_of_2 (free_bytes / slot_size) in
  assert (header_size + (nr_ents * (sslot_size + slot_size)) < total);
  set_sring_hdr_sr_cur buf 0;
  set_sring_hdr_sr_avail buf 0;
  set_sring_hdr_sr_size buf nr_ents;
  set_sring_hdr_sr_slotsize buf slot_size;
  { buf; header_size; slot_size; nr_ents }

type t = {
    backend_id: int;
    backend: string;
    mac: string;
    mutable active: bool;
    rx_ring: sring;
}

type id = string

external get_vifs: unit -> id list = "caml_get_vifs"
external plug_vif: id -> Io_page.t -> bool * int * string = "caml_plug_vif"
external unplug_vif: id -> unit = "caml_unplug_vif"

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
    let buf = Io_page.get ~pages_per_block:33 () in
    let rx_ring = of_buf buf 2048 in
    let active,backend_id,mac = plug_vif id buf in
    let t = { backend_id; backend; mac; active; rx_ring } in
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
  while_lwt (avail_packet ifc.rx_ring > 0) do
    fn (top_packet ifc.rx_ring) >>
    pop_packet ifc.rx_ring
  done

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
