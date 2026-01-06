#include "../include/ipc_ring.h"
#include "../include/sys_topology.h"

// Every core has its own private incoming ring buffer
static WldIpcRing incoming_rings[MAX_CORES];

bool sys_ipc_send(uint32_t target, uint32_t type, uint64_t payload) {
    if (target >= MAX_CORES) return false;

    WldIpcRing* ring = &incoming_rings[target];
    
    // 1. ATOMIC ALLOCATION: Claim a slot in the target's ring
    uint32_t head = atomic_fetch_add(&ring->head, 1);
    uint32_t idx = head % IPC_RING_SIZE;

    // 2. DATA DEPOSIT
    WldIpcPulse* p = &ring->buffer[idx];
    p->type = type;
    p->source_id = sys_topology_get_core_count(); // Current core ID
    p->payload = payload;

    return true;
}

int sys_ipc_reap(WldIpcPulse* out, int max) {
    uint32_t my_id = 0; // Resolved via reg_read("core_id") in VM
    WldIpcRing* ring = &incoming_rings[my_id];

    uint32_t current_tail = atomic_load(&ring->tail);
    uint32_t current_head = atomic_load(&ring->head);
    
    int count = 0;
    while (current_tail < current_head && count < max) {
        uint32_t idx = current_tail % IPC_RING_SIZE;
        out[count++] = ring->buffer[idx];
        current_tail++;
    }

    atomic_store(&ring->tail, current_tail);
    return count;
}
