#include "walia_freestanding.h"
#include "driver_vga.h"
#include "driver_serial.h"
#include "sys_cpu_features.h"
#include "sys_thermal.h"
#include "sys_topology.h"
#include "sys_pager_phys.h"
#include "sys_dedup.h"
#include "sys_governor.h"
#include "sys_watchdog.h"
#include "core/vm.h"

// Defined in linker.ld: marks the boundary of the kernel code
extern uint64_t _kernel_end;

// Sovereign Substrate Entry Points (Phases 1-19)
extern void initVM();
extern void sys_allocator_init_kernel(void* start, size_t size);
extern void sys_watchdog_init();
extern void walia_interpret(const char* source);

/**
 * @brief THE SOVEREIGN IGNITION
 * The first C code ever executed by the CPU in Walia OS.
 */
void walia_kernel_main(uint32_t magic, uint32_t addr) {
    // 1. PHYSICAL HANDOVER
    // Initialize serial first for "Blind Telemetry"
    k_serial_init();
    k_serial_print(">> Walia Kernel: Substrate Ignition Started.\n");

    // Initialize VGA screen for human operator
    k_vga_init();
    k_vga_set_color(VGA_CYAN, VGA_BLACK);
    k_vga_print(">> WALIA SOVEREIGN OS [v3.0.0 GOLD]\n");

    // 2. HARDWARE INTEGRITY AUDIT (Phase 30 Iron Dome)
    sys_cpu_features_init();
    if (global_cpu_caps.is_safe_mode) {
        k_vga_print("[WARN] Incompatible CPU Features. Running in Safe-Scalar mode.\n");
    }
    
    // 3. TOPOLOGY PROBE (Phase 21)
    // Counts the logical CPU cores for the Multikernel Fabric.
    sys_topology_probe();
    int total_cores = sys_topology_get_core_count();
    
    // 4. RAM SOVEREIGNTY (Phase 22)
    // Seize physical memory starting after the kernel binary.
    void* ram_start = (void*)&_kernel_end;
    size_t ram_pool_size = 1024 * 1024 * 128; // Initialize with 128MB reserved for core
    
    sys_allocator_init_kernel(ram_start, ram_pool_size);
    sys_dedup_init(ram_start + (1024 * 1024 * 10)); // Map Dedup matrix at 10MB offset
    k_vga_print("[INFO] RAM Sovereignty Established (Logic Folding Active).\n");

    // 5. THERMAL METABOLISM (Phase 30)
    sys_thermal_init();
    sys_governor_init();
    sys_watchdog_init();
    k_vga_print("[INFO] Iron Dome Safety Sentry Active.\n");

    // 6. SOVEREIGN VM INITIALIZATION
    k_vga_set_color(VGA_GREEN, VGA_BLACK);
    k_vga_print("[READY] Launching Sovereign Mind...\n\n");
    
    // Ignite the Virtual Machine directly on the physical heap
    initVM(); 

    // 7. THE SOVEREIGN JUMP
    // Handing execution to the Walia-native Kernel Soul.
    // This is the moment C is officially deprecated in favor of Walia logic.
    walia_interpret("import 'core/kernel_main'; kernel_main();");

    // 8. KERNEL SURVIVAL LOOP
    // If the logic returns, we enter a low-power infinite wait.
    k_vga_set_color(VGA_RED, VGA_BLACK);
    k_vga_print("\n[ALERT] Sovereign Logic Terminated. System Halted.");
    
    while (1) {
        // Feed the hardware watchdog in the idle loop
        sys_watchdog_pulse(0); 
        __asm__ volatile ("hlt");
    }
}
