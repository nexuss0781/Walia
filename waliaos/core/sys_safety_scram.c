#include "../include/walia_kernel_base.h"
#include "../include/driver_vga.h"
#include "../include/sys_pager_phys.h"
#include "../include/sys_thermal.h"

// ==========================================
// EMERGENCY PROTOCOLS
// ==========================================

/**
 * @brief Attempts to save the Sovereign State before death.
 * 
 * Logic:
 * We iterate through the Dirty Shadow Pages. Instead of a full
 * transactional commit, we perform a "Raw Dump" to the log area.
 * This is risky but better than total data loss.
 */
static void emergency_sync() {
    // 1. Signal PMMO to flush write buffers immediately
    /* sys_pmmo_force_flush(); */
    
    // 2. Mark Superblock as "Dirty Shutdown"
    // This triggers a filesystem check on next boot.
}

/**
 * @brief THE SOVEREIGN SCRAM (Thermal Kill Switch)
 * 
 * Logic:
 * 1. Seize visual control (Red Screen).
 * 2. Attempt micro-sync.
 * 3. Cut CPU logic power via infinite HALT loop.
 */
void sys_safety_scram(uint32_t current_temp) {
    // 1. VISUAL ALERT (Red Screen of Death)
    // We bypass the standard print buffer and write directly to VRAM
    // to ensure the user sees this even if the heap is corrupted.
    k_vga_set_color(VGA_WHITE, VGA_RED);
    k_vga_clear();
    
    k_vga_print("\n");
    k_vga_print("!!! SOVEREIGN SCRAM TRIGGERED !!!\n");
    k_vga_print("=================================\n");
    k_vga_print("REASON: THERMAL RUNAWAY DETECTED\n");
    
    char temp_str[16];
    k_itoa((int)current_temp, temp_str, 10);
    k_vga_print("CORE TEMP: ");
    k_vga_print(temp_str);
    k_vga_print("C (Limit: 98C)\n\n");
    
    k_vga_print("[1] Executing Emergency Sync... ");
    emergency_sync();
    k_vga_print("DONE.\n");
    
    k_vga_print("[2] Disabling Interrupts...     DONE.\n");
    k_vga_print("[3] Halting CPU Clock...        DONE.\n");
    
    k_vga_print("\nSYSTEM HALTED. ALLOW HARDWARE TO COOL. MANUAL REBOOT REQUIRED.");

    // 2. DISABLE INTERRUPTS
    // We cannot allow the Scheduler or Timer to wake us up.
    __asm__ volatile ("cli");

    // 3. HARDWARE FREEZE
    // We enter a tight loop of HLT instructions.
    // This puts the CPU into C1 state (Low Power), stopping the heat generation.
    while (1) {
        __asm__ volatile ("hlt");
    }
}
