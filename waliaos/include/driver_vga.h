#ifndef WALIA_DRIVER_VGA_H
#define WALIA_DRIVER_VGA_H

#include "walia_kernel_base.h"

// 1. SOVEREIGN COLOR PALETTE
typedef enum {
    VGA_BLACK = 0, VGA_BLUE = 1, VGA_GREEN = 2, VGA_CYAN = 3,
    VGA_RED = 4, VGA_MAGENTA = 5, VGA_BROWN = 6, VGA_LIGHT_GREY = 7,
    VGA_DARK_GREY = 8, VGA_LIGHT_BLUE = 9, VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11, VGA_LIGHT_RED = 12, VGA_LIGHT_MAGENTA = 13,
    VGA_LIGHT_BROWN = 14, VGA_YELLOW = 14, VGA_WHITE = 15,
} WldVgaColor;

// 2. HARDWARE CONTEXT
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_BUFFER_ADDR 0xB8000

// 3. SOVEREIGN VISUAL API
void k_vga_init();
void k_vga_putc(char c);
void k_vga_print(const char* str);
void k_vga_set_color(WldVgaColor fg, WldVgaColor bg);
void k_vga_clear();

#endif // WALIA_DRIVER_VGA_H
