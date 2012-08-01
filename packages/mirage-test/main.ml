
let rec fib n =
  Printf.printf "[n = %d]" n;
  if n < 2 then 1 else fib (n - 1) + fib (n - 2)

let f () =
  Printf.printf "Sorry, no in-kernel Mirage today!\n";
  Printf.printf "And there is a second message...\n";
  Printf.printf "Finally a third one.\n";
  Printf.printf "Let's do some Fibonacci!\n";
  fib 32;
  Printf.printf "\n";
  flush stdout;
  true

let _ =
  Printf.printf "This message comes somewhere from the body.\n";
  flush stdout;
  Callback.register "OS.Main.run" f
