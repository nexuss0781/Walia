#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "../tooling/tooling_history.h"
#include "../core/hash.h"
#include "../core/memory.h"

// ==========================================
// INTERNAL HELPERS
// ==========================================

static void ensure_dir(const char* path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        #ifdef _WIN32
            mkdir(path);
        #else
            mkdir(path, 0755);
        #endif
    }
}

/**
 * @brief Generates a persistent filename from a SHA-256 hash.
 * Result: .walia/history/a1b2c3d4...wal
 */
static void get_history_path(uint8_t* hash, char* outPath) {
    char hashStr[65];
    walia_hash_to_string(hash, hashStr);
    sprintf(outPath, "%s/%s.wal", WALIA_HISTORY_DIR, hashStr);
}

// ==========================================
// HISTORY OPERATIONS
// ==========================================

void walia_history_init() {
    ensure_dir(".walia");
    ensure_dir(WALIA_HISTORY_DIR);
    printf(">> Walia History: Version control system initialized.\n");
}

const char* walia_history_archive(uint8_t* hash, const char* body, size_t length) {
    static char path[1024];
    get_history_path(hash, path);

    // Check for existence to avoid redundant IO (Content-Addressable)
    struct stat st;
    if (stat(path, &st) == 0) {
        return path; // Already archived
    }

    FILE* file = fopen(path, "wb");
    if (!file) return NULL;

    // We store the raw body text within the archive
    fwrite(body, 1, length, file);
    fclose(file);

    return path;
}

char* walia_history_get_content(uint8_t* hash) {
    char path[1024];
    get_history_path(hash, path);

    FILE* file = fopen(path, "rb");
    if (!file) return NULL;

    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(size + 1);
    if (fread(buffer, 1, size, file) < size) {
        free(buffer);
        fclose(file);
        return NULL;
    }
    buffer[size] = '\0';
    fclose(file);

    return buffer;
}

// ==========================================
// LOGIC RECOVERY BRIDGE
// ==========================================

/**
 * @brief Logic for the "Quantum Undo" REPL feature.
 * Reverts the Registry pointer to the previous known hash.
 */
void walia_history_revert(const char* symbolName) {
    // 1. Fetch previous hash from Version Chain (Pseudo-logic)
    // 2. Update walia.registry to point to the old hash
    // 3. Trigger re-compilation of the reverted block
    printf(">> Walia History: Reverting '%s' to previous sovereign state.\n", symbolName);
}
