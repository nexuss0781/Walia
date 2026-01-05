#ifndef WALIA_VM_PAGER_LINK_H
#define WALIA_VM_PAGER_LINK_H

#include "common.h"
#include "db_pool.h"
#include "value.h"
#include "object.h"

// ==========================================
// FUSION SPECIFICATIONS
// ==========================================

/**
 * @brief Represents a Database Record mapped directly to the VM.
 * 
 * Architecture:
 * Instead of copying a document into a managed 'Instance', the VM 
 * creates this thin descriptor. Accessing a property on this object 
 * triggers a direct memory load from the persistent page.
 */
typedef struct {
    struct Obj obj;
    
    // Physical Linkage
    PageID   pageId;    // The logical PageID in WaliaDB
    uint32_t slotIndex; // The offset within the Slotted Page
    
    // Lifecycle Management
    // We pin the page in the Buffer Pool to ensure the physical 
    // memory address remains valid while the VM register points to it.
    bool     isPinned;
} ObjMappedRecord;

// ==========================================
// SUBSTRATE FUSION API
// ==========================================

/**
 * @brief Lifts a physical database record into the VM.
 * 
 * UFO SPEED: Performs NO copying of data. It simply establishes 
 * the memory-mapped relationship.
 */
ObjMappedRecord* walia_fusion_map_record(PageID pageId, uint32_t slotIndex);

/**
 * @brief Resolves a property from a mapped record.
 * 
 * Logic:
 * 1. Acquires a direct pointer to the Slotted Page via db_pool.
 * 2. Calculates the physical offset using the Slot Table.
 * 3. Returns the NaN-boxed Value directly from the disk-image memory.
 */
Value walia_fusion_get_property(ObjMappedRecord* record, ObjString* name);

/**
 * @brief Updates a property within a mapped record.
 * 
 * Logic:
 * 1. Triggers a Copy-on-Write (Shadow Page) if in a transaction.
 * 2. Performs a direct memory write to the mapped segment.
 * 3. Marks the card as dirty for atomic persistence.
 */
bool walia_fusion_set_property(ObjMappedRecord* record, ObjString* name, Value value);

#endif // WALIA_VM_PAGER_LINK_H