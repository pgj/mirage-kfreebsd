open Lwt
open Printf

cstruct ethernet {
  uint8_t   dst[6];
  uint8_t   src[6];
  uint16_t  ethertype
} as big_endian

let show_mac s =
  sprintf "%02x:%02x:%02x:%02x:%02x:%02x"
    (Char.code s.[0]) (Char.code s.[1]) (Char.code s.[2])
    (Char.code s.[3]) (Char.code s.[4]) (Char.code s.[5])

let resolve_type t = match t with
  | 0x0806 -> "ARP"
  | 0x0800 -> "IPv4"
  | 0x86dd -> "IPv6"
  | _      -> "unknown"

let main () =
  lwt t = OS.Netif.create
    (fun id netif ->
       eprintf "netif open %s (%s, %s)\n%!" id (OS.Netif.ethid netif)
         (show_mac (OS.Netif.mac netif));
       lwt _ = OS.Time.sleep 1000000 in
       OS.Netif.listen netif
         (fun frame ->
            eprintf "incoming frame of size %d\n%!" (Cstruct.len frame);
            let tp = get_ethernet_ethertype frame in
            eprintf "ethertype: %04X (%s)\n%!" tp (resolve_type tp);
            return ()
         )
    )
  in
  eprintf "netif thread done\n%!";
  return ()
