#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tooling_registry.h"
#include "table.h"
#include "memory.h"
#include "object.h"

// ==========================================
// INTERNAL STORAGE
// ==========================================
// We use Walia's internal high-performance Table to index 
// strings to Registry Entries.

static Table registryTable;
static bool isInitialized = false;

// Prototype for the Interactive Resolver (Implemented in Group 4)
extern void walia_resolve_conflict(WaliaRegistryEntry* existing, WaliaSymbolBlock* incoming);

// ==========================================
// REGISTRY CORE LOGIC
// ==========================================

void walia_registry_init() {
    if (isInitialized) return;
    initTable(&registryTable);

    FILE* file = fopen(WALIA_REGISTRY_FILE, "rb");
    if (file) {
        WaliaRegistryEntry entry;
        while (fread(&entry, sizeof(WaliaRegistryEntry), 1, file)) {
            // Re-build the in-memory hash table for O(1) lookups
            ObjString* name = copyString(entry.name, (int)strlen(entry.name));
            
            // We store the entry in a persistent heap allocation
            WaliaRegistryEntry* heapEntry = ALLOCATE(WaliaRegistryEntry, 1);
            memcpy(heapEntry, &entry, sizeof(WaliaRegistryEntry));
            
            tableSet(&registryTable, name, OBJ_VAL(heapEntry));
        }
        fclose(file);
    }
    
    isInitialized = true;
    printf(">> Walia Registry: Knowledge base loaded [%d symbols]\n", registryTable.count);
}

WaliaRegistryEntry* walia_registry_add(WaliaSymbolBlock* block) {
    ObjString* nameKey = copyString(block->name, (int)strlen(block->name));
    Value existingValue;

    // 1. CHECK FOR PRE-EXISTING SYMBOL
    if (tableGet(&registryTable, nameKey, &existingValue)) {
        WaliaRegistryEntry* existing = (WaliaRegistryEntry*)AS_OBJ(existingValue);

        // CASE A: Global Uniqueness Violation (Conflict)
        if (strcmp(existing->filePath, block->filePath) != 0) {
            existing->status = STATUS_CONFLICT;
            walia_resolve_conflict(existing, block);
            return existing;
        }

        // CASE B: Logic Modification (Dirty)
        if (memcmp(existing->currentHash, block->bodyHash, 32) != 0) {
            existing->status = STATUS_DIRTY;
            // Update metadata but trigger Versioning in Phase 10.3
            memcpy(existing->currentHash, block->bodyHash, 32);
            existing->startLine = block->startLine;
            existing->endLine = block->endLine;
            return existing;
        }

        // CASE C: Unchanged Logic (Clean)
        existing->status = STATUS_CLEAN;
        return existing;
    }

    // 2. REGISTER NEW SYMBOL
    WaliaRegistryEntry* newEntry = ALLOCATE(WaliaRegistryEntry, 1);
    strncpy(newEntry->name, block->name, 256);
    strncpy(newEntry->filePath, block->filePath, 512);
    newEntry->type = block->type;
    newEntry->startLine = block->startLine;
    newEntry->endLine = block->endLine;
    memcpy(newEntry->currentHash, block->bodyHash, 32);
    newEntry->status = STATUS_NEW;

    tableSet(&registryTable, nameKey, OBJ_VAL(newEntry));
    return newEntry;
}

const char* walia_registry_resolve(const char* symbolName) {
    ObjString* nameKey = copyString(symbolName, (int)strlen(symbolName));
    Value entryValue;
    
    if (tableGet(&registryTable, nameKey, &entryValue)) {
        return ((WaliaRegistryEntry*)AS_OBJ(entryValue))->filePath;
    }
    
    return NULL;
}

void walia_registry_save() {
    FILE* file = fopen(WALIA_REGISTRY_FILE, "wb");
    if (!file) return;

    // Iterate through the internal table and write raw entries to disk
    for (int i = 0; i < registryTable.capacity; i++) {
        Entry* slot = &registryTable.entries[i];
        if (slot->key != NULL) {
            WaliaRegistryEntry* entry = (WaliaRegistryEntry*)AS_OBJ(slot->value);
            fwrite(entry, sizeof(WaliaRegistryEntry), 1, file);
        }
    }

    fclose(file);
    printf(">> Walia Registry: Sovereign index synchronized to disk.\n");
}
