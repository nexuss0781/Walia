#ifndef WALIA_WLD_EVENT_LOOP_H
#define WALIA_WLD_EVENT_LOOP_H

#include "wld_event_common.h"
#include "sys_event_reactor.h"
#include "table.h"
#include "db_dispatch.h"

// ==========================================
// EVENT LOOP SPECIFICATION
// ==========================================

/**
 * @brief The Master Event Orchestrator.
 */
typedef struct {
    WldReactor reactor;
    
    // The Sovereign Registry:
    // Maps File Descriptors (int) to captured Continuations (Value).
    // Using our internal high-performance Table.
    Table      continuationMap;
    
    // Shared MPP Dispatcher for task handover
    WldDispatcher* dispatcher;
    
    bool isRunning;
} WldEventLoop;

// ==========================================
// EVENT LOOP API
// ==========================================

/**
 * @brief Initializes the Event Loop and attaches it to a parallel dispatcher.
 */
SerResult db_event_loop_init(WldEventLoop* loop, WldDispatcher* dispatcher);

/**
 * @brief Registers an asynchronous intent.
 * 
 * Logic:
 * 1. Associates the 'fd' with a 'continuation' (stack slice).
 * 2. Informs the low-level reactor to monitor for 'type' readiness.
 */
SerResult db_event_loop_register(WldEventLoop* loop, int fd, WldEventType type, Value continuation);

/**
 * @brief Executes one "Tick" of the Sovereign Reactor.
 * 
 * UFO SPEED Logic:
 * 1. Polls the hardware reactor for ready I/O.
 * 2. Resolves Continuations from the registry.
 * 3. Dispatches them as Parallel Tasks.
 */
void db_event_loop_step(WldEventLoop* loop);

/**
 * @brief Performs an orderly shutdown and flushes pending events.
 */
void db_event_loop_shutdown(WldEventLoop* loop);

#endif // WALIA_WLD_EVENT_LOOP_H