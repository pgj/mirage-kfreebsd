
LIBNAME=	mirage-stdlib

SRCS!=		ls src/*.ml
MLLIB=		src/stdlib.cma src/stdlib.cmxa src/stdlib.a
BUILDDIR=	.

all:
	${MAKE} -C src all

clean:
	${MAKE} -C src clean

.include "${.CURDIR}/../bsd.mirage.mk"
