#ifndef WALIA_DRIVER_VGA_H
#define WALIA_DRIVER_VGA_H

#include "walia_freestanding.h"

// ==========================================
// VGA COLOR CODES
// ==========================================
typedef enum {
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_MAGENTA = 5,
    VGA_BROWN = 6,
    VGA_LIGHT_GREY = 7,
    VGA_DARK_GREY = 8,
    VGA_LIGHT_BLUE = 9,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11,
    VGA_LIGHT_RED = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_LIGHT_BROWN = 14,
    VGA_WHITE = 15,
} VgaColor;

// ==========================================
// VGA API
// ==========================================

/**
 * @brief Initializes the VGA buffer (Clear screen).
 */
void k_vga_init();

/**
 * @brief Prints a string to the screen with automatic scrolling.
 */
void k_print(const char* str);

/**
 * @brief Prints a string with a specific color.
 */
void k_print_color(const char* str, VgaColor fg, VgaColor bg);

#endif // WALIA_DRIVER_VGA_H
