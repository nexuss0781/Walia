#include "../include/sys_shell.h"
#include "../include/driver_vga.h"
#include "../../src/tooling/tooling_nexus_bus.h"
#include "../include/walia_kernel_base.h"

// INTERNAL BUFFER (The Edit Segment)
static char    input_buffer[1024];
static int     buffer_idx = 0;
static bool    line_complete = false;

// ==========================================
// SOVEREIGN LINE EDITOR
// ==========================================

/**
 * @brief Logic for handling a single key pulse.
 * 
 * UFO SPEED: Performs character rendering and buffer management 
 * in a single atomic pass.
 */
static void process_key_pulse(WldPulse* p) {
    char c = (char)p->targetId;
    bool shifted = (bool)p->payload;

    // 1. Handle Special Keys
    if (c == '\n') {
        input_buffer[buffer_idx] = '\0';
        line_complete = true;
        k_vga_putc('\n');
        return;
    }

    if (c == '\b') { // Backspace
        if (buffer_idx > 0) {
            buffer_idx--;
            // Visual Backspace: Move cursor back, print space, move back again
            /* k_vga_backspace(); */
            k_vga_print("\b \b"); 
        }
        return;
    }

    // 2. Standard Character Ingress
    if (buffer_idx < 1023) {
        // Apply shift if active
        if (shifted && c >= 'a' && c <= 'z') c -= 32;
        
        input_buffer[buffer_idx++] = c;
        k_vga_putc(c); // Immediate hardware echo
    }
}

/**
 * @brief High-level bridge for the Walia Shell.
 * Reaps the fabric mailbox and populates the command string.
 */
const char* sys_shell_read_line(const char* prompt) {
    buffer_idx = 0;
    line_complete = false;
    k_memset(input_buffer, 0, 1024);

    k_vga_set_color(VGA_CYAN, VGA_BLACK);
    k_vga_print(prompt);
    k_vga_set_color(VGA_WHITE, VGA_BLACK);

    while (!line_complete) {
        // 1. Check for hardware input
        sys_keyboard_poll();

        // 2. Reap pulses from the Fabric
        WldPulse pulses[16];
        int count = walia_pulse_collect(pulses, 16);

        for (int i = 0; i < count; i++) {
            if (pulses[i].type == PULSE_SCAN_FILE) {
                process_key_pulse(&pulses[i]);
            }
        }
        
        // 3. Power Sovereignty: Yield if no input
        if (!line_complete) {
            __asm__ volatile ("pause"); // Hint to CPU that we are in a spin-wait
        }
    }

    return input_buffer;
}