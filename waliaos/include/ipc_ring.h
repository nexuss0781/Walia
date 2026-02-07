#ifndef WALIA_IPC_RING_H
#define WALIA_IPC_RING_H

#include "walia_kernel_base.h"
#include <stdatomic.h>

// ==========================================
// IPC SPECIFICATIONS
// ==========================================

#define IPC_RING_SIZE 1024 // 1024 pulses per core-pair

/**
 * @brief The Sovereign IPC Packet.
 * 16-byte aligned for atomic cache-line delivery.
 */
typedef struct {
    uint32_t type;      // Event type (Read/Write/Sync)
    uint32_t source_id; // Originating Core
    uint64_t payload;   // Physical address or bitmask
} WldIpcPulse;

typedef struct {
    WldIpcPulse buffer[IPC_RING_SIZE];
    atomic_uint head;
    atomic_uint tail;
} WldIpcRing;

// ==========================================
// IPC API
// ==========================================

/**
 * @brief Sends a wait-free pulse to a target core.
 */
bool sys_ipc_send(uint32_t target_core, uint32_t type, uint64_t payload);

/**
 * @brief Reaps pending pulses from the local core's incoming ring.
 */
int sys_ipc_reap(WldIpcPulse* out_buffer, int max_count);

#endif // WALIA_IPC_RING_H
