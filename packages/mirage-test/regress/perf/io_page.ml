open Printf
open Lwt

let with_time i label fn =
  let t1 = OS.Clock.time () in
  fn ();
  let t2 = OS.Clock.time () in
  let passed = (t2 - t1) / 1000 in
  printf "%s,%d,%d,%d ms\n%!" Sys.os_type i label passed

let main () =
  Random.self_init ();
  let sizes = [ 1; 2; 4; 8; 16; 32; 64; 128 ] in
  List.iter (fun sz ->
    with_time sz sz (fun () ->
      for i = 0 to 10000 do
        let p = OS.Io_page.get ~pages_per_block:sz () in
        let page_size = OS.Io_page.length p in
        for j  = 0 to (sz * 10) do
          let off = Random.int page_size in
          let len = Random.int (page_size - off) in
          let k   = Random.int sz in
          let _   = OS.Io_page.sub p off len in
          ()
        done;
      done;
    )
  ) sizes;
  return ()
