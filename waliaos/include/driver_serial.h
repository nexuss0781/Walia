#ifndef WALIA_DRIVER_SERIAL_H
#define WALIA_DRIVER_SERIAL_H

#include "walia_kernel_base.h"

#define COM1 0x3F8

void k_serial_init();
void k_serial_putc(char c);
void k_serial_print(const char* str);

#endif
