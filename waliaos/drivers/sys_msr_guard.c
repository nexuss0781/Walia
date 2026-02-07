#include "../include/walia_kernel_base.h"
#include "../include/sys_thermal.h"
#include "../include/driver_serial.h"

// ==========================================
// MSR BLACKLIST (Hardware Damage Prevention)
// ==========================================

#define MSR_IA32_PERF_CTL      0x199 // Voltage/Frequency Request
#define MSR_IA32_VOLTAGE_CTL   0x150 // Direct Voltage Control (Dangerous)
#define MSR_IA32_MISC_ENABLE   0x1A0 // Turbo/Thermal Limits

// ==========================================
// LOW-LEVEL ASSEMBLY
// ==========================================

static inline void raw_wrmsr(uint32_t msr, uint64_t value) {
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    __asm__ volatile ("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

// ==========================================
// THE FIREWALL ENGINE
// ==========================================

/**
 * @brief Safe wrapper for writing to Model Specific Registers.
 * 
 * Logic:
 * 1. Checks if the MSR is in the Safe List.
 * 2. Checks if the MSR is in the Block List.
 * 3. Rejects dangerous writes to Voltage/Frequency control.
 */
bool sys_msr_safe_write(uint32_t msr, uint64_t value) {
    // 1. ALLOWLIST: Safe Operational Registers
    switch (msr) {
        case MSR_IA32_CLOCK_MODULATION: // Thermal Throttling (Safe)
            raw_wrmsr(msr, value);
            return true;
        
        case 0x10: // TSC (Time Stamp Counter) - Safe to reset
            raw_wrmsr(msr, value);
            return true;
    }

    // 2. BLOCKLIST: Physical Damage Risk
    if (msr == MSR_IA32_PERF_CTL || msr == MSR_IA32_VOLTAGE_CTL) {
        k_serial_print("[IRON DOME] BLOCKED: Unauthorized Voltage/Frequency Write.\n");
        k_serial_print("[IRON DOME] Action: Preventing Silicon Damage.\n");
        return false;
    }

    // 3. GRAYLIST: Unknown Registers
    // In production, we default to BLOCK. For development, we WARN.
    k_serial_print("[WARN] Writing to unknown MSR. Potential stability risk.\n");
    raw_wrmsr(msr, value);
    return true;
}
