#ifndef WALIA_TOOLING_REGISTRY_H
#define WALIA_TOOLING_REGISTRY_H

#include "../core/common.h"
#include "../tooling/tooling_hash_scanner.h"

// ==========================================
// REGISTRY PERSISTENCE CONFIGURATION
// ==========================================

#define WALIA_REGISTRY_FILE "walia.registry"

typedef enum {
    STATUS_CLEAN,   // Logic matches persistent record
    STATUS_DIRTY,   // Logic has changed (Hash mismatch)
    STATUS_NEW,     // Symbol does not exist in registry
    STATUS_CONFLICT // Symbol exists in a DIFFERENT file
} SymbolStatus;

/**
 * @brief Persistent record for a logical symbol.
 */
typedef struct {
    char name[256];
    char filePath[512];
    WaliaBlockType type;
    int startLine;
    int endLine;
    uint8_t currentHash[32];
    SymbolStatus status;
} WaliaRegistryEntry;

// ==========================================
// SOVEREIGN REGISTRY API
// ==========================================

/**
 * @brief Initializes the registry by loading 'walia.registry' from disk.
 */
void walia_registry_init();

/**
 * @brief Attempts to register a scanned block.
 * 
 * Logic:
 * 1. If name exists in DIFFERENT file -> STATUS_CONFLICT.
 * 2. If name exists in SAME file + Hash mismatch -> STATUS_DIRTY.
 * 3. If name doesn't exist -> STATUS_NEW.
 */
WaliaRegistryEntry* walia_registry_add(WaliaSymbolBlock* block);

/**
 * @brief Commits the current registry state to the physical disk.
 */
void walia_registry_save();
/**
 * @brief Resolves a symbol name to its physical source file.
 * UFO SPEED: Performs a direct O(1) hash table lookup.
 * 
 * @return const char* Absolute file path or NULL if not found.
 */
const char* walia_registry_resolve(const char* symbolName);

#endif // WALIA_TOOLING_REGISTRY_H/
