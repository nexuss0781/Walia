#ifndef WALIA_LIB_TIMER_H
#define WALIA_LIB_TIMER_H

#include "../web/wld_event_common.h"
#include "../core/object.h" // Fixed: Added for physical struct Obj definition

// ==========================================
// TIMING WHEEL SPECIFICATIONS
// ==========================================

#define WLD_TIMER_SLOTS 1024
#define WLD_TIMER_RESOLUTION_MS 1

/**
 * @brief A Persistent Timer Task.
 */
typedef struct WldTimerNode {
    struct Obj obj;          
    uint64_t   expireTime;   
    Value      continuation; 
    bool       isInterval;
    uint32_t   intervalMs;
    struct WldTimerNode* next; 
} WldTimerNode;

/**
 * @brief The Sovereign Timing Wheel.
 */
typedef struct {
    WldTimerNode* buckets[WLD_TIMER_SLOTS];
    uint64_t      currentTick; 
    uint64_t      lastRealTime; 
} WldTimingWheel;

// ==========================================
// TIMER API
// ==========================================

void lib_timer_init(WldTimingWheel* wheel);
WldTimerNode* lib_timer_add(WldTimingWheel* wheel, uint32_t delayMs, Value continuation, bool isInterval);
int lib_timer_process(WldTimingWheel* wheel, Value* outContinuations, int maxCount);

#endif // WALIA_LIB_TIMER_H
