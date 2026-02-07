#ifndef WALIA_OBJECT_H
#define WALIA_OBJECT_H

#include "common.h"
#include "chunk.h"
#include "value.h"
#include "table.h"
#include "sys/sys_types.h"

// ==========================================
// FORWARD DECLARATIONS
// ==========================================
struct CallFrame;

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
    OBJ_CODE_BLOB,   // Phase 11.4
    OBJ_LIST,        // Added for compatibility
    OBJ_MAP,         // Added for compatibility
    OBJ_CONTINUATION, // Core for Algebraic Effects
    OBJ_WAIT_GROUP    // Added for compatibility
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

typedef struct ObjString {
    Obj obj;
    int length;
    uint32_t hash;
    char chars[]; 
} ObjString;

ObjString* copyString(const char* chars, int length);
ObjString* takeString(char* chars, int length);

typedef struct ObjFunction {
    Obj obj;
    int arity;
    int upvalueCount;
    Chunk chunk;
    ObjString* name;
    
    // PHASE 18: TIER-2 JIT & PROFILER
    struct ObjCodeBlob* nativeThunk;
    uint32_t callCount;
    bool isAsync;
    uint8_t fingerprint[32]; // UFO Grade: Cryptographic Identity
} ObjFunction;

ObjFunction* newFunction();

typedef Value (*NativeFn)(int argCount, Value* args);

typedef struct ObjNative {
    Obj obj;
    NativeFn function;
} ObjNative;

ObjNative* newNative(NativeFn function);

typedef struct ObjUpvalue {
    Obj obj;
    Value* location;
    Value closed;
    struct ObjUpvalue* next;
} ObjUpvalue;

ObjUpvalue* newUpvalue(Value* slot);

typedef struct ObjClosure {
    Obj obj;
    ObjFunction* function;
    ObjUpvalue** upvalues;
    int upvalueCount;
} ObjClosure;

ObjClosure* newClosure(ObjFunction* function);

typedef struct ObjCodeBlob {
    Obj obj;
    int size;
    uint8_t code[];
} ObjCodeBlob;

ObjCodeBlob* newCodeBlob(int size);

typedef struct ObjVector {
    Obj      obj;
    uint32_t dimensions;
    float    magnitude;    // For O(1) Cosine Similarity
    uint16_t format;       // WldVectorFormat (F32, SQ8, etc)
    uint16_t unused;       // Padding
    float    data[] __attribute__((aligned(64))); 
} ObjVector;

typedef struct ObjContinuation {
    Obj           obj;
    int           frameCount;
    struct CallFrame* capturedFrames; 
    int           stackCount;
    Value*        capturedStack;
    int           returnToReg; 
} ObjContinuation;

typedef struct ObjFuture {
    Obj obj;
    int status;
    Value result;
    ObjContinuation* waiter;
    void* groupOwner; // Points to ObjWaitGroup
    int groupIndex;
} ObjFuture;

typedef struct ObjWaitGroup {
    Obj obj;
    int mode;
    int pendingCount; // atomic_int in implementation
    int totalCount;
    void* results;    // ObjList*
    ObjContinuation* parentCont;
} ObjWaitGroup;

typedef struct ObjClass {
    Obj obj;
    ObjString* name;
    Table methods;
} ObjClass;

ObjClass* newClass(ObjString* name);

typedef struct ObjInstance {
    Obj obj;
    ObjClass* klass;
    Table fields;
} ObjInstance;

ObjInstance* newInstance(ObjClass* klass);

typedef struct ObjBoundMethod {
    Obj obj;
    Value receiver;
    ObjClosure* method;
} ObjBoundMethod;

ObjBoundMethod* newBoundMethod(Value receiver, ObjClosure* method);

#define OBJ_TYPE(value)     (AS_OBJ(value)->type)
#define IS_OBJ_TYPE(v, t)   (IS_OBJ(v) && AS_OBJ(v)->type == t)

#define IS_STRING(value)       IS_OBJ_TYPE(value, OBJ_STRING)
#define IS_FUNCTION(value)     IS_OBJ_TYPE(value, OBJ_FUNCTION)
#define IS_NATIVE(value)       IS_OBJ_TYPE(value, OBJ_NATIVE)
#define IS_CLOSURE(value)      IS_OBJ_TYPE(value, OBJ_CLOSURE)
#define IS_CLASS(value)        IS_OBJ_TYPE(value, OBJ_CLASS)
#define IS_INSTANCE(value)     IS_OBJ_TYPE(value, OBJ_INSTANCE)
#define IS_BOUND_METHOD(value) IS_OBJ_TYPE(value, OBJ_BOUND_METHOD)
#define IS_CONTINUATION(value) IS_OBJ_TYPE(value, OBJ_CONTINUATION)

#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)      (((ObjString*)AS_OBJ(value))->chars)
#define AS_FUNCTION(value)     ((ObjFunction*)AS_OBJ(value))
#define AS_NATIVE(value)       (((ObjNative*)AS_OBJ(value))->function)
#define AS_CLOSURE(value)      ((ObjClosure*)AS_OBJ(value))
#define AS_CLASS(value)        ((ObjClass*)AS_OBJ(value))
#define AS_INSTANCE(value)     ((ObjInstance*)AS_OBJ(value))
#define AS_BOUND_METHOD(value) ((ObjBoundMethod*)AS_OBJ(value))
#define AS_CONTINUATION(value) ((ObjContinuation*)AS_OBJ(value))

#endif
