#include "../core/opcode.h"
#include "../core/vm.h"
#include "../vm/vm_magic_names.h"
#include "../core/value.h"
#include "../core/object.h"
#include "../core/table.h"

// Helper to invoke a method by name (Internal VM call)
// Replicates the OP_CALL logic but from C code
static bool invoke_method(Value receiver, ObjString* name, int argCount) {
    // 1. Find Method
    // (In production, use vm_method_cache here)
    Value method = NIL_VAL;
    
    // Placeholder for class lookup logic
    // For now, we assume receiver is an object that might have the method in its table
    // if (IS_INSTANCE(receiver)) ...
    
    // For Phase 13.2, we support Native Iterators directly
    // This function is a stub for the full MOP dispatch
    return false; 
}

// ==========================================
// ITERATOR DISPATCH HANDLERS
// ==========================================
// These are called by the main VM loop

void vm_op_iter_init(int destReg, Value source) {
    // 1. Call source.__iter__()
    // 2. Store result in destReg
    
    // Native Optimization: If source is a List, creating a NativeIterator is faster
    // than calling a Walia method.
    
    // For now, we treat 'source' as the iterator itself (Python allows this)
    // if (IS_OBJ(source)) ...
    
    // Just move source to dest for this phase until LibIterator is ready
    vm.stack[destReg] = source;
}

bool vm_op_iter_next(int valReg, Value iterator, int jumpOffset) {
    // 1. Call iterator.__next__()
    // 2. If StopIteration effect is thrown -> return false (Jump)
    // 3. Else -> Store result in valReg, return true (Continue)
    
    // Logic requires Effect integration. 
    // In Phase 13, we check for a special sentinel value for speed.
    
    return true; // Continue loop
}
