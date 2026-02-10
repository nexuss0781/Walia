#include "../include/sys_cpu.h"
#include "../include/driver_vga.h"
#include "../include/driver_serial.h"
#include "../include/sys_cpu_features.h"
#include "../include/sys_thermal.h"

/* 
 * External Walia VM Entry Points
 */
extern void walia_vm_init();
extern void sys_allocator_init_kernel(void* start, size_t size);
extern void sys_watchdog_init(); // Phase 30.8

// Defined in linker.ld
extern uint32_t _kernel_end;

/**
 * @brief The Sovereign Bridge.
 */
void walia_kernel_init(uint32_t magic, uint32_t addr) {
    (void)addr;
    
    // 0. ESTABLISH SEGMENT & INTERRUPT SOVEREIGNTY
    sys_gdt_init();
    sys_idt_init();
    sys_paging_init();

    // 1. INITIALIZE HARDWARE INTERFACES
    k_vga_init();
    k_serial_init();

    k_vga_set_color(VGA_CYAN, VGA_BLACK);
    k_vga_print(">> WALIA SOVEREIGN OS [v3.0.0]\n");

    // 2. IRON DOME: HARDWARE CAPABILITY SCOUT (Phase 30.5)
    // We probe the CPU capabilities before doing anything complex.
    sys_cpu_features_init();

    if (global_cpu_caps.is_safe_mode) {
        k_vga_set_color(VGA_YELLOW, VGA_BLACK);
        k_vga_print("[WARN] Legacy Hardware. Engaging Safe Mode.\n");
    }

    // 3. IRON DOME: WATCHDOG INIT (Phase 30.8)
    // Start the Dead-Man timer.
    sys_watchdog_init();

    // 4. VALIDATE BOOTLOADER
    if (magic != 0x36d76289 && magic != 0x2BADB002) {
        k_vga_set_color(VGA_RED, VGA_BLACK);
        k_vga_print("[FAULT] Multiboot2 Magic Mismatch.\n");
        // We'll proceed for now, but in a hardened state this would trigger a lockdown.
    }

    // 5. SEIZE PHYSICAL RAM
    void* ram_ocean_start = (void*)&_kernel_end;
    size_t ram_ocean_size = 1024 * 1024 * 128; // 128 MB for Genesis
    sys_allocator_init_kernel(ram_ocean_start, ram_ocean_size);
    k_vga_print("[INFO] RAM Sovereignty Established.\n");

    // 6. THERMAL SENSOR CALIBRATION (Phase 30.1)
    // Read TjMax to prepare the governor.
    sys_thermal_init();

    // 7. HANDOVER TO WALIA LOGIC
    k_vga_set_color(VGA_GREEN, VGA_BLACK);
    k_vga_print("[READY] Launching Sovereign Mind...\n\n");
    walia_vm_init();
    
    // Safety Halt
    while (1) { __asm__ volatile ("hlt"); }
}
