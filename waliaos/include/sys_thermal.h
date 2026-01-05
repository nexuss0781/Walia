#ifndef WALIA_SYS_THERMAL_H
#define WALIA_SYS_THERMAL_H

#include "walia_kernel_base.h"

// ==========================================
// THERMAL MSR DEFINITIONS (Intel/AMD x64)
// ==========================================

#define MSR_IA32_THERM_STATUS      0x0000019C
#define MSR_IA32_THERM_INTERRUPT   0x0000019B
#define MSR_IA32_TEMPERATURE_TARGET 0x000001A2
#define MSR_IA32_CLOCK_MODULATION  0x0000019A

// ==========================================
// SAFETY THRESHOLDS (Celsius)
// ==========================================

#define TEMP_WARN     80  // Start task migration
#define TEMP_THROTTLE 90  // Inject clock wait-states
#define TEMP_CRITICAL 98  // Emergency SCRAM (Shutdown)

/**
 * @brief Represents the thermal state of a physical core.
 */
typedef struct {
    uint32_t core_id;
    uint32_t current_temp;
    uint32_t tj_max;      // Fabrication limit (usually 100C)
    bool     is_throttling;
    uint64_t last_check_tsc;
} WldThermalZone;

// ==========================================
// THERMAL SENTRY API
// ==========================================

/**
 * @brief Calibrates the thermal sensors against the CPU's Tjunction Max.
 */
void sys_thermal_init();

/**
 * @brief Reads the instant temperature of the current core.
 * 
 * UFO SPEED: Reads directly from MSR (No syscall/driver overhead).
 * @return uint32_t Temperature in Celsius.
 */
uint32_t sys_thermal_read_current();

/**
 * @brief Injects 'Duty Cycle' modulation to cool the core.
 * @param level 0 (Full Speed) to 15 (Lowest Speed).
 */
void sys_thermal_apply_throttle(uint8_t level);

#endif // WALIA_SYS_THERMAL_H
