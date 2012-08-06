.PHONY: clean

all:
	

basic/extern:
	ln -sf basic/extern.ml .
	echo "let _ = OS.Main.run (Extern.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=extern.ml KMOD=mirage-basic-extern

basic/console:
	ln -sf basic/console.ml .
	echo "let _ = OS.Main.run (Console.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=console.ml KMOD=mirage-basic-console

basic/exception:
	ln -sf basic/exception.ml .
	echo "let _ = OS.Main.run (Exception.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=exception.ml KMOD=mirage-basic-exception

basic/sleep:
	ln -sf basic/sleep.ml .
	echo "let _ = OS.Main.run (Sleep.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=sleep.ml KMOD=mirage-basic-sleep

perf/alloc:
	ln -sf perf/alloc.ml .
	echo "let _ = OS.Main.run (Alloc.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=alloc.ml KMOD=mirage-perf-alloc

perf/gcperf:
	ln -sf perf/gcperf.ml .
	echo "let _ = OS.Main.run (Gcperf.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=gcperf.ml KMOD=mirage-perf-gcperf

perf/sieve:
	ln -sf perf/sieve.ml .
	echo "let _ = OS.Main.run (Sieve.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=sieve.ml KMOD=mirage-perf-sieve

perf/thread_create:
	ln -sf perf/thread_create.ml .
	echo "let _ = OS.Main.run (Thread_create.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=thread_create.ml KMOD=mirage-perf-thread_create

perf/thread_lat:
	ln -sf perf/thread_lat.ml .
	echo "let _ = OS.Main.run (Thread_lat.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=thread_lat.ml KMOD=mirage-perf-thread_lat

clean:
	rm -f *.ml *.ko *.o *.cmx *.cmi
