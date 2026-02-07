#ifndef WALIA_DB_PAGER_H
#define WALIA_DB_PAGER_H

#include "db_common.h"

// ==========================================
// THE PAGER STRUCTURE
// ==========================================

typedef struct {
    int fd;                 // File descriptor for .wld
    uint8_t* fileMapping;   // Start of mmap'd memory
    uint64_t fileSize;      // Size in bytes
    uint32_t totalPages;    // Logical capacity
    
    WldSuperblock* header;  // Direct pointer to Page 0
} WldPager;

// ==========================================
// PAGER INTERFACE API
// ==========================================

/**
 * @brief Opens or creates a WaliaDB file and maps it to memory.
 */
WldPager* db_pager_open(const char* path);

/**
 * @brief Retrieves a direct pointer to a 4KB page.
 * UFO-Grade Speed: Direct memory access.
 */
void* db_pager_get_page(WldPager* pager, PageID pageId);

/**
 * @brief Allocates a new logical page, growing the file if necessary.
 */
PageID db_pager_allocate(WldPager* pager);

/**
 * @brief Synchronizes a specific memory-mapped page to disk.
 */
void db_pager_flush(WldPager* pager, PageID pageId);

/**
 * @brief Unmaps the memory and closes the database file.
 */
void db_pager_close(WldPager* pager);

extern WldPager* global_pager;

#endif // WALIA_DB_PAGER_H
