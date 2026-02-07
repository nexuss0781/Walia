#ifndef WALIA_LIB_STREAM_H
#define WALIA_LIB_STREAM_H

#include "core/common.h"
#include "core/object.h"
#include "core/value.h"

// ==========================================
// STREAM SPECIFICATIONS
// ==========================================

/**
 * @brief Represents a persistent position in a data source.
 * Must be defined before ObjStream to satisfy C declaration order.
 */
typedef struct {
    uint32_t pageId;    
    uint32_t slotIndex; 
    uint64_t totalRead; 
} WldStreamCursor;

/**
 * @brief The Sovereign Stream Object (Phase 19.4 Unified).
 */
typedef struct {
    struct Obj      obj;
    Value           source;      
    WldStreamCursor cursor;     
    bool            isClosed;
    uint32_t        batchSize;      
    uint16_t        shardCount;  // Parallelism factor
} ObjStream;

// ==========================================
// STREAM API
// ==========================================

/**
 * @brief Allocates a new Persistent Stream.
 * Signature synchronized with Phase 19 implementation.
 */
ObjStream* walia_stream_new(Value source, int shardCount);

/**
 * @brief Advances the stream and returns the next item.
 */
bool walia_stream_next(ObjStream* stream, Value* outValue);

/**
 * @brief Physically commits the cursor state to the .wld heap.
 */
void walia_stream_checkpoint(ObjStream* stream);

#endif // WALIA_LIB_STREAM_H
