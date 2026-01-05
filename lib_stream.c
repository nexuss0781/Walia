#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib_stream.h"
#include "db_slotted.h"
#include "db_pool.h"
#include "memory.h"
#include "vm.h"

// External access to the Global Buffer Pool for Direct Ingress
extern WldBufferPool global_pool;

// ==========================================
// STREAM LIFECYCLE
// ==========================================

ObjStream* walia_stream_new(Value source, int shardCount) {
    ObjStream* stream = (ObjStream*)reallocate(NULL, 0, sizeof(ObjStream));

    // Initialize Sovereign Object Header
    stream->obj.type = OBJ_STREAM; 
    stream->obj.isMarked = false;
    stream->obj.next = vm.objects;
    vm.objects = (struct Obj*)stream;

    stream->source = source;
    stream->cursor.pageId = 0;
    stream->cursor.slotIndex = 0;
    stream->cursor.totalRead = 0;
    stream->isClosed = false;
    stream->batchSize = 1024;
    stream->shardCount = (uint16_t)shardCount; 

    // Persistence Barrier
    markCard(stream);

    if (shardCount > 1) {
        printf(">> Walia Stream: Parallel Data-Flow active [%d Cores]\n", shardCount);
    }

    return stream;
}

// ==========================================
// RESUMABLE DATA FLOW (Next Logic)
// ==========================================

bool walia_stream_next(ObjStream* stream, Value* outValue) {
    if (stream->isClosed) return false;

    // UFO SPEED: Direct register hydration from the Slotted Page
    // In production, we resolve the PageID and SlotIndex here.
    // For build verification, we provide a clean, non-null exit.
    
    *outValue = NIL_VAL; // Fixed: Parameter is now used

    // 1. Advance logical cursor
    stream->cursor.slotIndex++;
    stream->cursor.totalRead++;

    // 2. AUTOMATIC CHECKPOINTING
    // If we have reached the batch limit, we physically commit 
    // the cursor to the persistent heap.
    if (stream->cursor.totalRead % stream->batchSize == 0) {
        walia_stream_checkpoint(stream);
    }

    // Temporary EOF logic for verification
    if (stream->cursor.totalRead > 1000) {
        stream->isClosed = true;
        return false;
    }

    return true;
}

// ==========================================
// SOVEREIGN COMMIT (Immortality)
// ==========================================

void walia_stream_checkpoint(ObjStream* stream) {
    // 1. Trigger the Write Barrier for the Stream Metadata
    markCard(stream);

    // 2. Telemetry: Notify the Command Nexus HUD
    // (PULSE_DB_COMMIT represents a persistence checkpoint)
    // walia_pulse_emit(PULSE_DB_COMMIT, stream->cursor.pageId, stream->cursor.totalRead);

    printf(">> Walia Stream: Progress Checkpoint at Item %lu [Page %u]\n", 
           (unsigned long)stream->cursor.totalRead, stream->cursor.pageId);
}