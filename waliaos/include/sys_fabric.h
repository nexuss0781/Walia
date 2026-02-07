#ifndef WALIA_SYS_FABRIC_H
#define WALIA_SYS_FABRIC_H

#include "walia_kernel_base.h"
#include <stdatomic.h>

// ==========================================
// FABRIC SPECIFICATIONS
// ==========================================

#define MAILBOX_SIZE 512 // 512 messages per core-pair

typedef enum {
    FABRIC_PULSE_EXEC,      // Execute a specific Walia continuation
    FABRIC_PULSE_SYNC,      // Memory barrier synchronization
    FABRIC_PULSE_INVALIDATE,// Flush L1 cache for a specific PageID
    FABRIC_PULSE_SHUTDOWN   // Hard core halt
} WldFabricPulseType;

/**
 * @brief A 128-bit Inter-Core Packet.
 * Optimized for cache-line delivery (4 pulses per 64-byte line).
 */
typedef struct {
    uint32_t type;      // WldFabricPulseType
    uint32_t source_id; // Originating core ID
    uint64_t payload;   // Physical address or Continuation pointer
} WldFabricPulse;

/**
 * @brief The Sovereign Mailbox.
 * A lock-free MPSC (Multi-Producer Single-Consumer) ring buffer.
 */
typedef struct {
    WldFabricPulse buffer[MAILBOX_SIZE];
    atomic_uint    head;
    atomic_uint    tail;
} WldMailbox;

// ==========================================
// INTERCONNECT API
// ==========================================

/**
 * @brief Initializes the Inter-Core Fabric.
 * Maps mailboxes for every discovered core.
 */
void sys_fabric_init(int total_cores);

/**
 * @brief Sends a wait-free pulse to a destination core.
 * UFO SPEED: Uses atomic_fetch_add for lock-free slot reservation.
 */
bool sys_fabric_send(int dest_core, uint32_t type, uint64_t payload);

/**
 * @brief Reaps and executes pulses for the current core.
 */
int sys_fabric_reap(WldFabricPulse* out_buffer, int max_count);

#endif // WALIA_SYS_FABRIC_H
