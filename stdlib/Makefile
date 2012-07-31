
.PHONY: all clean depend

PREFIX?=	/usr/local

all:
	gmake -f GNUMakefile PREFIX=${PREFIX} all
	gmake -f GNUMakefile PREFIX=${PREFIX} allopt

clean:
	gmake -f GNUMakefile clean

depend:
	gmake -f GNUMakefile depend

install: all
	gmake -f GNUMakefile install LIBDIR=${DESTDIR} PREFIX=${PREFIX}
	gmake -f GNUMakefile installopt LIBDIR=${DESTDIR} PREFIX=${PREFIX}
