#ifndef WALIA_TOOLING_HASH_SCANNER_H
#define WALIA_TOOLING_HASH_SCANNER_H

#include "../core/common.h"
#include "../core/hash.h"

// ==========================================
// SYMBOL BLOCK DEFINITION
// ==========================================

typedef enum {
    BLOCK_FUNCTION,
    BLOCK_CLASS
} WaliaBlockType;

/**
 * @brief Represents a logical block extracted from source code.
 */
typedef struct {
    char name[256];          // Name of the function or class
    WaliaBlockType type;     // Function vs Class
    char filePath[512];      // Absolute path to the source file
    int startLine;           // Start of the block logic
    int endLine;             // End of the block logic
    uint8_t bodyHash[32];    // SHA-256 of the content inside the braces
} WaliaSymbolBlock;

// ==========================================
// HASH-SENTRY SCANNER API
// ==========================================

/**
 * @brief Orchestrates a project-wide scan for .wal files.
 * 
 * Logic:
 * 1. Probes the root and subdirectories.
 * 2. Filters for '.wal' extension.
 * 3. Extracts every fun/class block and hashes their bodies.
 * 
 * @param directory The root project path.
 */
void walia_scan_project(const char* directory);

/**
 * @brief Analyzes a specific Walia file to extract and hash its symbols.
 */
void walia_scan_file(const char* path);

#endif // WALIA_TOOLING_HASH_SCANNER_H/
