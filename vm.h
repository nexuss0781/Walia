#ifndef WALIA_VM_H
#define WALIA_VM_H

#include "object.h"
#include "table.h"
#include "value.h"
#include "opcode.h"
#include "db_shadow.h"

// ==========================================
// ARCHITECTURAL LIMITS
// ==========================================
#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * 256)
#define HANDLER_STACK_MAX 64    // Fixed: Defined for effect management
#define TRACE_BUFFER_SIZE 65536 

// ==========================================
// VM INFRASTRUCTURE
// ==========================================

typedef struct {
    Instruction* ip;    
    int regIndex;       
    Value previousValue; 
} TraceEntry;

typedef struct {
    ObjClosure* closure;
    Instruction* ip;
    Value* slots;
    int returnToReg;    
} CallFrame;

/**
 * @brief The Walia Sovereign Virtual Machine.
 */
typedef struct {
    // Standard Register Window
    Value stack[STACK_MAX];
    Value* stackTop;
    
    // Call Stack Metadata
    CallFrame frames[FRAMES_MAX];
    int frameCount;

    // Phase 10: Effect Management
    // Fixed: Explicitly typed pointer for hardware-aligned access
    struct EffectHandler* handlerStack; 
    int handlerCount;

    // Persistence & GC Metadata
    Obj* objects;
    size_t bytesAllocated;
    size_t nextGC;
    Table strings;
    Table globals;
    Table methods;
    
    // Telemetry: Time-Travel Trace Buffer
    TraceEntry* traceBuffer;
    int traceIndex;
    bool isTraceActive;

    // GC Synchronization
    int grayCount;
    int grayCapacity;
    Obj** grayStack;
    ObjUpvalue* openUpvalues;

    // Test Mode Flag
    bool isTestMode;
} VM;

// ==========================================
// SOVEREIGN API
// ==========================================

extern VM vm;

void initVM();
void freeVM();

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
    INTERPRET_TEST_FAILURE
} InterpretResult;

InterpretResult interpret(ObjFunction* function);
void walia_rewind(uint32_t steps);
void walia_hot_swap(ObjString* name, ObjClosure* newImplementation);

void push(Value value);
Value pop();

WldTransaction* vm_get_active_transaction(void);
void runtimeError(const char* format, ...);

#endif // WALIA_VM_H

