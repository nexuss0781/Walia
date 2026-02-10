#include "driver_vga.h"

static uint16_t* const VGA_MEM = (uint16_t*)VGA_BUFFER_ADDR;
static uint8_t  current_color = 0x07;
static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;

void k_vga_init() {
    k_vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    k_vga_clear();
}

void k_vga_set_color(WldVgaColor fg, WldVgaColor bg) {
    current_color = (uint8_t)fg | ((uint8_t)bg << 4);
}

void k_vga_clear() {
    for (uint16_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEM[i] = (uint16_t)' ' | (uint16_t)current_color << 8;
    }
    cursor_x = 0;
    cursor_y = 0;
}

static void scroll() {
    for (uint16_t y = 1; y < VGA_HEIGHT; y++) {
        k_memcpy(&VGA_MEM[(y - 1) * VGA_WIDTH], &VGA_MEM[y * VGA_WIDTH], VGA_WIDTH * 2);
    }
    for (uint16_t x = 0; x < VGA_WIDTH; x++) {
        VGA_MEM[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (uint16_t)' ' | (uint16_t)current_color << 8;
    }
}

void k_vga_putc(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        VGA_MEM[cursor_y * VGA_WIDTH + cursor_x] = (uint16_t)c | (uint16_t)current_color << 8;
        cursor_x++;
    }

    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= VGA_HEIGHT) {
        scroll();
        cursor_y = VGA_HEIGHT - 1;
    }
}

void k_vga_print(const char* str) {
    while (*str) k_vga_putc(*str++);
}

void k_vga_backspace() {
    if (cursor_x > 0) {
        cursor_x--;
    } else if (cursor_y > 0) {
        cursor_y--;
        cursor_x = VGA_WIDTH - 1;
    }
    VGA_MEM[cursor_y * VGA_WIDTH + cursor_x] = (uint16_t)' ' | (uint16_t)current_color << 8;
}
