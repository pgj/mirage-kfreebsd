
OCAMLFIND?=	ocamlfind

.if empty(LIBNAME) || ${LIBNAME} != "mirage-stdlib"
STDLIBDIR!=	${OCAMLFIND} query mirage-stdlib
.endif

OCAMLC=		ocamlc.opt ${OCAMLCFLAGS} ${DEPLIBS}
OCAMLOPT=	ocamlopt.opt -nodynlink -fno-PIC ${OCAMLOPTFLAGS} ${DEPLIBS}
OCAMLBUILD=	ocamlbuild -ocamlc "${OCAMLC}" -ocamlopt "${OCAMLOPT}"

.if !empty(DEPS)
_DEPLIBS!=	${OCAMLFIND} query ${DEPS}
DEPLIBS=	${_DEPLIBS:M*:S|^|-I |}
.endif

.if defined(LIBNAME)

CMIS=		${SRCS:M*.ml:S/.ml$/.cmi/}
CMXS=		${SRCS:M*.ml:S/.ml$/.cmx/}

BUILDDIR?=	_build
_MLOBJS=	${CMIS} ${CMXS} ${MLLIB}
MLOBJS=		${_MLOBJS:M*:S|^|${BUILDDIR}/|}

.SUFFIXES:	.o

.S.o:
	${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}

.c.o:
	${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}

.PHONY: all clean deinstall

.if !target(all)
all: ${MLOBJS} ${COBJS}
.endif

${MLOBJS}:
	${OCAMLBUILD} ${CMIS} ${CMXS} ${MLLIB}

install: all
	${OCAMLFIND} install ${LIBNAME} META ${MLOBJS} ${COBJS}

deinstall:
	${OCAMLFIND} remove ${LIBNAME}

reinstall: deinstall install

.if !target(clean)
clean::
	rm -rf _build
.if !empty(COBJS)
	rm -rf ${COBJS}
.endif
.endif

.endif # LIBNAME
