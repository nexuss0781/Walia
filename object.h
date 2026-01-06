#ifndef WALIA_OBJECT_H
#define WALIA_OBJECT_H

#include "common.h"
#include "chunk.h"
#include "value.h"
#include "table.h"
#include "sys_types.h"

// ==========================================
// SOVEREIGN OBJECT TYPES
// ==========================================

typedef enum {
    OBJ_STRING,
    OBJ_FUNCTION,
    OBJ_NATIVE,
    OBJ_CLOSURE,
    OBJ_UPVALUE,
    OBJ_CLASS,
    OBJ_INSTANCE,
    OBJ_BOUND_METHOD,
    OBJ_VECTOR,      // Phase 4
    OBJ_STREAM,      // Phase 17.3
    OBJ_FUTURE,      // Phase 15.2
    OBJ_GENE,        // Phase 17.5
    OBJ_GENOME,      // Phase 17.5
    OBJ_CODE_BLOB    // Phase 11.4
} ObjType;

/**
 * @brief Unified Sovereign Object Header
 * Aligned to 64 bytes to prevent cache-line contention.
 */
typedef struct Obj {
    ObjType type;
    bool isMarked;
    struct Obj* next;
} __attribute__((aligned(64))) Obj;

// ==========================================
// CORE OBJECT STRUCTURES
// ==========================================

typedef struct {
    Obj obj;
    int length;
    uint32_t hash;
    char chars[]; 
} ObjString;

typedef struct {
    Obj obj;
    int arity;
    int upvalueCount;
    Chunk chunk;
    ObjString* name;
    
    // PHASE 18: TIER-2 JIT & PROFILER
    struct ObjCodeBlob* nativeThunk;
    uint32_t callCount;
    bool isAsync;
} ObjFunction;

typedef Value (*NativeFn)(int argCount, Value* args);

typedef struct {
    Obj obj;
    NativeFn function;
} ObjNative;

typedef struct ObjUpvalue {
    Obj obj;
    Value* location;
    Value closed;
    struct ObjUpvalue* next;
} ObjUpvalue;

typedef struct {
    Obj obj;
    ObjFunction* function;
    ObjUpvalue** upvalues;
    int upvalueCount;
} ObjClosure;

// ... (Other structs: ObjClass, ObjInstance, etc.) ...

#define OBJ_TYPE(value)     (AS_OBJ(value)->type)
#define IS_OBJ_TYPE(v, t)   (IS_OBJ(v) && AS_OBJ(v)->type == t)

#endif
