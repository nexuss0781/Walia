#ifndef WALIA_SYS_DEDUP_H
#define WALIA_SYS_DEDUP_H

#include "walia_kernel_base.h"
#include <stdatomic.h>

// ==========================================
// DEDUPLICATION SPECIFICATIONS
// ==========================================

// Supports 1,048,576 unique patterns (1M slots)
#define DEDUP_BUCKETS 1048576 
#define DEDUP_MASK    (DEDUP_BUCKETS - 1)

/**
 * @brief An atomic entry in the global logic map.
 * Total size: 16 bytes. Exactly 4 entries fit in one 64-byte cache line.
 */
typedef struct {
    atomic_uint_least32_t logic_hash;   // SHA-256 Prefix (Identity)
    atomic_uint_least64_t physical_addr; // Physical RAM offset
} WldDedupEntry;

/**
 * @brief The Sovereign Memory Matrix.
 * Resident in the 'System Sovereignty' segment of physical RAM.
 */
typedef struct {
    WldDedupEntry* frames;
    uint64_t       savings_count; // Total bytes saved by folding
} WldDedupMatrix;

// ==========================================
// DEDUP API
// ==========================================

/**
 * @brief Initializes the Memory Folding Engine.
 * @param memory_start Pointer to the reserved system RAM segment.
 */
void sys_dedup_init(void* memory_start);

/**
 * @brief Resolves a logical bit-pattern to a physical RAM frame.
 * @return uintptr_t Physical address if found (Fold), or 0 if unique.
 */
uintptr_t sys_dedup_fold(uint32_t hash);

/**
 * @brief Atomically registers a new unique logic block.
 */
void sys_dedup_register(uint32_t hash, uintptr_t physicalAddr);

#endif // WALIA_SYS_DEDUP_H
