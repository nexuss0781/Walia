#ifndef WALIA_DRIVER_VGA_H
#define WALIA_DRIVER_VGA_H

#include "walia_kernel_base.h"

#define VGA_BLACK   0
#define VGA_BLUE    1
#define VGA_GREEN   2
#define VGA_CYAN    3
#define VGA_RED     4
#define VGA_MAGENTA 5
#define VGA_BROWN   6
#define VGA_LIGHT_GREY 7
#define VGA_WHITE   15

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEM_ADDR 0xB8000

void k_vga_init();
void k_vga_set_color(uint8_t fg, uint8_t bg);
void k_vga_putc(char c);
void k_vga_print(const char* str);
void k_vga_clear();

#endif
