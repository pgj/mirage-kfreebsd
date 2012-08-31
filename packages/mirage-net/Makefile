
LIBNAME=	mirage-net

_SRCS!=		ls lib/*.ml
_SRCS_DHCP!=	ls lib/dhcp/*.ml
_SRCS_TCP!=	ls lib/tcp/*.ml

SRCS=		${_SRCS} ${_SRCS_DHCP} ${_SRCS_TCP}
MLLIB=		lib/net.cma lib/net.cmxa lib/net.a

.include "${.CURDIR}/../bsd.mirage.mk"
