
.if !defined(STDLIBDIR)
.error STDLIBDIR must be specified.
.endif

CAMLROOT?=	/usr/local
CAML_BIN?=	${CAMLROOT}/bin
CAML_LIB?=	${CAMLROOT}/lib/ocaml
CAML_P4LIB?=	${CAML_LIB}/camlp4
CAML_SITELIB?=	${CAML_LIB}/site-lib

OCAMLC=		ocamlc.opt ${DEPLIBS}
OCAMLOPT=	ocamlopt.opt -nodynlink -fno-PIC ${DEPLIBS}
OCAMLBUILD=	ocamlbuild -ocamlc "${OCAMLC}" -ocamlopt "${OCAMLOPT}"

.if defined(LIBNAME)

DESTDIR?=	${CAML_SITELIB}/${LIBNAME}

.if !empty(DEPS)
DEPLIBS=	${DEPS:M*:S|^|-I ${CAML_SITELIB}/|}
.endif

CMIS=		${SRCS:M*.ml:S/.ml$/.cmi/}
CMXS=		${SRCS:M*.ml:S/.ml$/.cmx/}

_MLOBJS=	${CMIS} ${CMXS} ${MLLIB}
MLOBJS=		${_MLOBJS:M*:S|^|_build/|}

.SUFFIXES:	.o

.S.o:
	${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}

.c.o:
	${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}

.PHONY: all clean deinstall

all: ${MLOBJS} ${COBJS}

${MLOBJS}:
	${OCAMLBUILD} ${CMIS} ${CMXS} ${MLLIB}

install: all
	mkdir -p ${DESTDIR}
	cp ${MLOBJS} ${DESTDIR}
.if !empty(${COBJS})
	cp ${COBJS} ${DESTDIR}
.endif

deinstall:
	rm -rf ${CAML_SITELIB}/${LIBNAME}

clean::
	rm -rf _build
.if !empty(${COBJS})
	rm -rf ${COBJS}
.endif

.endif # LIBNAME
