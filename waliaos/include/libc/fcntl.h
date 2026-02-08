#ifndef WALIA_LIBC_FCNTL_H
#define WALIA_LIBC_FCNTL_H
#define O_RDWR  0x2
#define O_CREAT 0x40
int open(const char* pathname, int flags, ...);
#endif
