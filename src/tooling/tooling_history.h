#ifndef WALIA_TOOLING_HISTORY_H
#define WALIA_TOOLING_HISTORY_H

#include "core/common.h"
#include "tooling/tooling_registry.h"

// ==========================================
// HISTORY CONFIGURATION
// ==========================================

#define WALIA_HISTORY_DIR ".walia/history"

/**
 * @brief Represents a specific version of a logical block.
 */
typedef struct {
    uint8_t  hash[32];      // Unique content identity
    uint64_t timestamp;     // When this version was captured
    uint32_t byteSize;      // Size of the body text
} WaliaVersionNode;

// ==========================================
// VERSIONING API
// ==========================================

/**
 * @brief Initializes the history storage environment.
 * Creates the .walia/history directory if missing.
 */
void walia_history_init();

/**
 * @brief Archives a logic block's body text.
 * 
 * Logic:
 * 1. Checks if a file named <hash>.wal already exists.
 * 2. If not, writes the content to the history directory.
 * 3. Returns the path to the archived snippet.
 */
const char* walia_history_archive(uint8_t* hash, const char* body, size_t length);

/**
 * @brief Retrieves a specific version of a symbol's logic.
 * Used for "Red" side of Diffs.
 * 
 * @return char* The allocated body text (caller must free).
 */
char* walia_history_get_content(uint8_t* hash);

/**
 * @brief Returns the last known-good hash for a symbol from the registry.
 */
uint8_t* walia_history_get_previous_hash(const char* symbolName);

#endif // WALIA_TOOLING_HISTORY_H
