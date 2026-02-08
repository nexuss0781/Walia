#include <stdio.h>
#include <string.h>

#include "../vm/vm_pager_link.h"
#include "../db/db_slotted.h"
#include "../db/db_pool.h"
#include "../core/memory.h"
#include "../core/vm.h"
#include "../db/db_sentry_mapper.h"

// External access to the shared database resources
extern WldBufferPool global_pool;

// ==========================================
// FUSION LIFECYCLE
// ==========================================

ObjMappedRecord* walia_fusion_map_record(PageID pageId, uint32_t slotIndex) {
    // 1. Allocate the Fusion Descriptor in the Persistent Heap
    ObjMappedRecord* record = (ObjMappedRecord*)reallocate(NULL, 0, sizeof(ObjMappedRecord));

    record->obj.type = OBJ_NATIVE;
    record->obj.isMarked = false;
    record->obj.next = vm.objects;
    vm.objects = (struct Obj*)record;

    record->pageId = pageId;
    record->slotIndex = slotIndex;

    // 2. PINNING: LOCK THE PAGE IN RAM
    // Prevents the Buffer Pool from evicting this data segment while
    // the VM register is physically mapping it to the address space.
    db_pool_pin(&global_pool, pageId);
    record->isPinned = true;

    markCard(record);
    return record;
}

// ==========================================
// DIRECT REGISTER LIFTING (The Fast Path)
// ==========================================

Value walia_fusion_get_property(ObjMappedRecord* record, ObjString* name) {
    // 1. PHYSICAL RESOLUTION
    // Acquire a direct pointer to the memory-mapped 4KB segment.
    void* pageAddr = db_pool_acquire(&global_pool, NULL, record->pageId, false);
    if (!pageAddr) return NIL_VAL;

    // 2. OFFSET DISCOVERY
    // Uses the Sentry Resolver to find the physical offset in O(1).
    int offset = db_sentry_resolve_offset(record->pageId, name);

    if (offset < 0) {
        db_pool_release(&global_pool, record->pageId);
        return NIL_VAL;
    }

    // 3. ZERO-COPY HYDRATION
    // Directly dereference the bit-pattern from the disk image into a register.
    Value* dataPtr = (Value*)((uint8_t*)pageAddr + offset);
    Value result = *dataPtr;

    db_pool_release(&global_pool, record->pageId);
    return result;
}

bool walia_fusion_set_property(ObjMappedRecord* record, ObjString* name, Value value) {
    // 1. TRANSACTIONAL LINKAGE
    // Syncs the VM write operation with the Shadow Pager's ACID lifecycle.
    WldTransaction* tx = vm_get_active_transaction();
    
    // 2. SHADOW PAGE ACQUISITION
    // Triggers Copy-on-Write (CoW) if the page is being modified for the first time.
    void* pageAddr = db_pool_acquire(&global_pool, tx, record->pageId, true);
    if (!pageAddr) return false;

    // 3. OFFSET RESOLUTION
    int offset = db_sentry_resolve_offset(record->pageId, name);
    if (offset < 0) {
        db_pool_release(&global_pool, record->pageId);
        return false;
    }

    // 4. PHYSICAL PERSISTENT WRITE
    Value* dataPtr = (Value*)((uint8_t*)pageAddr + offset);
    *dataPtr = value;

    // 5. WRITE BARRIER
    // Notify the persistence sentry that the hardware segment is dirty.
    markCard(dataPtr);

    db_pool_release(&global_pool, record->pageId);
    return true;
}
