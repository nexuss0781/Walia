#ifndef WALIA_LIBC_SYS_MMAN_H
#define WALIA_LIBC_SYS_MMAN_H
#include <stddef.h>
#define PROT_READ  0x1
#define PROT_WRITE 0x2
#define PROT_EXEC  0x4
#define MAP_SHARED 0x01
#define MAP_FAILED ((void*)-1)
void* mmap(void* addr, size_t length, int prot, int flags, int fd, size_t offset);
int msync(void* addr, size_t length, int flags);
int mprotect(void *addr, size_t len, int prot);
#define MS_SYNC 0x01
#endif
