#ifndef WALIA_SYS_GOVERNOR_H
#define WALIA_SYS_GOVERNOR_H

#include "walia_kernel_base.h"

// ==========================================
// GOVERNOR POLICIES
// ==========================================

typedef enum {
    GOV_MODE_PERFORMANCE, // Run at TjMax - 10C (Risk: High)
    GOV_MODE_BALANCED,    // Run at TjMax - 20C (Standard)
    GOV_MODE_POWERSAVE,   // Aggressive clock gating (Mobile)
    GOV_MODE_SURVIVAL     // Emergency cooling only
} WldPowerPolicy;

/**
 * @brief State of a specific Core's Governor.
 */
typedef struct {
    uint32_t core_id;
    WldPowerPolicy policy;
    uint8_t  current_throttle_level; // 0-15 (0 = 100% speed)
    uint32_t thermal_headroom;
} WldCoreGovernor;

// ==========================================
// GOVERNOR API
// ==========================================

/**
 * @brief Initializes the Governor logic for the local core.
 */
void sys_governor_init();

/**
 * @brief The Metabolic Heartbeat.
 * Called by the Scheduler Tick (1000Hz).
 * 
 * Logic:
 * 1. Reads Instant Temperature.
 * 2. Compares against Policy Thresholds.
 * 3. Adjusts Clock Modulation immediately.
 */
void sys_governor_update();

/**
 * @brief Manually sets the power policy (e.g., for AI Training).
 */
void sys_governor_set_policy(WldPowerPolicy policy);

#endif // WALIA_SYS_GOVERNOR_H
