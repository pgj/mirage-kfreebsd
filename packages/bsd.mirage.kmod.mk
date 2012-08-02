
KMOD?= mirage

# caml begins here

CAMLP4=		camlp4of -I +camlp4 ${LWTSYNTAX}
CAMLCC=		${OCAMLOPT} -nostdlib -I ${STDLIBDIR} -pp '${CAMLP4}'

MIRDIR!=	${OCAMLFIND} query mirage-platform
MIROBJS!=	ls ${MIRDIR}/*.cmxa
MIRAS!=		ls ${MIRDIR}/*.a

KERNOBJS!=	ls ${MIRDIR}/*.o

LWTDIR!=	${OCAMLFIND} query lwt
LWTOBJS!=	ls ${LWTDIR}/*.cmxa
LWTAS!=		ls ${LWTDIR}/*.a
LWTSYNTAX=	-I ${LWTDIR} lwt-syntax-options.cma lwt-syntax.cma

CSTDIR!=	${OCAMLFIND} query cstruct
CSTOBJS!=	ls ${CSTDIR}/*.cmxa
CSTAS!=		ls ${CSTDIR}/*.a

STLIBS=		${MIRAS} ${CSTAS} ${LWTAS}

MLOBJS=		${CSTOBJS} ${LWTOBJS} ${MIROBJS}
MLINCS=		-I ${MIRDIR} -I ${LWTDIR} -I ${CSTDIR}

.SUFFIXES: .ml .cmx .o

CMXS:=		${SRCS:M*.ml:S/.ml$/.cmx/}
CMIS:=		${SRCS:M*.ml:S/.ml$/.cmi/}

.ml.cmx:
	${CAMLCC} ${MLINCS} -c ${.IMPSRC} -o ${.TARGET}

OBJS+=	main.o

main.o: main.ml ${CMXS}
	${CAMLCC} ${MLINCS} ${MLOBJS} ${CMXS} -output-obj -o _${.TARGET} main.ml
	mv -f _${.TARGET} ${.TARGET}

CLEANFILES+=	main.cmi main.cmx ${CMXS} ${CMIS} ${SRCS:M*.ml:S/.ml$/.o/}

${KMOD}.ko: ${OBJS} ${STLIBS}
	${LD} ${LDFLAGS} -r -d -o ${.TARGET} main.o ${KERNOBJS} ${STLIBS}

.include <bsd.kmod.mk>
