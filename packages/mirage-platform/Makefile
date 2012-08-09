
LIBNAME=	mirage-platform

SRCS!=		ls lib/*.ml
MLLIB=		lib/oS.cmxa lib/oS.a lib/oS.cmx lib/oS.cmi
COBJS=		runtime/kernel/netif_stubs.o runtime/kernel/page_stubs.o \
		runtime/kernel/clock_stubs.o runtime/kernel/kmod.o \
		runtime/ocaml/libocaml.a

PWD!=		pwd

CFLAGS=		-O2 -DCAML_NAME_SPACE -DNATIVE_CODE -DSYS_bsd_elf \
		-DTARGET_amd64 -Werror -D_KERNEL -DKLD_MODULE -nostdinc \
		-I${PWD}/runtime/include -I${PWD}/runtime/kernel/@ \
		-I${PWD}/runtime/kernel/@/contrib/altq -I${PWD}/runtime/kernel \
		-finline-limit=8000 --param inline-unit-growth=100 \
		--param large-function-growth=1000 -fno-common \
		-fno-omit-frame-pointer -mcmodel=kernel -mno-red-zone -mno-mmx \
		-msoft-float  -fno-asynchronous-unwind-tables -ffreestanding \
		-fstack-protector -std=iso9899:1999 -fstack-protector -Wall \
		-Wredundant-decls -Wnested-externs -Wstrict-prototypes \
		-Wmissing-prototypes -Wpointer-arith -Winline -Wcast-qual \
		-Wundef -Wno-pointer-sign -fformat-extensions \
		-Wmissing-include-dirs -fdiagnostics-show-option -mno-sse \
		-DKDTRACE_HOOKS

DEPS=		lwt

LIBOCAML_SRCS!=		ls runtime/ocaml/*.c
LIBOCAML_OBJS=		runtime/ocaml/amd64.o ${LIBOCAML_SRCS:M*.c:S/.c$/.o/}

K=		runtime/kernel
S=		/usr/src/sys
_ILINKS=	@ machine x86

.include "${.CURDIR}/../bsd.mirage.mk"

runtime/ocaml/libocaml.a: ${LIBOCAML_OBJS}
	${AR} rc ${.TARGET} ${LIBOCAML_OBJS}

${_ILINKS}:
	@case ${.TARGET} in \
	@) \
		path=${S} ;; \
	machine) \
		path=${S}/${MACHINE}/include ;; \
	*) \
		path=${S}/${.TARGET}/include ;; \
	esac ; \
	${ECHO} ${.TARGET} "->" $$path ; \
	ln -s $$path ${K}/${.TARGET}

.if !exists(${K}/@)
runtime/kernel/kmod.o: ${_ILINKS}
.endif

clean::
	rm -rf ${LIBOCAML_OBJS}
