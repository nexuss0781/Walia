#ifndef WALIA_KERNEL_BASE_H
#define WALIA_KERNEL_BASE_H

typedef unsigned long long uint64_t;
typedef long long          int64_t;
typedef unsigned int       uint32_t;
typedef int                int32_t;
typedef unsigned short     uint16_t;
typedef short              int16_t;
typedef unsigned char      uint8_t;
typedef signed char        int8_t;
typedef unsigned long      size_t;
typedef unsigned long      uintptr_t;

#define NULL ((void*)0)
#define true  1
#define false 0
typedef _Bool bool;

// Metal Runtime Prototypes
void* k_memset(void* ptr, int value, size_t num);
void* k_memcpy(void* dest, const void* src, size_t num);
size_t k_strlen(const char* str);
char* k_itoa(int value, char* str, int base);

#endif
