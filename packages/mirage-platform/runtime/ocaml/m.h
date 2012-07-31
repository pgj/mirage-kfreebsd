#if defined(__amd64__)

#define ARCH_SIXTYFOUR
#define SIZEOF_INT 4
#define SIZEOF_LONG 8
#define SIZEOF_PTR 8
#define SIZEOF_SHORT 2
#define ARCH_INT64_TYPE long
#define ARCH_UINT64_TYPE unsigned long
#define ARCH_INT64_PRINTF_FORMAT "l"
#undef ARCH_BIG_ENDIAN
#undef ARCH_ALIGN_DOUBLE
#undef ARCH_ALIGN_INT64
#undef NONSTANDARD_DIV_MOD

#elif defined(__i386__)

#undef ARCH_SIXTYFOUR
#define SIZEOF_INT 4
#define SIZEOF_LONG 4
#define SIZEOF_PTR 4
#define SIZEOF SHORT 2
#define ARCH_INT64_TYPE long long
#define ARCH_UINT64_TYPE unsigned long long
#define ARCH_INT64_PRINTF_FORMAT "ll"
#undef ARCH_BIG_ENDIAN
#undef ARCH_ALIGN_DOUBLE
#undef ARCH_ALIGN_INT64
#undef NONSTANDARD_DIV_MOD

#else
#endif
