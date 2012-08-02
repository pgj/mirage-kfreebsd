
basic/extern:
	ln -sf basic/extern.ml extern.ml
	echo "let _ = OS.Main.run (Extern.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=extern.ml KMOD=mirage-regress-extern

basic/console:
	ln -sf basic/console.ml console.ml
	echo "let _ = OS.Main.run (Console.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=console.ml KMOD=mirage-regress-console

basic/exception:
	ln -sf basic/exception.ml exception.ml
	echo "let _ = OS.Main.run (Exception.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=exception.ml KMOD=mirage-regress-exception

perf/alloc:
	ln -sf perf/alloc.ml alloc.ml
	echo "let _ = OS.Main.run (Alloc.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=alloc.ml KMOD=mirage-perf-alloc

perf/gcperf:
	ln -sf perf/gcperf.ml gcperf.ml
	echo "let _ = OS.Main.run (Gcperf.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=gcperf.ml KMOD=mirage-perf-gcperf

perf/sieve:
	ln -sf perf/sieve.ml sieve.ml
	echo "let _ = OS.Main.run (Sieve.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=sieve.ml KMOD=mirage-perf-sieve

.PHONY: clean

clean:
	rm -f *.ml *.ko *.o *.cmx *.cmi


