#ifndef __FAKE_IO_H_INCLUDED__
#define __FAKE_IO_H_INCLUDED__

#include <sys/types.h>
#include <sys/stat.h>

int open(const char *path, int flags, ...);
int close(int d);
int stat(const char *path, struct stat *sb);
int fcntl(int fd, int cmd, ...);
int unlink(const char *rpath);
int rename(const char *from, const char *to);
char * getwd(char *buf);
int system(const char *string);
int chdir(const char *path);

#endif

