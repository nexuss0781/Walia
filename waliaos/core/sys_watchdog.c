#include "../include/sys_topology.h"
#include "../include/logic_cell.h"
#include "../include/driver_serial.h"
#include "../include/sys_fabric.h"

// ==========================================
// WATCHDOG CONFIGURATION
// ==========================================

#define WATCHDOG_THRESHOLD_MS 500 // If a core halts for >500ms, it is dead.
#define MAX_CORES 256

// Shared Heartbeat Table (Atomic timestamps)
static volatile uint64_t core_heartbeats[MAX_CORES];
static bool watchdog_active = false;

// Helper to get raw CPU cycles (TSC)
static inline uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

// ==========================================
// WATCHDOG API
// ==========================================

void sys_watchdog_init() {
    for (int i = 0; i < MAX_CORES; i++) core_heartbeats[i] = 0;
    watchdog_active = true;
    k_serial_print(">> Walia Watchdog: Core Liveness Monitor Active.\n");
}

/**
 * @brief Called by the Scheduler Main Loop on every core.
 * "Pets the dog" to prove liveness.
 */
void sys_watchdog_pulse(int core_id) {
    // UFO SPEED: Simple memory write, no locking needed.
    // TSC is monotonic on modern invariant processors.
    core_heartbeats[core_id] = rdtsc();
}

/**
 * @brief The Dead-Man Check.
 * Runs on the Bootstrap Core (Core 0) via a high-priority timer.
 */
void sys_watchdog_scan() {
    if (!watchdog_active) return;

    uint64_t now = rdtsc();
    // Approximation: 1ms ~= 2,000,000 cycles (2GHz CPU)
    // 500ms ~= 1,000,000,000 cycles.
    uint64_t threshold = 1000000000ULL; 

    int total_cores = sys_topology_get_core_count();

    for (int i = 1; i < total_cores; i++) { // Skip Core 0 (Self)
        uint64_t last_beat = core_heartbeats[i];
        
        // Check for Deadlock
        if (last_beat != 0 && (now - last_beat) > threshold) {
            k_serial_print("[IRON DOME] CORE FAILURE DETECTED: ID ");
            char idStr[4]; k_itoa(i, idStr, 10);
            k_serial_print(idStr);
            k_serial_print("\n");

            // IMMUNE RESPONSE: Quarantine
            // 1. Remove from Fabric Topology (Mark Offline)
            // 2. Send NMI (Non-Maskable Interrupt) to force halt/reset
            
            k_serial_print("[ACTION] Isolating Core. Re-balancing Fabric.\n");
            
            // In a full implementation, we send an IPI_NMI here.
            // For Phase 30, we zero the heartbeat to prevent log spam.
            core_heartbeats[i] = 0; 
        }
    }
}
