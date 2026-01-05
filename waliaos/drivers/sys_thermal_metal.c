#include "../include/sys_thermal.h"
#include "../include/driver_serial.h"

// Global Tjunction Max (shared across cores for homogeneous CPUs)
static uint32_t global_tj_max = 100; 

// ==========================================
// LOW-LEVEL MSR ACCESS
// ==========================================

static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

static inline void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    __asm__ volatile ("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

// ==========================================
// THERMAL CALIBRATION
// ==========================================

void sys_thermal_init() {
    k_serial_print(">> Walia Health: Calibrating Thermal Sensors...\n");

    // 1. Read Temperature Target (TjMax)
    // Bits 23:16 contain the manufacturing limit (e.g., 100C)
    uint64_t msr_target = rdmsr(MSR_IA32_TEMPERATURE_TARGET);
    global_tj_max = (msr_target >> 16) & 0xFF;

    if (global_tj_max == 0) global_tj_max = 100; // Fallback standard

    // 2. Enable Digital Readout
    // Ensure IA32_THERM_STATUS is valid
    uint64_t status = rdmsr(MSR_IA32_THERM_STATUS);
    if (!(status & (1ULL << 31))) {
        k_serial_print("[WARN] Thermal sensor reading invalid or unsupported.\n");
    }

    k_serial_print(">> Walia Health: T-Junction Calibrated.\n");
}

// ==========================================
// INSTANT HEAT PROBE
// ==========================================

uint32_t sys_thermal_read_current() {
    // 1. Direct Hardware Read
    uint64_t msr = rdmsr(MSR_IA32_THERM_STATUS);

    // 2. Decode Delta
    // Bits 22:16 contain the "Digital Readout" (Delta from TjMax)
    uint32_t delta = (msr >> 16) & 0x7F;
    uint32_t current = global_tj_max - delta;

    // 3. Safety Check (The SCRAM Trigger)
    if (current >= TEMP_CRITICAL) {
        k_serial_print("[CRITICAL] THERMAL RUNAWAY DETECTED. EMERGENCY SCRAM.\n");
        // Force hardware halt loops to drop power immediately
        __asm__ volatile ("cli; hlt");
    }

    return current;
}

// ==========================================
// CLOCK MODULATION (Active Cooling)
// ==========================================

void sys_thermal_apply_throttle(uint8_t level) {
    // Level 0 = 100% Speed
    // Level 8 = 50% Speed
    // Level 15 = 6.25% Speed (Max throttling)
    
    if (level > 15) level = 15;

    uint64_t modulation = rdmsr(MSR_IA32_CLOCK_MODULATION);
    
    // Clear bits 4:1 (Duty Cycle) and Bit 4 (Enable)
    modulation &= ~0x1E; 

    if (level > 0) {
        // Enable On-Demand Clock Modulation (Bit 4)
        // Set Duty Cycle (Bits 3:1) -> 0 means lower duty
        // Note: Logic inverted; higher 'level' param means MORE cooling (slower clock)
        uint8_t duty = 16 - level; 
        modulation |= (1 << 4) | ((duty & 0xF) << 1);
        
        k_serial_print(">> Walia Governor: Thermal Braking Active.\n");
    }

    wrmsr(MSR_IA32_CLOCK_MODULATION, modulation);
}
