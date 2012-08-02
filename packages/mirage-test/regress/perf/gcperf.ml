open Lwt
open Printf

let with_time i label fn =
  let t1 = OS.Clock.time () in
  fn ();
  let t2 = OS.Clock.time () in
  let passed = (t2 - t1) / 1000 in
  printf "%s,%d,%d,%d ms\n%!" Sys.os_type i label passed

let main () =
  (*let _ = Gc.set { (Gc.get ()) with Gc.verbose = 0x3 } in*)
  let _ = Gc.create_alarm (fun () -> Printf.printf "gc\n%!") in
  let sizes = [ 100; 500; 1000; 5000; 10000 ] in
  List.iter (fun sz ->
    with_time sz sz (fun () ->
      for i = 0 to 1000000 do
        let p = String.create sz in
        (ignore p)
      done
    )
  ) sizes;
  return ()
