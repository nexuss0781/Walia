#ifndef WALIA_DB_SLOTTED_H
#define WALIA_DB_SLOTTED_H

#include "db_common.h"
#include "db_pool.h"

// ==========================================
// SLOTTED PAGE GEOMETRY
// ==========================================

/**
 * @brief A Slot is a 4-byte entry in the Slot Array.
 * It grows from the END of the page toward the beginning.
 */
typedef struct {
    uint16_t offset; // Offset from start of page to data segment
    uint16_t size;   // Length of the variable-width data
} WldSlot;

/**
 * @brief Slotted Page Header.
 * Memory layout: [Header][Data Space...][Free Space...][Slot Array]
 */
typedef struct {
    WldPageHeader header;
    uint16_t slotCount;      // Number of entries in the slot array
    uint16_t freeSpaceStart; // Offset to first byte of available data space
    uint16_t freeSpaceEnd;   // Offset to first byte of the slot array
} WldSlottedPage;

// ==========================================
// SLOTTED PAGE API
// ==========================================

/**
 * @brief Initializes a fresh Slotted Page.
 */
void db_slotted_init(void* pageAddr);

/**
 * @brief Inserts variable-width data into the page.
 * 
 * @return int The slot index (Logical ID within page), or -1 if full.
 */
int db_slotted_insert(void* pageAddr, const uint8_t* data, uint32_t size);

/**
 * @brief Retrieves a pointer to data at a specific slot.
 */
uint8_t* db_slotted_get(void* pageAddr, int slotIndex, uint32_t* outSize);

/**
 * @brief Deletes a slot, marking it as empty.
 */
void db_slotted_delete(void* pageAddr, int slotIndex);

/**
 * @brief Self-Healing Compaction.
 * Shifts all data segments to the top of the page to eliminate 
 * fragmented holes without changing slot indices.
 */
void db_slotted_compact(void* pageAddr);

#endif // WALIA_DB_SLOTTED_H
