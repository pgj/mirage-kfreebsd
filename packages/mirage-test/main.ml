
let rec fib n =
  (*prerr_endline "(n = %d)" n;*)
  if n < 2 then 1 else fib (n - 1) + fib (n - 2)

let f () =
  prerr_endline "Sorry, no in-kernel Mirage today!";
  prerr_endline "And there is a second message...";
  prerr_endline "Finally a third one.";
  prerr_endline "Let's do some Fibonacci!";
  fib 32;
  true

let _ =
  prerr_endline "This message comes somewhere from the body.";
  Callback.register "OS.Main.run" f
