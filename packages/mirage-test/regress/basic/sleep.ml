open Lwt
open OS

let iter (nm,sz,m) =
  Console.log ("start: " ^ nm);
  let tot = ref 0 in
  for_lwt i = 1 to sz do
    Console.log (Printf.sprintf "%s %d" nm i);
    let t = i / m in
    tot := t + !tot;
    Time.sleep t;
  done >>
    (Console.log ("done: " ^ nm); return ())

let main () =
  let t1 = "one", 5, 3100000 in
  let t2 = "two", 3, 1900000 in
  let t3 = "three", 4, 1800000 in
  let t4 = "four", 5, 3200000 in
  let r t = iter t >> return () in
  join [r t1; r t2; r t3; r t4]
