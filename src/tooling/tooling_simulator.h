#ifndef WALIA_TOOLING_SIMULATOR_H
#define WALIA_TOOLING_SIMULATOR_H

#include "../core/common.h"
#include "../core/object.h"
#include "../core/vm.h"

// ==========================================
// SIMULATOR CONFIGURATION
// ==========================================

#define GHOST_STEP_LIMIT 1000 

typedef enum {
    SIM_SUCCESS,
    SIM_ERROR_NIL_ACCESS,
    SIM_ERROR_RECURSION_DEPTH,
    SIM_ERROR_INFINITE_LOOP
} SimResult;

/**
 * @brief Represents the outcome of a "Ghost Run".
 */
typedef struct {
    SimResult status;
    int instructionsExecuted;
    Value finalResult;
} WaliaSimReport;

// ==========================================
// GHOST EXECUTION API
// ==========================================

/**
 * @brief Executes a function in a volatile sandbox.
 */
WaliaSimReport walia_simulate_logic(ObjFunction* func);

/**
 * @brief Heuristic: Detects if a loop is likely infinite.
 */
bool walia_sim_check_loop(Instruction* start, Instruction* end);

#endif // WALIA_TOOLING_SIMULATOR_H
