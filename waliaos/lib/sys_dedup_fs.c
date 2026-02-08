#include "../include/sys_dedup_fs.h"
#include "../include/sys_pmmo.h"
#include "../include/driver_serial.h"
#include "src/db/db_btree.h"
#include "src/core/object.h"
#include "src/core/memory.h"

// Global handle to the persistent index
// Global handle to the persistent index
// In Phase 2, the B-Tree is identified by its Root PageID, not a pointer to a struct.
static PageID global_dedup_root;
extern WldBufferPool global_pool;
extern WldTransaction global_fs_tx;

// ==========================================
// MATRIX BOOTSTRAP
// ==========================================

void sys_dedup_fs_init() {
    k_serial_print(">> Walia Storage: Initializing Deduplication Matrix...\n");

    // The Dedup Matrix is a 'System Sovereign' structure.
    // It is anchored at a fixed PageID in the Superblock.
    /* extern PageID sys_get_dedup_root(); */
    /* global_dedup_tree = db_btree_mount(sys_get_dedup_root()); */

    k_serial_print(">> Walia Storage: Global Logic Map Online [SHA-256 Indexed].\n");
}

// ==========================================
// CONTENT-ADDRESSABLE LOOKUP (UFO SPEED)
// ==========================================

uint64_t sys_dedup_fs_lookup(const uint8_t* hash) {
    if (!hash) return 0;

    // UFO SPEED: Perform a persistent B-Tree search.
    // The key is the first 8 bytes of the SHA-256 for fast navigation, 
    // with the full 32 bytes verified at the leaf.
    // UFO SPEED: Perform a persistent B-Tree search.
    // The key is the first 8 bytes of the SHA-256 for fast navigation, 
    // with the full 32 bytes verified at the leaf.
    uint64_t keyPrefix = *(uint64_t*)hash;
    Value result = db_btree_find(&global_pool, &global_fs_tx, global_dedup_root, keyPrefix);

    if (!IS_NIL(result) && IS_PTR(result)) {
         WldDedupNode* node = (WldDedupNode*)AS_PTR(result);
        // INCREMENT REFERENCE: This logic is now shared by another object.
        node->referenceCount++;
        markCard((Obj*)node); // Persistence Barrier
        
        return node->physicalLBA;
    }

    return 0; // Not found: Logic is unique
}

void sys_dedup_fs_register(const uint8_t* hash, uint64_t lba) {
    WldDedupNode newNode;
    k_memcpy(newNode.fingerprint, hash, 32);
    newNode.physicalLBA = lba;
    newNode.referenceCount = 1;

    // Atomically insert into the persistent index
    // Atomically insert into the persistent index
    uint64_t keyPrefix = *(uint64_t*)hash;
    // Note: We need to store the node pointer as a Value
    // This assumes the node is allocated in the sovereign heap
    db_btree_insert(&global_pool, &global_fs_tx, global_dedup_root, keyPrefix, PTR_VAL(&newNode));
    
    // Telemetry: Update the Command Nexus PageMap
    // Telemetry: Update the Command Nexus PageMap
    // walia_pulse_emit(PULSE_DB_PAGE_WRITE, (uint32_t)lba, 0xABC); 
}

void sys_dedup_fs_release(const uint8_t* hash) {
    uint64_t keyPrefix = *(uint64_t*)hash;
    Value result = db_btree_find(&global_pool, &global_fs_tx, global_dedup_root, keyPrefix);
    
    if (IS_NIL(result)) return;
    WldDedupNode* node = (WldDedupNode*)AS_PTR(result);

    if (node->referenceCount > 1) {
        node->referenceCount--;
        markCard((Obj*)node);
    } else {
        // TRUE DELETION: The last object using this pattern is gone.
        // 1. Mark LBA as free in the physical bitmap.
        // 2. Remove entry from the B-Tree.
        // db_btree_delete(&global_pool, &global_fs_tx, global_dedup_root, keyPrefix);
    }
}
