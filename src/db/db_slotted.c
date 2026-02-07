#include <string.h>
#include <stdio.h>

#include "db_slotted.h"

// = = = = = = = = = = = = = = = = = = = = = =
// INTERNAL MACROS
// = = = = = = = = = = = = = = = = = = = = = =

#define SLOT_PTR(page, idx) \
    ((WldSlot*)((uint8_t*)(page) + WLD_PAGE_SIZE - (sizeof(WldSlot) * ((idx) + 1))))

// = = = = = = = = = = = = = = = = = = = = = =
// PUBLIC SLOTTED API
// = = = = = = = = = = = = = = = = = = = = = =

void db_slotted_init(void* pageAddr) {
    WldSlottedPage* page = (WldSlottedPage*)pageAddr;
    page->slotCount = 0;
    page->freeSpaceStart = sizeof(WldSlottedPage);
    page->freeSpaceEnd = WLD_PAGE_SIZE; // Slot array grows backwards from here
}

int db_slotted_insert(void* pageAddr, const uint8_t* data, uint32_t size) {
    WldSlottedPage* page = (WldSlottedPage*)pageAddr;

    // UFO Speed Check: Do we have space for the data + a new 4-byte slot?
    uint32_t spaceRequired = size + sizeof(WldSlot);
    uint32_t availableSpace = page->freeSpaceEnd - page->freeSpaceStart;

    if (availableSpace < spaceRequired) {
        // Fragmentation Check: Maybe we have enough space if we compact?
        db_slotted_compact(pageAddr);
        availableSpace = page->freeSpaceEnd - page->freeSpaceStart;
        if (availableSpace < spaceRequired) return -1; // Truly full
    }

    // 1. Write data to the top of the free space
    uint16_t offset = page->freeSpaceStart;
    memcpy((uint8_t*)pageAddr + offset, data, size);
    page->freeSpaceStart += size;

    // 2. Allocate Slot (Reuse empty slots if available)
    int targetSlot = -1;
    for (int i = 0; i < page->slotCount; i++) {
        WldSlot* s = SLOT_PTR(pageAddr, i);
        if (s->size == 0) { // Found a deleted slot hole
            targetSlot = i;
            break;
        }
    }

    if (targetSlot == -1) {
        targetSlot = page->slotCount++;
        page->freeSpaceEnd -= sizeof(WldSlot);
    }

    // 3. Set Slot metadata
    WldSlot* slot = SLOT_PTR(pageAddr, targetSlot);
    slot->offset = offset;
    slot->size = size;

    return targetSlot;
}

uint8_t* db_slotted_get(void* pageAddr, int slotIndex, uint32_t* outSize) {
    WldSlottedPage* page = (WldSlottedPage*)pageAddr;
    if (slotIndex >= page->slotCount) return NULL;

    WldSlot* slot = SLOT_PTR(pageAddr, slotIndex);
    if (slot->size == 0) return NULL; // Deleted record

    if (outSize) *outSize = slot->size;
    return (uint8_t*)pageAddr + slot->offset;
}

void db_slotted_delete(void* pageAddr, int slotIndex) {
    WldSlottedPage* page = (WldSlottedPage*)pageAddr;
    if (slotIndex < page->slotCount) {
        WldSlot* slot = SLOT_PTR(pageAddr, slotIndex);
        slot->size = 0; // Marking size 0 effectively deletes the record
        // We don't change the offset; compaction will handle the hole.
    }
}

// = = = = = = = = = = = = = = = = = = = = = =
// SELF-HEALING COMPACTION
// = = = = = = = = = = = = = = = = = = = = = =

void db_slotted_compact(void* pageAddr) {
    WldSlottedPage* page = (WldSlottedPage*)pageAddr;
    uint8_t temp[WLD_PAGE_SIZE];
    memcpy(temp, pageAddr, WLD_PAGE_SIZE);

    uint16_t currentOffset = sizeof(WldSlottedPage);

    // Repack all active data segments contiguously
    for (int i = 0; i < page->slotCount; i++) {
        WldSlot* oldSlot = SLOT_PTR(temp, i);
        if (oldSlot->size > 0) {
            WldSlot* newSlot = SLOT_PTR(pageAddr, i);
            
            // Move data from temp buffer to current page at new offset
            memcpy((uint8_t*)pageAddr + currentOffset, 
                   temp + oldSlot->offset, 
                   oldSlot->size);
            
            newSlot->offset = currentOffset;
            currentOffset += oldSlot->size;
        }
    }

    page->freeSpaceStart = currentOffset;
    // freeSpaceEnd remains the same (anchored to the end of the slot array)
}
