#ifndef WALIA_SYS_MSR_H
#define WALIA_SYS_MSR_H

#include "walia_kernel_base.h"

// ==========================================
// HARDWARE SIGNALING (x86_64 APIC)
// ==========================================

/**
 * @brief Sends a Startup IPI to wake a sleeping core.
 */
static inline void sys_apic_signal_core(uint32_t apic_id) {
    // Write to the Interrupt Command Register (ICR)
    volatile uint32_t* icr_low = (uint32_t*)0xFEE00300;
    volatile uint32_t* icr_hi  = (uint32_t*)0xFEE00310;

    *icr_hi = (apic_id << 24);
    *icr_low = 0x00004000 | 0x40; // Fixed vector + Assert
}

/**
 * @brief Reads the Model Specific Register.
 */
static inline uint64_t sys_read_msr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

#endif // WALIA_SYS_MSR_H
