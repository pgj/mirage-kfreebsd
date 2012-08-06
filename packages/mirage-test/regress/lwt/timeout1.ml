open Lwt (* provides >>= and join *)
open OS  (* provides Time, Console and Main *)

let timeout f t =
  Time.sleep f >>
  match state t with
  | Return v -> return (Some v)
  | _        -> cancel t; return None

let main () =
  Random.self_init ();
  let t =
    Time.sleep (Random.int 3000000) >>= fun () ->
    return "Heads"
  in
  timeout 2000000 t >>= fun v ->
  match v with
  | Some s -> Console.log_s (Printf.sprintf "Some %s" s)
  | None   -> Console.log_s (Printf.sprintf "None")
