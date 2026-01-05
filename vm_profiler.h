#ifndef WALIA_VM_PROFILER_H
#define WALIA_VM_PROFILER_H

#include "common.h"
#include "object.h"

// ==========================================
// PROFILER SPECIFICATIONS
// ==========================================

#define WLD_HOT_THRESHOLD 1024 // Calls before Tier-2 JIT triggers
#define WLD_MAX_HOT_SPOTS 256

/**
 * @brief Tracks the "Temperature" of a logic block.
 */
typedef struct {
    ObjFunction* function;
    uint32_t     callCount;
    uint32_t     loopChurn;
    bool         isNative; // True if already Tier-2 JITed
} WldHotSpot;

// ==========================================
// PROFILER API
// ==========================================

/**
 * @brief Initializes the Hot-Spot Sentinel.
 */
void vm_profiler_init();

/**
 * @brief Ticks the profiler for a specific function call.
 * 
 * Logic:
 * 1. Increments callCount.
 * 2. If count > WLD_HOT_THRESHOLD, triggers Tier-2 Native JIT.
 */
void vm_profiler_tick(ObjFunction* func);

/**
 * @brief Identifies if a function requires immediate native escalation.
 */
bool vm_profiler_should_escalate(ObjFunction* func);

#endif // WALIA_VM_PROFILER_H