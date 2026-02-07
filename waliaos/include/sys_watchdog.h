#ifndef WALIA_SYS_WATCHDOG_H
#define WALIA_SYS_WATCHDOG_H

#include "walia_kernel_base.h"

// ==========================================
// IRON DOME SENTRY API
// ==========================================

/**
 * @brief Initializes the local core heartbeat table.
 * Deploys the 'Sovereign Watchdog' to monitor core liveness.
 */
void sys_watchdog_init();

/**
 * @brief Updates the core's heartbeat timestamp.
 * 
 * Logic:
 * This is an atomic, lock-free operation that writes the current 
 * TSC (Time Stamp Counter) to the core's slot in the heartbeat table.
 * 
 * @param core_id The physical ID of the core (0-MAX_CORES).
 */
void sys_watchdog_pulse(int core_id);

/**
 * @brief Scans the heartbeat table for deadlocked cores.
 * Triggers the 'Immune Response' if a core fails to pulse within the threshold.
 */
void sys_watchdog_scan();

#endif // WALIA_SYS_WATCHDOG_H
