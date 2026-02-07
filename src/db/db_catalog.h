#ifndef WALIA_DB_CATALOG_H
#define WALIA_DB_CATALOG_H

#include "db_common.h"
#include "db_pool.h"

// ==========================================
// CATALOG SPECIFICATIONS
// ==========================================

#define WLD_MAX_ENTRIES_PER_PAGE 30

typedef enum {
    ENTRY_SQL_TABLE,
    ENTRY_NOSQL_COLLECTION,
    ENTRY_VECTOR_INDEX,
    ENTRY_SECURITY_REGISTRY
} WldEntryType;

/**
 * @brief A single registry entry in the persistent catalog.
 */
typedef struct {
    char         name[WLD_MAX_NAME_LEN]; 
    uint32_t     type;        // Castable to WldEntryType
    PageID       rootPage;    // Logical ID of the structure's entry point
    uint8_t      schemaHash[32]; 
    bool         isActive;    
} WldCatalogEntry;

/**
 * @brief The physical layout of Page 1.
 * Optimized for 4096-byte hardware pages.
 */
typedef struct {
    WldPageHeader   header;
    uint32_t        entryCount;
    WldCatalogEntry entries[WLD_MAX_ENTRIES_PER_PAGE];
    uint8_t         padding[136]; 
} WldCatalogPage;

// ==========================================
// CATALOG API
// ==========================================

/**
 * @brief Initializes the catalog on Page 1 of a fresh .wld file.
 */
void db_catalog_init(WldBufferPool* pool, WldTransaction* tx);

/**
 * @brief Registers a new data structure in the sovereign catalog.
 */
WldResult db_catalog_add(WldBufferPool* pool, WldTransaction* tx, 
                         const char* name, WldEntryType type, PageID root);

/**
 * @brief Locates the root page ID for a named table or collection.
 */
PageID db_catalog_lookup(WldBufferPool* pool, WldTransaction* tx, 
                         const char* name, WldEntryType* outType);

/**
 * @brief Prurges an entry from the registry.
 */
WldResult db_catalog_remove(WldBufferPool* pool, WldTransaction* tx, const char* name);

#endif // WALIA_DB_CATALOG_H
