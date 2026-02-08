#include "../drivers/driver_vga.h"

// ==========================================
// HARDWARE CONSTANTS
// ==========================================
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

// Internal State
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;

// ==========================================
// INTERNAL HELPERS
// ==========================================

static inline uint8_t vga_entry_color(VgaColor fg, VgaColor bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

static void k_vga_put_entry_at(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    VGA_MEMORY[index] = vga_entry(c, color);
}

static void k_vga_scroll() {
    // Move lines up
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t src_idx = (y + 1) * VGA_WIDTH + x;
            const size_t dst_idx = y * VGA_WIDTH + x;
            VGA_MEMORY[dst_idx] = VGA_MEMORY[src_idx];
        }
    }
    // Clear last line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        VGA_MEMORY[index] = vga_entry(' ', terminal_color);
    }
}

// ==========================================
// PUBLIC API implementation
// ==========================================

#if 0
void k_vga_init() {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK);
    
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            VGA_MEMORY[index] = vga_entry(' ', terminal_color);
        }
    }
}

void k_print_color(const char* str, VgaColor fg, VgaColor bg) {
    terminal_color = vga_entry_color(fg, bg);
    k_print(str);
}

void k_print(const char* str) {
    for (size_t i = 0; i < k_strlen(str); i++) {
        char c = str[i];

        if (c == '\n') {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                k_vga_scroll();
                terminal_row = VGA_HEIGHT - 1;
            }
            continue;
        }

        k_vga_put_entry_at(c, terminal_color, terminal_column, terminal_row);
        
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                k_vga_scroll();
                terminal_row = VGA_HEIGHT - 1;
            }
        }
    }
}
#endif
