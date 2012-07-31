
let rec fib n =
  (*Printf.eprintf "[n = %d]" n;*)
  if n < 2 then 1 else fib (n - 1) + fib (n - 2)

let f () =
  Printf.eprintf "Sorry, no in-kernel Mirage today!\n";
  Printf.eprintf "And there is a second message...\n";
  Printf.eprintf "Finally a third one.\n";
  Printf.eprintf "Let's do some Fibonacci!\n";
  fib 32;
  Printf.eprintf "\n";
  true

let _ =
  Printf.eprintf "This message comes somewhere from the body.\n";
  Callback.register "OS.Main.run" f
