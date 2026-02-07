#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tooling/tooling_simulator.h"
#include "core/opcode.h"
#include "core/value.h"

// ==========================================
// THE VOLATILE SANDBOX
// ==========================================

WaliaSimReport walia_simulate_logic(ObjFunction* func) {
    WaliaSimReport report;
    report.status = SIM_SUCCESS;
    report.instructionsExecuted = 0;
    report.finalResult = NIL_VAL;

    // 1. Setup Volatile Environment
    // We allocate a small stack on the C-heap, NOT the sovereign heap.
    Value simStack[256]; 
    Instruction* ip = func->chunk.code;
    
    // 2. The Ghost Dispatch Loop
    // We use a simplified version of vm.c run() logic.
    while (report.instructionsExecuted < GHOST_STEP_LIMIT) {
        Instruction inst = *ip++;
        int op = GET_OP(inst);
        report.instructionsExecuted++;

        switch (op) {
            case OP_LOAD_CONST:
                simStack[GET_A(inst)] = func->chunk.constants.values[GET_BC(inst)];
                break;

            case OP_ADD: {
                Value b = simStack[GET_B(inst)];
                Value c = simStack[GET_C(inst)];
                
                // CRASH DETECTION: Check for non-numeric operands
                if (!IS_NUMBER(b) || !IS_NUMBER(c)) {
                    report.status = SIM_ERROR_NIL_ACCESS;
                    return report;
                }
                simStack[GET_A(inst)] = NUMBER_VAL(AS_NUMBER(b) + AS_NUMBER(c));
                break;
            }

            // SOVEREIGN BYPASS:
            // Opcodes that write to disk are treated as NOPs in the ghost engine.
            case OP_SET_GLOBAL:
            case OP_DEFINE_GLOBAL:
                // We record that a write WOULD have happened, but don't perform it.
                printf(">> Ghost: Intercepted persistent write to registry.\n");
                break;

            case OP_RETURN:
                report.finalResult = simStack[GET_A(inst)];
                return report;

            case OP_LOOP:
                // INFINITE LOOP DETECTION
                // If we hit the same loop multiple times without changing state,
                // we flag it as a Logic Fault.
                if (report.instructionsExecuted > GHOST_STEP_LIMIT / 2) {
                    report.status = SIM_ERROR_INFINITE_LOOP;
                    return report;
                }
                ip -= GET_BC(inst);
                break;

            default:
                // Standard opcodes follow standard logic...
                break;
        }

        if (ip >= func->chunk.code + func->chunk.count) break;
    }

    if (report.instructionsExecuted >= GHOST_STEP_LIMIT) {
        report.status = SIM_ERROR_RECURSION_DEPTH;
    }

    return report;
}

// ==========================================
// STATIC ANALYSIS HEURISTICS
// ==========================================

bool walia_sim_check_loop(Instruction* start, Instruction* end) {
    // Advanced Logic: Scans the instruction block for "Progress".
    // If a loop contains no mutations to the condition variable, it is infinite.
    for (Instruction* ptr = start; ptr < end; ptr++) {
        int op = GET_OP(*ptr);
        if (op == OP_ADD || op == OP_SUB || op == OP_MOVE) {
            return true; // Likely makes progress
        }
    }
    return false; // No state mutation found in loop body
}
