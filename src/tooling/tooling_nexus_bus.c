#include <stdio.h>
#include <string.h>

#include "tooling/tooling_nexus_bus.h"

// The Global Sovereign Bus
static WaliaNexusBus nexusBus;

// ==========================================
// CORE BUS LOGIC
// ==========================================

void walia_pulse_init() {
    atomic_init(&nexusBus.writeIndex, 0);
    nexusBus.readIndex = 0;
    atomic_init(&nexusBus.isActive, true);
    
    // Warm up the buffer memory in the persistent heap segment
    memset(nexusBus.buffer, 0, sizeof(WldPulse) * NEXUS_BUS_CAPACITY);
    
    printf(">> Walia Command Nexus: Pulse Substrate online [Lock-Free Bus Active]\n");
}

/**
 * @brief Sovereign Pulse Emission.
 * 
 * High-Performance Logic:
 * 1. Increment the write index atomically (Lock-free).
 * 2. Write the data to the specific slot.
 * 3. Handle wrap-around via bitwise masking (Capacity is power-of-two).
 */
void walia_pulse_emit(WldPulseType type, uint32_t targetId, uint64_t payload) {
    if (!atomic_load(&nexusBus.isActive)) return;

    // Use bitwise mask for O(1) wrap-around (65535 is capacity-1)
    uint32_t idx = atomic_fetch_add(&nexusBus.writeIndex, 1) & (NEXUS_BUS_CAPACITY - 1);
    
    WldPulse* slot = &nexusBus.buffer[idx];
    
    // Direct store. Memory barrier not strictly required for telemetry data accuracy.
    slot->type = type;
    slot->targetId = targetId;
    slot->payload = payload;
}

/**
 * @brief Pulse Collection for the HUD Consumer.
 * 
 * Implements Adaptive Sampling: if the buffer is overflowing, the consumer 
 * skips to the latest pulses to maintain visual real-time accuracy.
 */
int walia_pulse_collect(WldPulse* outArray, int maxCount) {
    uint32_t currentWrite = atomic_load(&nexusBus.writeIndex);
    uint32_t available = currentWrite - nexusBus.readIndex;

    if (available == 0) return 0;

    // Adaptive Sampling: If we are lagging behind by more than the buffer size,
    // jump forward to the latest events to prevent "Frozen UI" effects.
    if (available > NEXUS_BUS_CAPACITY) {
        nexusBus.readIndex = currentWrite - maxCount;
        available = maxCount;
    }

    int toRead = (available > (uint32_t)maxCount) ? maxCount : (int)available;

    for (int i = 0; i < toRead; i++) {
        uint32_t idx = (nexusBus.readIndex + i) & (NEXUS_BUS_CAPACITY - 1);
        outArray[i] = nexusBus.buffer[idx];
    }

    nexusBus.readIndex += toRead;
    return toRead;
}

bool walia_pulse_is_active() {
    return atomic_load(&nexusBus.isActive);
}

void walia_pulse_shutdown() {
    atomic_store(&nexusBus.isActive, false);
}
