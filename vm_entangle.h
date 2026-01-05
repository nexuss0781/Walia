#ifndef WALIA_VM_ENTANGLE_H
#define WALIA_VM_ENTANGLE_H

#include "common.h"
#include "object.h"
#include "value.h"

// ==========================================
// ENTANGLEMENT SPECIFICATIONS
// ==========================================

#define MAX_ENTANGLE_SOURCES 8

/**
 * @brief Represents a single Quantum Constraint.
 * 
 * Architecture:
 * Links multiple 'Source' registers to one 'Sink' (Target) register 
 * via a bytecode 'Thunk'.
 */
typedef struct {
    struct Obj obj;
    
    // The Recalculation Logic
    struct ObjFunction* thunk; 
    
    // The Geometry of the Entanglement
    int sinkReg;                       // Register to be updated
    int sourceRegs[MAX_ENTANGLE_SOURCES]; // Registers that trigger update
    int sourceCount;
    
    // Persistence Metadata
    uint32_t lastUpdateTick;
} ObjEntanglement;

/**
 * @brief The Global Constraint Registry.
 */
typedef struct {
    // Bitmask for O(1) detection in the VM loop.
    // Bit N is set if Register N is a source for ANY entanglement.
    uint64_t dependencyMask[4]; // Supports up to 256 registers (4 * 64)
    
    // Registry of active entanglements
    Table    sinkMap; // Maps SinkReg -> ObjEntanglement
} WldEntangleRegistry;

// ==========================================
// ENTANGLEMENT CORE API
// ==========================================

/**
 * @brief Initializes the persistent constraint registry.
 */
void walia_entangle_init();

/**
 * @brief Establishes a new entanglement link.
 * 
 * @param sink The register being constrained.
 * @param thunk The logic to execute on trigger.
 * @param sources Array of register indices that drive the sink.
 * @param count Number of sources.
 */
void walia_entangle_register(int sink, struct ObjFunction* thunk, int* sources, int count);

/**
 * @brief High-Speed check for register dependency.
 * Called by VM mutation opcodes (OP_MOVE, OP_ADD).
 */
static inline bool walia_is_entangled(int reg) {
    extern WldEntangleRegistry globalEntangleRegistry;
    return (globalEntangleRegistry.dependencyMask[reg / 64] & (1ULL << (reg % 64))) != 0;
}

#endif // WALIA_VM_ENTANGLE_H