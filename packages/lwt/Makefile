
LIBNAME=	lwt

SRCS!=		ls src/core/*.ml syntax/*.ml
MLLIB=		src/core/lwt.cmxa src/core/lwt.cma src/core/lwt.a \
		syntax/lwt-syntax-options.a syntax/lwt-syntax-options.cma \
		syntax/lwt-syntax-options.cmxa \
		syntax/lwt-syntax.a syntax/lwt-syntax.cma syntax/lwt-syntax.cmxa \
		syntax/lwt-syntax-log.a syntax/lwt-syntax-log.cma \
		syntax/lwt-syntax-log.cmxa \
		syntax/optcomp.a syntax/optcomp.cma syntax/optcomp.cmxa

.include "../bsd.mirage.mk"
