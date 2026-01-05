#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lib_timer.h"
#include "memory.h"
#include "vm.h"

// ==========================================
// INTERNAL UTILITIES
// ==========================================

static uint64_t get_now_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

// ==========================================
// CORE WHEEL LOGIC
// ==========================================

void lib_timer_init(WldTimingWheel* wheel) {
    // Zero out the buckets in the persistent segment
    for (int i = 0; i < WLD_TIMER_SLOTS; i++) {
        wheel->buckets[i] = NULL;
    }
    
    wheel->currentTick = get_now_ms();
    wheel->lastRealTime = (uint64_t)time(NULL);
    
    markCard(wheel);
    printf(">> Walia Timer: Persistent Timing Wheel online [O(1) Dispatch Active]\n");
}

WldTimerNode* lib_timer_add(WldTimingWheel* wheel, uint32_t delayMs, Value continuation, bool isInterval) {
    // 1. Create a Persistent Timer Object
    WldTimerNode* node = (WldTimerNode*)reallocate(NULL, 0, sizeof(WldTimerNode));
    node->obj.type = OBJ_NATIVE;
    node->obj.isMarked = false;
    node->obj.next = vm.objects;
    vm.objects = (struct Obj*)node;

    node->expireTime = get_now_ms() + delayMs;
    node->continuation = continuation;
    node->isInterval = isInterval;
    node->intervalMs = delayMs;

    // 2. Identify Bucket (Modulo arithmetic)
    uint32_t targetSlot = (node->expireTime / WLD_TIMER_RESOLUTION_MS) % WLD_TIMER_SLOTS;
    
    // 3. Insert into Bucket (Head insertion)
    node->next = wheel->buckets[targetSlot];
    wheel->buckets[targetSlot] = node;

    // 4. Persistence Write Barrier
    markCard(node);
    markCard(wheel);

    return node;
}

/**
 * @brief The Heartbeat of the Sovereign Reactor.
 * 
 * This function advances the timing wheel and reaps expired tasks.
 */
int lib_timer_process(WldTimingWheel* wheel, Value* outContinuations, int maxCount) {
    uint64_t now = get_now_ms();
    int count = 0;

    // UFO SPEED: O(1) access to current time slice
    while (wheel->currentTick <= now && count < maxCount) {
        uint32_t slot = (wheel->currentTick / WLD_TIMER_RESOLUTION_MS) % WLD_TIMER_SLOTS;
        WldTimerNode* node = wheel->buckets[slot];
        WldTimerNode* prev = NULL;

        while (node != NULL && count < maxCount) {
            if (node->expireTime <= now) {
                // TIMER EXPIRED!
                outContinuations[count++] = node->continuation;

                // Remove from bucket
                WldTimerNode* expired = node;
                node = node->next;
                if (prev) prev->next = node;
                else wheel->buckets[slot] = node;

                // If interval, reschedule
                if (expired->isInterval) {
                    lib_timer_add(wheel, expired->intervalMs, expired->continuation, true);
                }
                
                // Note: 'expired' memory is eventually reclaimed by GC or manual release
            } else {
                prev = node;
                node = node->next;
            }
        }
        
        wheel->currentTick += WLD_TIMER_RESOLUTION_MS;
    }

    return count;
}