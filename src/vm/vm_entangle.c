#include <stdio.h>
#include <string.h>

#include "vm/vm_entangle.h"
#include "core/memory.h"
#include "core/vm.h"
#include "core/table.h"

// The Global Sovereign Registry Singleton
WldEntangleRegistry globalEntangleRegistry;

// ==========================================
// REGISTRY LIFECYCLE
// ==========================================

void walia_entangle_init() {
    memset(globalEntangleRegistry.dependencyMask, 0, sizeof(uint64_t) * 4);
    initTable(&globalEntangleRegistry.sinkMap);
    
    printf(">> Walia Entanglement: Constraint Substrate online [O(1) Bitmask Active]\n");
}

// ==========================================
// REGISTRATION ENGINE
// ==========================================

void walia_entangle_register(int sink, struct ObjFunction* thunk, int* sources, int count) {
    // 1. Allocate the Persistent Entanglement Object
    ObjEntanglement* ent = (ObjEntanglement*)reallocate(NULL, 0, sizeof(ObjEntanglement));
    ent->obj.type = OBJ_NATIVE;
    ent->obj.isMarked = false;
    ent->obj.next = vm.objects;
    vm.objects = (struct Obj*)ent;

    ent->thunk = thunk;
    ent->sinkReg = sink;
    ent->sourceCount = (count > MAX_ENTANGLE_SOURCES) ? MAX_ENTANGLE_SOURCES : count;
    ent->lastUpdateTick = 0;

    // 2. Update the Dependency Bitmask
    // This allows the VM to check if a register mutation triggers a ripple
    // in exactly 1 clock cycle during the main loop.
    for (int i = 0; i < ent->sourceCount; i++) {
        int reg = sources[i];
        ent->sourceRegs[i] = reg;
        globalEntangleRegistry.dependencyMask[reg / 64] |= (1ULL << (reg % 64));
    }

    // 3. Store in Registry
    // We use the sink register index as the key.
    char keyStr[16];
    sprintf(keyStr, "%d", sink);
    ObjString* key = copyString(keyStr, (int)strlen(keyStr));
    tableSet(&globalEntangleRegistry.sinkMap, key, OBJ_VAL(ent));

    // 4. Persistence Write Barrier
    markCard(ent);
    markCard(&globalEntangleRegistry.sinkMap);

    printf(">> Walia Entanglement: Linked Sink R%d to %d sources via Thunk [%p]\n", 
           sink, ent->sourceCount, (void*)thunk);
}

// ==========================================
// TRIGGER LOGIC (MPPA Ripple Prep)
// ==========================================

/**
 * @brief Retrieves the entanglement associated with a source register.
 * In production, this uses an inverted index for O(1) trigger lookup.
 */
ObjEntanglement* walia_entangle_get_for_source(int sourceReg) {
    // (Optimization: In Group 3 we will implement the inverted trigger table)
    // For now, we perform a safe registry scan.
    for (int i = 0; i < globalEntangleRegistry.sinkMap.capacity; i++) {
        Entry* e = &globalEntangleRegistry.sinkMap.entries[i];
        if (e->key == NULL) continue;
        
        ObjEntanglement* ent = (ObjEntanglement*)AS_OBJ(e->value);
        for (int s = 0; s < ent->sourceCount; s++) {
            if (ent->sourceRegs[s] == sourceReg) return ent;
        }
    }
    return NULL;
}
