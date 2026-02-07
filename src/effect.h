#ifndef WALIA_EFFECT_H
#define WALIA_EFFECT_H

#include "core/common.h"
#include "core/object.h"
#include "core/value.h"

// ==========================================
// EFFECT HANDLER SPECIFICATION
// ==========================================

/**
 * @brief Represents an active 'handle' block in the VM.
 */
typedef struct EffectHandler {
    ObjString* effectName; // The string identifier (e.g. "AsyncIO")
    int        frameIndex; // Index in vm.frames where the handler starts
    Value*     stackBase;  // Pointer into vm.stack where the handler's scope begins
    uint32_t*  resumeIp;   // The address of the logic following the handle block
} EffectHandler;

/**
 * @brief Freezes the current execution context into a persistent object.
 */
ObjContinuation* captureContinuation(EffectHandler* handler, int destReg);

/**
 * @brief Slices the logic back onto the CPU and continues execution.
 */
void resumeContinuation(ObjContinuation* continuation, Value result);

/**
 * @brief High-Velocity Effect Dispatch.
 */
Value perform_effect(const char* effectName);

/**
 * @brief GC integration: Traces captured state for reachability.
 */
void markContinuation(ObjContinuation* cont);

/**
 * @brief Memory management: Frees the physical arrays in a continuation.
 */
void freeContinuation(ObjContinuation* cont);

#endif // WALIA_EFFECT_H
