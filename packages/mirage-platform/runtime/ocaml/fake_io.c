
#include <sys/types.h>
#include <sys/stat.h>

#include "fake.h"
#include "fake_io.h"
#include "misc.h"

int
open(const char *path, int flags, ...)
{
  printf("XXX: CAML: open() is not implemented.\n");
  return -1;
}

int
close(int fd)
{
  printf("XXX: CAML: close() is not implemented.\n");
  return -1;
}

int
stat(const char *path, struct stat *sb)
{
  printf("XXX: CAML: stat() is not implemented.\n");
  return -1;
}

int
fcntl(int fd, int cmd, ...)
{
  printf("XXX: CAML: fcntl() is not implemented.\n");
  return -1;
}

int
unlink(const char *rpath)
{
  printf("XXX: CAML: unlink() is not implemented.\n");
  return -1;
}

int
rename(const char *from, const char *to)
{
  printf("XXX: CAML: rename() is not implemented.\n");
  return -1;
}

char *
getwd(char *buf)
{
  printf("XXX: CAML: getwd() is not implemented.\n");
  return NULL;
}

int
system(const char *string)
{
  printf("XXX: CAML: system() is not implemented.\n");
  return -1;
}

int
chdir(const char *path)
{
  printf("XXX: CAML: chdir() is not implemented.\n");
  return -1;
}
