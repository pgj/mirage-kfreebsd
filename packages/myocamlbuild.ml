open Ocamlbuild_plugin;;
open Command;;

let stdlib_dir =
  try Sys.getenv "STDLIBDIR" with
    | Not_found ->
      failwith "STDLIBDIR variable must be set."
  ;;

dispatch begin function
| After_rules ->
    flag ["camlp4of"; "build_syntax"]
	(S[A "-I"; A "+camlp4"]);
    flag ["compile"; "use_custom_stdlib"]
	(S[A "-nostdlib"; A "-I"; A stdlib_dir]);
| _ -> ()
end;;
