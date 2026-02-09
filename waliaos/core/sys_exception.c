#include "../include/sys_cpu.h"
#include "../include/driver_vga.h"

/**
 * @brief The Sovereign Scram.
 * Called when the hardware detects a violation or fault.
 */
void sys_exception_handler() {
    k_vga_set_color(VGA_WHITE, VGA_RED);
    k_vga_print("\n\n[!!!] SOVEREIGN SCRAM: EXCEPTION DETECTED [!!!]\n");
    k_vga_print("Hardware protection triggered a kernel halt.\n");
    k_vga_print("The system has been locked down to prevent corruption.\n");
    
    while(1) {
        __asm__ volatile ("hlt");
    }
}
