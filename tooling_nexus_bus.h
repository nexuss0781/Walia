#ifndef WALIA_TOOLING_NEXUS_BUS_H
#define WALIA_TOOLING_NEXUS_BUS_H

#include "common.h"
#include <stdatomic.h>

// ==========================================
// PULSE DEFINITIONS (Tracked Actions)
// ==========================================

typedef enum {
    PULSE_NONE = 0,
    // Data Actions
    PULSE_DB_PAGE_READ,   // Pager access
    PULSE_DB_PAGE_WRITE,  // Shadow page creation
    PULSE_DB_COMMIT,      // Atomic sync
    // Neural Actions
    PULSE_NEURAL_HOP,     // HNSW graph navigation
    PULSE_NEURAL_MATH,    // SIMD calculation cycle
    // Sovereign Actions
    PULSE_GC_MARK,        // Object tracing
    PULSE_GC_SWEEP,       // Memory reclamation
    // Intelligence Actions
    PULSE_SCAN_FILE,      // Content-addressable scan
    PULSE_JIT_COMPILE,    // SQE bytecode generation
    // Async Actions (Phase 19.1)
    PULSE_RESUME_CONTINUATION  // Async continuation resumption
} WldPulseType;

/**
 * @brief A single atomic event packet.
 * Packed to 16 bytes for cache-line efficiency.
 */
typedef struct {
    WldPulseType type;
    uint32_t     targetId; // PageID, HashID, or WorkerID
    uint64_t     payload;  // Micro-benchmarking or bitmask data
} WldPulse;

// ==========================================
// SOVEREIGN BUS API
// ==========================================

// Buffer size: 64k entries (approx 1MB)
#define NEXUS_BUS_CAPACITY 65536

typedef struct {
    WldPulse buffer[NEXUS_BUS_CAPACITY];
    atomic_uint writeIndex; // Advanced by producers (VM/DB threads)
    uint32_t    readIndex;  // Advanced by consumer (HUD thread)
    atomic_bool isActive;
} WaliaNexusBus;

/**
 * @brief Initializes the global pulse substrate.
 */
void walia_pulse_init();

/**
 * @brief Emits a pulse into the ring buffer.
 * 
 * UFO SPEED: This is a lock-free, non-blocking call.
 * Uses atomic fetch-and-add for zero contention across 64 cores.
 */
void walia_pulse_emit(WldPulseType type, uint32_t targetId, uint64_t payload);

/**
 * @brief Collects pending pulses for the HUD.
 * 
 * @param outArray Pre-allocated buffer to store collected pulses.
 * @param maxCount Max pulses to retrieve per frame.
 * @return int Actual pulses retrieved.
 */
int walia_pulse_collect(WldPulse* outArray, int maxCount);

/**
 * @brief Checks if the Nexus HUD is active to skip telemetry if not needed.
 */
bool walia_pulse_is_active();

#endif // WALIA_TOOLING_NEXUS_BUS_H
