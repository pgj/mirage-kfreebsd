
.if !defined(KAMLROOT)
.error KAMLROOT must be specified.
.endif

KAML_BIN?=	${KAMLROOT}/bin
KAML_LIB?=	${KAMLROOT}/lib/ocaml
KAML_P4LIB?=	${KAML_LIB}/camlp4
KAML_SITELIB?=	${KAML_LIB}/site-lib

OCAMLC=		ocamlc -nostdlib -I ${KAML_LIB} -I ${KAML_P4LIB} ${DEPLIBS}
OCAMLOPT=	ocamlopt -nostdlib -I ${KAML_LIB} -I ${KAML_P4LIB} -nodynlink -fno-PIC ${DEPLIBS}
OCAMLBUILD=	ocamlbuild -no-stdlib -no-plugin -ocamlc "${OCAMLC}" -ocamlopt "${OCAMLOPT}"

.if defined(LIBNAME)

COBJS?=		# nada
DESTDIR?=	${KAML_SITELIB}/${LIBNAME}
DEPS?=		# nada

DEPLIBS=	${DEPS:M*:S|^|-I ${KAML_SITELIB}/|}

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
.if ${COBJS} != ""
	cp ${COBJS} ${DESTDIR}
.endif

deinstall:
	rm -rf ${KAML_SITELIB}/${LIBNAME}

clean::
	rm -rf _build
.if ${COBJS} != ""
	rm -rf ${COBJS}
.endif

.endif # LIBNAME
