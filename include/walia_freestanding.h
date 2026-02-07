#ifndef WALIA_FREESTANDING_H
#define WALIA_FREESTANDING_H

// ==========================================
// SOVEREIGN TYPE DEFINITIONS
// ==========================================
// Detect if we are in a HOSTED environment (Standard C Lib Available)
// or a BARE-METAL environment (Substrate Implementation).

#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ == 1
    #include <stdint.h>
    #include <stddef.h>
    #include <stdbool.h>
    #ifndef WALIA_TYPES_DEFINED
    #define WALIA_TYPES_DEFINED
    #endif
#else
    #ifndef WALIA_TYPES_DEFINED
    #define WALIA_TYPES_DEFINED

    #if defined(__x86_64__) || defined(__aarch64__)
        typedef unsigned long long uint64_t;
        typedef long long          int64_t;
        typedef unsigned long      size_t;
        typedef long               intptr_t;
        typedef unsigned long      uintptr_t;
    #else
        typedef unsigned int       size_t;
        typedef int                intptr_t;
    #endif

    typedef unsigned int   uint32_t;
    typedef int            int32_t;
    typedef unsigned short uint16_t;
    typedef short          int16_t;
    typedef unsigned char  uint8_t;
    typedef signed char    int8_t;

    #define NULL ((void*)0)
    #define true  1
    #define false 0
    typedef _Bool bool;
    #endif
#endif

// ==========================================
// KERNEL RUNTIME API
// ==========================================

void* k_memset(void* ptr, int value, size_t num);
void* k_memcpy(void* dest, const void* src, size_t num);
size_t k_strlen(const char* str);
char* k_itoa(int value, char* str, int base);

#endif // WALIA_FREESTANDING_H