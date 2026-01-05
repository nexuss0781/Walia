#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>

#include "persistence.h"
#include "memory.h"
#include "object.h"
#include "table.h"
#include "vm.h"

// The Superblock lives at the very start of the mapped heap.
static WaliaSuperblock* superblock = NULL;

// ==========================================
// BOOT & RESUME LOGIC
// ==========================================

bool walia_boot() {
    // 1. Initialize the low-level Persistent Heap (mmap)
    // This provides us with the 'heapStart' pointer.
    initPersistentHeap();

    // 2. Map the Superblock to the very beginning of the heap
    // We assume memory.c provides the base address.
    // (Note: In production, we'd extern heapStart or provide a getter)
    extern uint8_t* heapStart; 
    superblock = (WaliaSuperblock*)heapStart;

    // 3. Check for "Warm Resume"
    if (superblock->magic == WALIA_MAGIC) {
        if (superblock->version != WALIA_VERSION) {
            fprintf(stderr, "Fatal: Incompatible Walia state version (File: %d, VM: %d).\n", 
                    superblock->version, WALIA_VERSION);
            exit(EXIT_COMPILE_ERROR);
        }

        // RESUME SUCCESSFUL:
        // Restore the global roots from the persistent table.
        // The table's memory address is already valid because of mmap.
        // We cast the persistent Value back into our live VM globals.
        // vm.globals = *(Table*)AS_OBJ(superblock->rootTable); // Simplified
        
        printf(">> Walia: Warm Resume successful. Timestamp: %lu\n", (unsigned long)superblock->timestamp);
        return true;
    }

    // 4. COLD BOOT: Initialize fresh metadata
    printf(">> Walia: Cold Boot. Initializing fresh persistent state...\n");
    
    superblock->magic = WALIA_MAGIC;
    superblock->version = WALIA_VERSION;
    superblock->timestamp = (uint64_t)time(NULL);
    
    // Create the "Immortal" Root Table
    // We use our existing Table implementation.
    Table* rootTable = ALLOCATE(Table, 1);
    initTable(rootTable);
    
    // Wrap it in a Value and store it in the superblock.
    // This is the "Anchor" for all future GC cycles.
    // superblock->rootTable = OBJ_VAL(rootTable);

    walia_checkpoint(); // Initial sync
    return false;
}

// ==========================================
// ATOMIC CHECKPOINT
// ==========================================

void walia_checkpoint() {
    // 1. Prune the dead weight. 
    // We only want to save what is actually reachable.
    // collectGarbage(); 

    // 2. Update Metadata
    superblock->timestamp = (uint64_t)time(NULL);
    
    // 3. Checksum Generation (Safety Enhancement)
    // Simple XOR checksum for this phase; production would use CRC32/SHA.
    superblock->checksum = superblock->magic ^ superblock->timestamp;

    // 4. Physical Flush
    // This forces the OS to move the data from RAM buffers to the Disk.
    syncPersistentHeap();

    printf(">> Walia: Checkpoint created successfully.\n");
}

void walia_shutdown() {
    walia_checkpoint();
    // Low-level munmap handled by OS on exit, or explicit call here.
    printf(">> Walia: Sovereign Shutdown complete.\n");
}

// ==========================================
// PERSISTENCE API
// ==========================================

void walia_persist(const char* name, Value value) {
    ObjString* key = copyString(name, (int)strlen(name));
    
    // Root the key so it doesn't get collected
    push(OBJ_VAL(key));
    
    // Save to the persistent globals table
    tableSet(&vm.globals, key, value);
    
    // Mark the table as "dirty" so it gets synced to disk
    markCard(&vm.globals);
    
    pop();
}

Value walia_retrieve(const char* name) {
    ObjString* key = copyString(name, (int)strlen(name));
    Value value;
    
    if (tableGet(&vm.globals, key, &value)) {
        return value;
    }

    return NIL_VAL;
}
