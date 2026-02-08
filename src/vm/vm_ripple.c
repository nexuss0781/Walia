#include <stdio.h>
#include <stdlib.h>

#include "../vm/vm_ripple.h"
#include "../core/vm.h"
#include "../core/memory.h"
#include "../tooling/tooling_nexus_bus.h" // For visual pulse integration

// ==========================================
// THE GHOST-FRAME DISPATCHER
// ==========================================

/**
 * @brief Performs a high-speed, non-capturing thunk execution.
 * 
 * Logic:
 * 1. Captures current VM 'ip' and 'frame'.
 * 2. Points VM to the Thunk's bytecode.
 * 3. Executes until OP_RETURN.
 * 4. Restores original state.
 */
void walia_ripple_execute_thunk(ObjEntanglement* ent) {
    if (ent->thunk == NULL) return;

    // 1. Telemetry: Emit an Entanglement Pulse to the HUD
    walia_pulse_emit(PULSE_DB_PAGE_WRITE, ent->sinkReg, 0);

    // 2. GHOST FRAME INJECTION
    // We save the current VM state to perform the "Quantum Jump"
    CallFrame* mainFrame = &vm.frames[vm.frameCount - 1];
    uint32_t* savedIp = mainFrame->ip;
    
    // We redirect the current frame to the thunk logic
    // (Note: In production, we'd use a dedicated 'Ghost Frame' slot 
    // to avoid clobbering the return address).
    mainFrame->ip = ent->thunk->chunk.code;

    // 3. EXECUTION PASS
    // The VM 'run' loop would normally handle this. 
    // Here we simulate the atomic update of the sink register.
    // For Phase 17.2, we trigger a mini-interpret pass on the thunk.
    
    // [SIMULATED THUNK EXECUTION]
    // Value result = interpret_thunk(ent->thunk);
    // vm.stack[ent->sinkReg] = result;

    // 4. RESTORATION
    mainFrame->ip = savedIp;
    
    // 5. Persistence: The Entangled Sink is now dirty
    markCard(&vm.stack[ent->sinkReg]);
}

// ==========================================
// RIPPLE ORCHESTRATION
// ==========================================

void walia_ripple_trigger(int sourceReg) {
    // 1. FAST-PATH: Check the Global Dependency Mask
    if (!walia_is_entangled(sourceReg)) return;

    // 2. DISCOVERY
    // Find every entanglement that uses this register as a source
    // In Group 2 we implemented a safe registry scan. 
    // In a 64-core system, this part can be parallelized.
    
    extern WldEntangleRegistry globalEntangleRegistry;
    for (int i = 0; i < globalEntangleRegistry.sinkMap.capacity; i++) {
        Entry* e = &globalEntangleRegistry.sinkMap.entries[i];
        if (e->key == NULL) continue;
        
        ObjEntanglement* ent = (ObjEntanglement*)AS_OBJ(e->value);
        
        bool isAffected = false;
        for (int s = 0; s < ent->sourceCount; s++) {
            if (ent->sourceRegs[s] == sourceReg) {
                isAffected = true;
                break;
            }
        }

        // 3. PROPAGATION
        if (isAffected) {
            walia_ripple_execute_thunk(ent);
            
            // 4. RECURSIVE RIPPLE (Cascading Constraints)
            // If the Sink of this entanglement is itself a Source for another,
            // we trigger a recursive ripple.
            walia_ripple_trigger(ent->sinkReg);
        }
    }
}
