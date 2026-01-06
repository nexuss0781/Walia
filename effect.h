#ifndef WALIA_EFFECT_H
#define WALIA_EFFECT_H

#include "common.h"
#include "object.h"
#include "value.h"

// ==========================================
// EFFECT HANDLER SPECIFICATION
// ==========================================

/**
 * @brief Represents an active 'handle' block in the VM.
 */
typedef struct {
    ObjString* effectName; // The string identifier (e.g. "AsyncIO")
    int        frameIndex; // Index in vm.frames where the handler starts
    Value*     stackBase;  // Pointer into vm.stack where the handler's scope begins
    uint32_t*  resumeIp;   // The address of the logic following the handle block
} EffectHandler;

/**
 * @brief The Sovereign Continuation (Stack Slice).
 * Resides in the persistent heap, making logic 'immortal'.
 */
typedef struct {
    Obj           obj;
    EffectHandler handler;
    int           destReg; // Target register for the resumed value
    
    // Physical state captures
    int           savedFrameCount;
    Value*        capturedStack;
    int           capturedStackCount;
} ObjContinuation;

// ==========================================
// EFFECT ENGINE API
// ==========================================

/**
 * @brief Freezes the current execution context into a persistent object.
 */
ObjContinuation* captureContinuation(EffectHandler* handler, int destReg);

/**
 * @brief Slices the logic back onto the CPU and continues execution.
 */
void resumeContinuation(ObjContinuation* continuation, Value result);

#endif // WALIA_EFFECT_H
