open OS.Clock

let rec fib n =
  Printf.printf "[n = %d]%!" n;
  if n < 2 then 1 else fib (n - 1) + fib (n - 2)

let f () =
  Printf.printf "Sorry, no in-kernel Mirage today!\n%!";
  Printf.printf "And there is a second message...\n%!";
  Printf.printf "Finally a third one.\n%!";
  Printf.printf "Let's do some Fibonacci!\n%!";
  ignore (fib 10);
  Printf.printf "\n%!";
  true

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
  let tm = OS.Clock.gmtime (OS.Clock.time ()) in
  Printf.printf
    "Current date and time: %d.%02d.%02d. %s %02d:%02d:%02d. (%d)\n%!"
    (1900 + tm.tm_year) (tm.tm_mon + 1) tm.tm_mday (day_of tm.tm_wday)
    tm.tm_hour tm.tm_min tm.tm_sec (tm.tm_yday + 1);
  Callback.register "OS.Main.run" f
