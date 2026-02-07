#ifndef WALIA_KERNEL_BASE_H
#define WALIA_KERNEL_BASE_H

#ifdef __STDC_HOSTED__
    #include <stdint.h>
    #include <stddef.h>
    #include <stdbool.h>
#else
    // Freestanding headers are available in GCC/Clang
    #include <stdint.h>
    #include <stddef.h>
    #include <stdbool.h>
#endif

// Metal Runtime Prototypes
void* k_memset(void* ptr, int value, size_t num);
void* k_memcpy(void* dest, const void* src, size_t num);
size_t k_strlen(const char* str);
char* k_itoa(int value, char* str, int base);

#endif // WALIA_KERNEL_BASE_H
