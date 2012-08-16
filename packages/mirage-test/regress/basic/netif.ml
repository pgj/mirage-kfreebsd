open Lwt
open Printf

let show_mac s =
  sprintf "%02x:%02x:%02x:%02x:%02x:%02x"
    (Char.code s.[0]) (Char.code s.[1]) (Char.code s.[2])
    (Char.code s.[3]) (Char.code s.[4]) (Char.code s.[5])

let main () =
  lwt t = OS.Netif.create
    (fun id netif ->
       eprintf "netif open %s (%s, %s)\n%!" id (OS.Netif.ethid netif)
         (show_mac (OS.Netif.mac netif));
       lwt _ = OS.Time.sleep 1000000 in
       OS.Netif.listen netif
         (fun page ->
            eprintf "incoming %d\n%!" (Cstruct.len page);
            return ()
         )
    )
  in
  eprintf "netif thread done\n%!";
  return ()
