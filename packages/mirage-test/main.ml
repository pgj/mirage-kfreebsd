
let rec fib n =
  (*prerr_endline "(n = %d)" n;*)
  if n < 2 then 1 else fib (n - 1) + fib (n - 2)

let f () =
  prerr_endline "Sorry, no in-kernel Mirage today!\n";
  prerr_endline "And there is a second message...\n";
  prerr_endline "Finally a third one.\n";
  prerr_endline "Let's do some Fibonacci!\n";
  fib 100;
  true

let _ =
  prerr_endline "This message comes somewhere from the body.\n";
  Callback.register "OS.Main.run" f
