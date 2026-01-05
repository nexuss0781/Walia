#ifndef WALIA_PERSISTENCE_H
#define WALIA_PERSISTENCE_H

#include "common.h"
#include "value.h"
#include "table.h"

// ==========================================
// PERSISTENCE CONFIGURATION
// ==========================================

#define WALIA_HEAP_FILE    "walia.state" // Fixed: Identifier for main.c
#define WALIA_MAGIC        0x57414C49 
#define WALIA_VERSION      1

typedef struct {
    uint32_t magic;         
    uint32_t version;       
    uint64_t timestamp;     
    Value    rootTable;     
    uint64_t checksum;      
} WaliaSuperblock;

// ==========================================
// SOVEREIGNTY API
// ==========================================

/**
 * @brief Bootstraps the persistent environment.
 */
bool walia_boot();

/**
 * @brief Commits current VM state to the physical heap file.
 */
void walia_checkpoint();

/**
 * @brief Gracefully shuts down the persistent session.
 */
void walia_shutdown();

void walia_persist(const char* name, Value value);
Value walia_retrieve(const char* name);

#endif // WALIA_PERSISTENCE_H
