#ifndef WALIA_LIBC_UNISTD_H
#define WALIA_LIBC_UNISTD_H
#include <stddef.h>
int ftruncate(int fd, size_t length);
int close(int fd);
long sysconf(int name);
#define _SC_PAGESIZE 1
#endif
