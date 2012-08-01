
basic/extern:
	ln -sf basic/extern.ml extern.ml
	echo "let _ = OS.Main.run (Extern.main ())" > main.ml
	${MAKE} -f Makefile.kmod STDLIBDIR=${STDLIBDIR} SRCS=extern.ml KMOD=mirage-regress-extern

basic/console:
	ln -sf basic/console.ml console.ml
	echo "let _ = OS.Main.run (Console.main ())" > main.ml
	${MAKE} -f Makefile.kmod STDLIBDIR=${STDLIBDIR} SRCS=console.ml KMOD=mirage-regress-console

.PHONY: clean

clean:
	rm -f *.ml *.ko *.o *.cmx *.cmi


