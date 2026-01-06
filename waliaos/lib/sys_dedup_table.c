#include "../include/sys_dedup.h"
#include "../include/driver_serial.h"

// The Global Intelligence Matrix
static WldDedupMatrix global_dedup_matrix;

// ==========================================
// TABLE BOOTSTRAP
// ==========================================

void sys_dedup_init(void* memory_start) {
    k_serial_print(">> Walia Health: Initializing Global Dedup Matrix...\n");

    global_dedup_matrix.frames = (WldDedupEntry*)memory_start;
    global_dedup_matrix.savings_count = 0;

    // Zero-out the atomic slots across the system segment
    for (int i = 0; i < DEDUP_BUCKETS; i++) {
        atomic_init(&global_dedup_matrix.frames[i].logic_hash, 0);
        atomic_init(&global_dedup_matrix.frames[i].physical_addr, 0);
    }

    k_serial_print(">> Walia Health: Logic Folding Engine Online [Wait-Free].\n");
}

// ==========================================
// THE SOVEREIGN FOLD (UFO SPEED)
// ==========================================

uintptr_t sys_dedup_fold(uint32_t hash) {
    if (hash == 0) return 0;

    // 1. Hardware-Direct Indexing
    uint32_t index = hash & DEDUP_MASK;

    // 2. Linear Probe (Limited to 4 slots = 1 cache line)
    // This is the physical limit of memory-access efficiency.
    for (int i = 0; i < 4; i++) {
        WldDedupEntry* slot = &global_dedup_matrix.frames[(index + i) & DEDUP_MASK];
        
        // Load the hash with 'Relaxed' ordering for initial check
        uint32_t storedHash = atomic_load_explicit(&slot->logic_hash, memory_order_relaxed);
        
        if (storedHash == hash) {
            // FOLD DETECTED: We have seen this logic before.
            // Acquire the address and return it for virtual re-mapping.
            return (uintptr_t)atomic_load_explicit(&slot->physical_addr, memory_order_acquire);
        }
        
        if (storedHash == 0) {
            // Reached an empty slot: This logic is unique so far.
            return 0;
        }
    }

    return 0; // Collision/Probe limit reached
}

// ==========================================
// ATOMIC REGISTRATION
// ==========================================

void sys_dedup_register(uint32_t hash, uintptr_t addr) {
    if (hash == 0) return;

    uint32_t index = hash & DEDUP_MASK;

    for (int i = 0; i < 4; i++) {
        WldDedupEntry* slot = &global_dedup_matrix.frames[(index + i) & DEDUP_MASK];
        uint32_t expected = 0;

        // THE ATOMIC HANDSHAKE:
        // Try to claim the slot. If logic_hash is 0, swap it with our hash.
        if (atomic_compare_exchange_strong(&slot->logic_hash, &expected, hash)) {
            // If swap succeeded, store the physical address.
            atomic_store_explicit(&slot->physical_addr, (uint64_t)addr, memory_order_release);
            return;
        }

        // If another core beat us to the same hash, we are done.
        if (expected == hash) return;
    }
}
