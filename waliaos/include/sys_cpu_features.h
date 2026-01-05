#ifndef WALIA_SYS_CPU_FEATURES_H
#define WALIA_SYS_CPU_FEATURES_H

#include "walia_kernel_base.h"

// ==========================================
// HARDWARE CAPABILITY FLAGS
// ==========================================

typedef enum {
    CPU_FEAT_FPU    = (1 << 0),  // Floating Point Unit
    CPU_FEAT_SSE    = (1 << 1),  // Streaming SIMD Extensions (Basic)
    CPU_FEAT_AVX    = (1 << 2),  // Advanced Vector Extensions (256-bit)
    CPU_FEAT_AVX512 = (1 << 3),  // Ultra-Wide Vectors (512-bit)
    CPU_FEAT_MSR    = (1 << 4),  // Model Specific Registers (Thermal/Power)
    CPU_FEAT_RDRAND = (1 << 5),  // Hardware Random Number Generator
    CPU_FEAT_APIC   = (1 << 6),  // Advanced Programmable Interrupt Controller
    CPU_FEAT_TSC    = (1 << 7)   // Time Stamp Counter (High-Res Timing)
} WldCpuFlag;

/**
 * @brief Global Hardware Identity.
 * Populated at boot, immutable afterwards.
 */
typedef struct {
    char vendor_id[13];
    uint32_t features_bitmap;
    uint32_t max_cpuid_level;
    bool is_safe_mode; // True if hardware is too old for high-performance logic
} WldCpuCaps;

extern WldCpuCaps global_cpu_caps;

// ==========================================
// CAPABILITY API
// ==========================================

/**
 * @brief Interrogates the silicon to build the capability mask.
 * Called immediately after the Pager Init.
 */
void sys_cpu_features_init();

/**
 * @brief Checks if the current hardware supports a specific feature.
 * 
 * UFO SPEED: This is a simple bitwise AND. It takes < 1 cycle.
 * Usage: if (sys_cpu_has(CPU_FEAT_AVX512)) { ... }
 */
static inline bool sys_cpu_has(WldCpuFlag flag) {
    return (global_cpu_caps.features_bitmap & flag) != 0;
}

#endif // WALIA_SYS_CPU_FEATURES_H
