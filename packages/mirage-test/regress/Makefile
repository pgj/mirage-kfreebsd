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

basic/netif:
	ln -sf basic/netif.ml .
	echo "let _ = OS.Main.run (Netif.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=netif.ml KMOD=mirage-basic-netif

basic/sleep:
	ln -sf basic/sleep.ml .
	echo "let _ = OS.Main.run (Sleep.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=sleep.ml KMOD=mirage-basic-sleep

lwt/echoserver1:
	ln -sf lwt/echoserver1.ml .
	echo "let _ = OS.Main.run (Echoserver1.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=echoserver1.ml KMOD=mirage-lwt-echoserver1

lwt/echoserver2:
	ln -sf lwt/echoserver2.ml .
	echo "let _ = OS.Main.run (Echoserver2.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=echoserver2.ml KMOD=mirage-lwt-echoserver2

lwt/heads1:
	ln -sf lwt/heads1.ml .
	echo "let _ = OS.Main.run (Heads1.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=heads1.ml KMOD=mirage-lwt-heads1

lwt/heads2:
	ln -sf lwt/heads2.ml .
	echo "let _ = OS.Main.run (Heads2.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=heads2.ml KMOD=mirage-lwt-heads2

lwt/heads_syntax:
	ln -sf lwt/heads_syntax.ml .
	echo "let _ = OS.Main.run (Heads_syntax.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=heads_syntax.ml KMOD=mirage-lwt-heads_syntax

lwt/intserver:
	ln -sf lwt/intserver.ml .
	echo "let _ = OS.Main.run (Intserver.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=intserver.ml KMOD=mirage-lwt-intserver

lwt/stream_processing:
	ln -sf lwt/stream_processing.ml .
	echo "let _ = OS.Main.run (Stream_processing.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=stream_processing.ml KMOD=mirage-lwt-stream_processing

lwt/timeout1:
	ln -sf lwt/timeout1.ml .
	echo "let _ = OS.Main.run (Timeout1.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=timeout1.ml KMOD=mirage-lwt-timeout1

lwt/timeout2:
	ln -sf lwt/timeout2.ml .
	echo "let _ = OS.Main.run (Timeout2.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=timeout2.ml KMOD=mirage-lwt-timeout2

perf/alloc:
	ln -sf perf/alloc.ml .
	echo "let _ = OS.Main.run (Alloc.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=alloc.ml KMOD=mirage-perf-alloc

perf/gcperf:
	ln -sf perf/gcperf.ml .
	echo "let _ = OS.Main.run (Gcperf.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=gcperf.ml KMOD=mirage-perf-gcperf

perf/io_page:
	ln -sf perf/io_page.ml .
	echo "let _ = OS.Main.run (Io_page.main ())" > main.ml
	${MAKE} -f Makefile.kmod SRCS=io_page.ml KMOD=mirage-perf-io_page

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
