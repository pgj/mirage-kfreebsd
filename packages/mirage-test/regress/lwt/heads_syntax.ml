open Lwt
open OS

let main () =
  let heads =
    Time.sleep 1000000 >>
    return (Console.log "Heads");
  in
  let tails =
    Time.sleep 2000000 >>
    return (Console.log "Tails");
  in
  lwt () = heads <&> tails in
  Console.log "Finished";
  return ()

