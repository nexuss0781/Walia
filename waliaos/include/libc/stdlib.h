#ifndef WALIA_LIBC_STDLIB_H
#define WALIA_LIBC_STDLIB_H
#include <stddef.h>
void* reallocate(void* pointer, size_t oldSize, size_t newSize);
int k_atoi(const char* str);
#define malloc(size) reallocate(NULL, 0, size)
#define calloc(num, size) reallocate(NULL, 0, (num) * (size))
#define free(ptr) reallocate(ptr, 0, 0)
#define exit(code) while(1) { __asm__ volatile("hlt"); }
#define atoi k_atoi
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
void* realloc(void* ptr, size_t size);
double strtod(const char* str, char** endptr);
#endif
