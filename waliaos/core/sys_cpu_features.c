#include "../include/sys_cpu_features.h"
#include "../include/driver_serial.h"

WldCpuCaps global_cpu_caps;

// ==========================================
// LOW-LEVEL INTERROGATION (x86_64)
// ==========================================

#if defined(__x86_64__)
static inline void cpuid(int code, uint32_t *a, uint32_t *d) {
    __asm__ volatile ("cpuid" : "=a"(*a), "=d"(*d) : "a"(code) : "ecx", "ebx");
}

static inline void cpuid_ex(int code, int sub, uint32_t *b) {
    uint32_t a, c, d;
    __asm__ volatile ("cpuid" : "=a"(a), "=b"(*b), "=c"(c), "=d"(d) : "a"(code), "c"(sub));
}
#endif

// ==========================================
// FEATURE DISCOVERY ENGINE
// ==========================================

void sys_cpu_features_init() {
    k_serial_print(">> Walia Iron Dome: Scanning Hardware Capabilities...\n");
    
    k_memset(&global_cpu_caps, 0, sizeof(WldCpuCaps));
    global_cpu_caps.is_safe_mode = false;

#if defined(__x86_64__)
    uint32_t eax, edx, ebx;

    // 1. Get Vendor ID
    // Registers EBX, EDX, ECX contain the string (e.g., "GenuineIntel")
    uint32_t vendor[4];
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(vendor[0]), "=d"(vendor[1]), "=c"(vendor[2]) : "a"(0));
    vendor[3] = 0;
    k_memcpy(global_cpu_caps.vendor_id, vendor, 12);
    global_cpu_caps.vendor_id[12] = '\0';
    
    k_serial_print(">> CPU Vendor: ");
    k_serial_print(global_cpu_caps.vendor_id);
    k_serial_print("\n");

    // 2. Scan Standard Features (Leaf 1)
    cpuid(1, &eax, &edx);
    
    if (edx & (1 << 0))  global_cpu_caps.features_bitmap |= CPU_FEAT_FPU;
    if (edx & (1 << 4))  global_cpu_caps.features_bitmap |= CPU_FEAT_TSC;
    if (edx & (1 << 5))  global_cpu_caps.features_bitmap |= CPU_FEAT_MSR;
    if (edx & (1 << 9))  global_cpu_caps.features_bitmap |= CPU_FEAT_APIC;
    if (edx & (1 << 25)) global_cpu_caps.features_bitmap |= CPU_FEAT_SSE;

    // 3. Scan Extended Features (Leaf 1, ECX)
    uint32_t ecx;
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));
    
    if (ecx & (1 << 28)) global_cpu_caps.features_bitmap |= CPU_FEAT_AVX;
    if (ecx & (1 << 30)) global_cpu_caps.features_bitmap |= CPU_FEAT_RDRAND;

    // 4. Scan Advanced Features (Leaf 7, EBX)
    // AVX-512 Foundation is Bit 16
    cpuid_ex(7, 0, &ebx);
    if (ebx & (1 << 16)) {
        global_cpu_caps.features_bitmap |= CPU_FEAT_AVX512;
        k_serial_print(">> Status: SUPERCOMPUTER CLASS (AVX-512 Detected).\n");
    } else {
        k_serial_print(">> Status: STANDARD CLASS (Scalar/SSE Mode).\n");
    }

#elif defined(__aarch64__)
    // ARM64 Feature detection via System Registers
    // Simplified for this stage: Assume NEON is present on all ARMv8
    global_cpu_caps.features_bitmap |= CPU_FEAT_FPU;
    global_cpu_caps.features_bitmap |= CPU_FEAT_SSE; // Mapping NEON to generic vector flag
    k_memcpy(global_cpu_caps.vendor_id, "ARM64_SOC", 10);
#endif

    // 5. SAFETY FALLBACK
    // If no FPU or SSE is found, force Safe Mode (Pure Scalar C-Loops)
    if (!sys_cpu_has(CPU_FEAT_FPU)) {
        global_cpu_caps.is_safe_mode = true;
        k_serial_print("[WARN] Legacy Hardware Detected. Enhancements Disabled.\n");
    }
}
