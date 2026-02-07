#ifndef WALIA_SYS_UNITS_REGISTRY_H
#define WALIA_SYS_UNITS_REGISTRY_H

#include "sys/sys_units_math.h"

// ==========================================
// UNIT DEFINITION STRUCTURE
// ==========================================

typedef struct {
    const char* name;
    WldDimensionVector vector;
} WldUnitDefinition;

// ==========================================
// REGISTRY API
// ==========================================

/**
 * @brief Bootstraps the standard SI unit table.
 */
void sys_units_init_registry();

/**
 * @brief Resolves a unit name or composite string to a Dimensional Vector.
 * 
 * UFO SPEED: Handles both atomic units ("kg") and 
 * derived composites ("m/s^2") in a single pass.
 * 
 * @param unitStr The string inside the angle brackets (e.g. "m/s").
 * @param length  Length of the string.
 * @return WldDimensionVector The calculated physical identity.
 */
WldDimensionVector sys_units_resolve(const char* unitStr, int length);

#endif // WALIA_SYS_UNITS_REGISTRY_H
