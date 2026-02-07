#include "walia_freestanding.h"

// ==========================================
// MEMORY PRIMITIVES
// ==========================================

void* k_memset(void* ptr, int value, size_t num) {
    unsigned char* p = (unsigned char*)ptr;
    while (num--) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}

void* k_memcpy(void* dest, const void* src, size_t num) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (num--) {
        *d++ = *s++;
    }
    return dest;
}

// ==========================================
// STRING PRIMITIVES
// ==========================================

size_t k_strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

void k_reverse(char* str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

char* k_itoa(int value, char* str, int base) {
    int i = 0;
    bool isNegative = false;

    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    if (value < 0 && base == 10) {
        isNegative = true;
        value = -value;
    }

    while (value != 0) {
        int rem = value % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value = value / base;
    }

    if (isNegative) str[i++] = '-';

    str[i] = '\0';
    k_reverse(str, i);

    return str;
}
