#include "../include/sys_governor.h"
#include "../include/sys_thermal.h"
#include "../include/driver_serial.h"

// Local Core State (Thread-Local in full SMP, static here for MVP)
static WldCoreGovernor local_gov;

// ==========================================
// THRESHOLD MATRIX
// ==========================================

static uint32_t get_target_temp(WldPowerPolicy policy) {
    switch (policy) {
        case GOV_MODE_PERFORMANCE: return TEMP_THROTTLE - 5; // Run hot (~85C)
        case GOV_MODE_POWERSAVE:   return 50;                // Run cool
        case GOV_MODE_SURVIVAL:    return 40;                // Ice cold
        default:                   return 70;                // Balanced
    }
}

// ==========================================
// GOVERNOR ENGINE
// ==========================================

void sys_governor_init() {
    local_gov.core_id = 0; // In SMP, read APIC ID
    local_gov.policy = GOV_MODE_BALANCED;
    local_gov.current_throttle_level = 0;
    local_gov.thermal_headroom = 100;

    k_serial_print(">> Walia Governor: Metabolic Controller Online.\n");
}

void sys_governor_set_policy(WldPowerPolicy policy) {
    local_gov.policy = policy;
    // Reset modulation on policy change
    sys_thermal_apply_throttle(0); 
    local_gov.current_throttle_level = 0;
}

void sys_governor_update() {
    // 1. SENSE: Direct Hardware Read
    uint32_t temp = sys_thermal_read_current();
    uint32_t target = get_target_temp(local_gov.policy);

    // 2. DECIDE: Hysteresis Logic
    // We avoid rapid oscillation by requiring a delta.
    
    if (temp > target) {
        // TOO HOT: Increase throttling (slow down)
        if (local_gov.current_throttle_level < 15) {
            local_gov.current_throttle_level++;
            
            // Log if we are crossing into dangerous territory
            if (temp > TEMP_WARN) {
                k_serial_print("[GOVERNOR] Thermal ceiling approached. Throttling ++.\n");
            }
        }
    } else if (temp < (target - 5)) {
        // TOO COLD: Decrease throttling (speed up)
        if (local_gov.current_throttle_level > 0) {
            local_gov.current_throttle_level--;
        }
    }

    // 3. ACT: Physical Modulation
    sys_thermal_apply_throttle(local_gov.current_throttle_level);
    
    // Update State
    local_gov.thermal_headroom = (temp < 100) ? (100 - temp) : 0;
}
