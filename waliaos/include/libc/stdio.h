#ifndef WALIA_LIBC_STDIO_H
#define WALIA_LIBC_STDIO_H
#include <stddef.h>
#include <stdarg.h>
void k_vga_print(const char* str);
void k_serial_print(const char* str);
#define printf(fmt, ...) k_serial_print(fmt)
#define fprintf(stream, fmt, ...) k_serial_print(fmt)
#define perror(msg) k_serial_print(msg)
#define stderr (void*)0
int sprintf(char* str, const char* format, ...);
int vfprintf(void* stream, const char* format, va_list args);
int fputs(const char* s, void* stream);
#endif
