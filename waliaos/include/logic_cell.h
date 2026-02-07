#ifndef WALIA_LOGIC_CELL_H
#define WALIA_LOGIC_CELL_H

#include "walia_kernel_base.h"
#include "core/value.h"

// ==========================================
// CELL SPECIFICATIONS
// ==========================================

#define TASK_QUEUE_SIZE 256

typedef enum {
    CELL_IDLE,
    CELL_BUSY,
    CELL_FAULTED
} WldCellStatus;

/**
 * @brief Represents the Local Execution State of a CPU Core.
 * Aligned to 64 bytes to saturate the L1 Cache without false sharing.
 */
typedef struct __attribute__((aligned(64))) {
    uint32_t      core_id;
    WldCellStatus status;
    
    // The Active Context (Phase 10.4 Continuation)
    Value         current_task; 
    
    // Local Task Queue (Lock-Free for local access)
    Value         queue[TASK_QUEUE_SIZE];
    uint16_t      q_head;
    uint16_t      q_tail;

    // Metrics for the Command Nexus
    uint64_t      instructions_retired;
    uint32_t      stolen_tasks_count;
} WldLogicCell;

// ==========================================
// CELL API
// ==========================================

/**
 * @brief Initializes a logic cell for a specific physical core.
 */
void sys_cell_init(WldLogicCell* cell, uint32_t id);

/**
 * @brief Local push (The Fast Path).
 */
bool sys_cell_push_task(WldLogicCell* cell, Value task);

/**
 * @brief Remote pop (The Work-Stealing Path).
 * Uses atomic primitives to allow other cores to 'steal' tasks.
 */
Value sys_cell_steal_task(WldLogicCell* cell);

#endif // WALIA_LOGIC_CELL_H
