#ifndef WALIA_LIBC_STRING_H
#define WALIA_LIBC_STRING_H
#include <stddef.h>
void* k_memset(void* ptr, int value, size_t num);
void* k_memcpy(void* dest, const void* src, size_t num);
size_t k_strlen(const char* str);
int k_strncmp(const char* s1, const char* s2, size_t n);
int k_memcmp(const void* s1, const void* s2, size_t n);
char* k_strcpy(char* dest, const char* src);
char* k_strchr(const char* s, int c);
char* k_strtok(char* str, const char* delim);
char* k_strdup(const char* s);
#define memset k_memset
#define memcpy k_memcpy
#define strlen k_strlen
#define strncmp k_strncmp
#define memcmp k_memcmp
#define strcpy k_strcpy
#define strchr k_strchr
#define strtok k_strtok
#define strdup k_strdup
#endif
