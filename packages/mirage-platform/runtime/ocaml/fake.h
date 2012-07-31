#ifndef __FAKE_H_INCLUDED__
#define __FAKE_H_INCLUDED__

#define DUMMY(R,F,X,V)					\
  R							\
  F(X)							\
  {							\
    printf("XXX: CAML: "#F"() is not implemented.\n");	\
    return V;						\
  }

#define DUMMY2(R,F,X,Y,V)				\
  R							\
  F(X,Y)						\
  {							\
    printf("XXX: CAML: "#F"() is not implemented.\n");	\
    return V;						\
  }

#define NOT_IMPLEMENTED(F)				\
  printf("XXX: CAML: "#F"() is not implemented.\n");	\
  return 0

#define NOT_IMPLEMENTED_VOID(F)				\
  printf("XXX: CAML: "#F"() is not implemented.\n")

#endif
