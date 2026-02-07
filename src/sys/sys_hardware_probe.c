#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __x86_64__
    #include <cpuid.h>
#endif

#include "sys/sys_hardware_probe.h"

// Singleton instance for global VM access
WldHardwareTopology globalTopology;

// ================= =========================
// ARCHITECTURE-SPECIFIC PROBING
// ==========================================

void sys_hardware_probe_init() {
    printf(">> Walia Hardware Sentry: Commencing Physical Topology Probe...\n");

    // 1. Detect Logic Cores
    globalTopology.coreCount = (int)sysconf(_SC_NPROCESSORS_ONLN);

    // 2. Detect Cache Line Size (Standard logic)
    #ifdef __APPLE__
        globalTopology.cacheLineSize = 64; // Apple Silicon standard
    #else
        globalTopology.cacheLineSize = (int)sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    #endif
    
    if (globalTopology.cacheLineSize <= 0) globalTopology.cacheLineSize = 64;

    // 3. Feature Detection (AVX-512)
    #ifdef __x86_64__
        unsigned int eax, ebx, ecx, edx;
        if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
            globalTopology.hasAVX512 = (ebx & (1 << 16)) != 0; // AVX512F bit
        }
    #elif defined(__aarch64__)
        globalTopology.hasNEON = true; // Always present on ARM64
    #endif

    // 4. SUBSTRATE ADAPTATION
    // We adjust the Pager and MPP parameters based on detected L3 cache.
    // Logic: A Work-Unit should be exactly 50% of the L3 Cache to allow 
    // for two units to coexist during a steal.
    long l3 = sysconf(_SC_LEVEL3_CACHE_SIZE);
    globalTopology.l3CacheSize = (l3 > 0) ? (int)l3 : 8388608; // Default 8MB

    printf(">> Walia Hardware: %d Cores | L1:%d | L3:%d MB | AVX512:%s\n",
           globalTopology.coreCount, globalTopology.cacheLineSize, 
           globalTopology.l3CacheSize / (1024*1024),
           globalTopology.hasAVX512 ? "YES" : "NO");
}
