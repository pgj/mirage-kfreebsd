open Lwt
open OS.Clock
open OS.Console
open OS.Time
open OS.Main

let rec fib n =
  if n < 2 then 1 else fib (n - 1) + fib (n - 2)

let f () =
  log_s "Sorry, no in-kernel Mirage today!" >>
  log_s "And there is a second message..." >>
  log_s "Finally a third one." >>
  log_s "Go to bed for a second..." >>
  let t1 = time () in
  sleep 1000000 >>
  let t2 = time () in
  log_s "... and now wake up!" >>
  let passed = (t2 - t1) / 1000 in
  let msg = Printf.sprintf "Time passed: %d ms.\n" passed in
  log_s msg >>
  log_s "Let's do some Fibonacci!" >>
  let n = 42 in
  let msg = Printf.sprintf "fib %d = " n in
  log_s msg >>
  let msg = Printf.sprintf "%d" (fib n) in
  log_s msg >>
  return ()

let day_of n = match n with
  | 0 -> "Sun"
  | 1 -> "Mon"
  | 2 -> "Tue"
  | 3 -> "Wed"
  | 4 -> "Thu"
  | 5 -> "Fri"
  | 6 -> "Sat"
  | _ -> "???"

let _ =
  Printf.printf "This message comes somewhere from the body.\n%!";
  let tm = gmtime (time ()) in
  Printf.printf
    "Current date and time: %d.%02d.%02d. %s %02d:%02d:%02d. (%d)\n%!"
    (1900 + tm.tm_year) (tm.tm_mon + 1) tm.tm_mday (day_of tm.tm_wday)
    tm.tm_hour tm.tm_min tm.tm_sec (tm.tm_yday + 1);
  Printf.printf "Launch the lwt thread!\n%!";
  run (f ())
