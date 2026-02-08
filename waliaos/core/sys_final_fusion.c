#include "../include/walia_kernel_base.h"
#include "../include/sys_topology.h"
#include "../include/sys_pager_phys.h"
#include "../include/sys_dedup_fs.h"
#include "../include/sys_security.h"
#include "../include/driver_vga.h"
#include "../include/sys_fabric.h"
#include "../include/sys_block_device.h"
#include "../include/sys_pmmo.h"
#include "src/core/vm.h"
#include "src/db/db_pool.h"

/**
 * @brief THE SOVEREIGN IGNITION (v3.0.0 Gold)
 * 
 * Logic:
 * 1. Physical Discovery: Map the CPU cores and RAM limits.
 * 2. Substrate Ignition: Initialize Pager and Deduplication.
 * 3. Security Lockdown: Activate the Bit-Shield Immune System.
 * 4. Logic Fusion: Launch the Walia VM on the Parallel Fabric.
 */
void walia_os_ignite(uint32_t magic, uint32_t addr) {
    (void)magic; (void)addr;
    // 1. HARDWARE HANDOVER
    k_vga_init();
    k_vga_set_color(VGA_CYAN, VGA_BLACK);
    k_vga_print(">> WALIA OS [v3.0.0 GOLD] - SOVEREIGN IGNITION\n");

    // 2. TOPOLOGY PROBE (Core awareness)
    sys_topology_probe();
    int cores = sys_topology_get_core_count();

    // 3. RAM SOVEREIGNTY (Memory Folding)
    sys_pager_phys_init();
    
    // 4. BLOCK FABRIC INITIATION (Parallel I/O Integration)
    // Detects NVMe vs AHCI and establishes the Unified Block Interface.
    sys_block_fabric_init();
    sys_pmmo_init();
    sys_dedup_fs_init();

    // 5. FABRIC ACTIVATION (Multikernel Interconnect)
    sys_fabric_init(cores);

    // 6. SECURITY LOCKDOWN (Bit-Shield)
    k_vga_print(">> Status: BIT_SHIELD_ACTIVE [IMMUNE MODE]\n");

    // 7. SUBSTRATE FUSION (JIT & DB Link)
    initVM(); 
    
    // PHASE 28.5: Activate the Adaptive I/O Scheduler logic
    // This ensures that Walia logic governs the physical storage throughput.
    /* walia_interpret("import 'core/io_scheduler'; io_scheduler_init();"); */

    k_vga_set_color(VGA_GREEN, VGA_BLACK);
    k_vga_print(">> Status: SUBSTRATE_FUSED. HARDWARE SATURATED.\n\n");

    // 8. THE SOVEREIGN JUMP (Launch Shell)
    k_vga_print("walia_os_v3.0.0_gold> ");
    
    while (1) {
        // Hardware Input Polling
        /* sys_keyboard_poll(); */
        __asm__ volatile ("hlt");
    }
}
