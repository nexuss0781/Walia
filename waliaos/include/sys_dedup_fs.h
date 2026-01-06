#ifndef WALIA_SYS_DEDUP_FS_H
#define WALIA_SYS_DEDUP_FS_H

#include "walia_kernel_base.h"

// ==========================================
// DEDUP MATRIX SPECIFICATIONS
// ==========================================

/**
 * @brief Represents a unique logic fingerprint on the physical platter.
 */
typedef struct {
    uint8_t  fingerprint[32]; // SHA-256 logic-hash
    uint64_t physicalLBA;     // Hardware sector address
    uint32_t referenceCount;  // Number of virtual objects sharing this bit-pattern
} WldDedupNode;

// ==========================================
// DEDUP MATRIX API
// ==========================================

/**
 * @brief Initializes the persistent deduplication index.
 * Mounts the B+ Tree from the reserved system segment.
 */
void sys_dedup_fs_init();

/**
 * @brief High-velocity search for a logic fingerprint on disk.
 * 
 * @param hash Pointer to the 32-byte SHA-256 identity.
 * @return uint64_t The physical LBA if found, or 0 if unique.
 */
uint64_t sys_dedup_fs_lookup(const uint8_t* hash);

/**
 * @brief Registers a new unique logical block in the disk index.
 */
void sys_dedup_fs_register(const uint8_t* hash, uint64_t lba);

/**
 * @brief Decrements the reference count. 
 * If count reaches 0, the physical sector is returned to the Free List.
 */
void sys_dedup_fs_release(const uint8_t* hash);

#endif // WALIA_SYS_DEDUP_FS_H
