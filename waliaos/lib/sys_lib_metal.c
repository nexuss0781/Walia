#include "walia_kernel_base.h"

/*
 * SOVEREIGN MEMORY MOVER (UFO-Grade)
 * Logic: Move data in 64-bit chunks to minimize CPU clock cycles.
 */
void* k_memcpy(void* dest, const void* src, size_t num) {
    uint64_t* d64 = (uint64_t*)dest;
    const uint64_t* s64 = (const uint64_t*)src;

    // 1. Unrolled Copy: Move in 8-byte increments
    while (num >= 8) {
        *d64++ = *s64++;
        num -= 8;
    }

    // 2. Tail Copy: Handle remaining bytes
    uint8_t* d8 = (uint8_t*)d64;
    const uint8_t* s8 = (const uint8_t*)s64;
    while (num--) {
        *d8++ = *s8++;
    }

    return dest;
}

/*
 * SOVEREIGN MEMORY INITIALIZER
 */
void* k_memset(void* ptr, int value, size_t num) {
    uint8_t* p = (uint8_t*)ptr;
    uint8_t v = (uint8_t)value;

    // Fast-path: If setting to zero, we can optimize via 64-bit words
    if (v == 0 && num >= 8) {
        uint64_t* p64 = (uint64_t*)ptr;
        while (num >= 8) {
            *p64++ = 0;
            num -= 8;
        }
        p = (uint8_t*)p64;
    }

    while (num--) {
        *p++ = v;
    }
    return ptr;
}

/*
 * SOVEREIGN STRING ANALYSIS
 */
size_t k_strlen(const char* str) {
    const char* start = str;
    while (*str) str++;
    return (size_t)(str - start);
}

int k_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}
