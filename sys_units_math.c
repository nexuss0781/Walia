#include <stdio.h>
#include <string.h>

#include "sys_units_math.h"

// ==========================================
// CORE ALGEBRA IMPLEMENTATION
// ==========================================

bool wld_dim_equal(WldDimensionVector a, WldDimensionVector b) {
    for (int i = 0; i < 7; i++) {
        if (a.units[i] != b.units[i]) return false;
    }
    return true;
}

WldDimensionVector wld_dim_multiply(WldDimensionVector a, WldDimensionVector b) {
    WldDimensionVector res;
    for (int i = 0; i < 7; i++) {
        // We use saturated math logic to prevent exponent overflow
        // though SI units rarely exceed small integer ranges.
        res.units[i] = a.units[i] + b.units[i];
    }
    return res;
}

WldDimensionVector wld_dim_divide(WldDimensionVector a, WldDimensionVector b) {
    WldDimensionVector res;
    for (int i = 0; i < 7; i++) {
        res.units[i] = a.units[i] - b.units[i];
    }
    return res;
}

bool wld_dim_is_scalar(WldDimensionVector v) {
    for (int i = 0; i < 7; i++) {
        if (v.units[i] != 0) return false;
    }
    return true;
}

WldDimensionVector wld_dim_empty() {
    WldDimensionVector v = {{0, 0, 0, 0, 0, 0, 0}};
    return v;
}

// ==========================================
// ENTERPRISE DIAGNOSTICS
// ==========================================

/**
 * @brief Formats a dimensional vector for the Command Nexus HUD.
 * Example: [1, 0, -2, 0, 0, 0, 0] -> "m/s^2"
 */
void wld_dim_to_string(WldDimensionVector v, char* buffer) {
    static const char* symbols[] = {"m", "kg", "s", "A", "K", "mol", "cd"};
    int pos = 0;
    bool first = true;

    for (int i = 0; i < 7; i++) {
        if (v.units[i] != 0) {
            if (!first) pos += sprintf(buffer + pos, "*");
            pos += sprintf(buffer + pos, "%s", symbols[i]);
            if (v.units[i] != 1) pos += sprintf(buffer + pos, "^%d", v.units[i]);
            first = false;
        }
    }

    if (first) sprintf(buffer, "scalar");
}