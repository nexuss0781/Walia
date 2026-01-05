#include <stdio.h>
#include <stdint.h>

#include "vm_ptr_ops.h"
#include "vm.h"

// ==========================================
// UNSAFE MEMORY LOGIC
// ==========================================

Value ptr_read(uintptr_t address) {
    // SECURITY CHECK: In a production OS kernel, we would validate 
    // that 'address' is within the process's mapped segments.
    // For Walia Systems Mode, we assume the developer knows what they are doing.
    
    // Cast address to a Value pointer and dereference
    Value* ptr = (Value*)address;
    return *ptr;
}

void ptr_write(uintptr_t address, Value val) {
    // Raw memory write
    Value* ptr = (Value*)address;
    *ptr = val;
}

uintptr_t ptr_get_stack_addr(Value* stackSlot) {
    // Return the virtual address of the stack slot
    return (uintptr_t)stackSlot;
}

// ==========================================
// POINTER DISPATCH LOGIC (For JIT)
// ==========================================

/* 
   CASE(OP_PTR_READ) {
       Instruction inst = READ_INST();
       // R(B) holds the address. R(A) gets the result.
       uintptr_t addr = (uintptr_t)AS_NUMBER(R(GET_B(inst)));
       R(GET_A(inst)) = ptr_read(addr);
       DISPATCH();
   }

   CASE(OP_PTR_WRITE) {
       Instruction inst = READ_INST();
       // R(A) holds address. R(B) holds value.
       uintptr_t addr = (uintptr_t)AS_NUMBER(R(GET_A(inst)));
       ptr_write(addr, R(GET_B(inst)));
       DISPATCH();
   }

   CASE(OP_ADDRESS_OF) {
       Instruction inst = READ_INST();
       // R(B) is the target variable. R(A) gets its address.
       uintptr_t addr = ptr_get_stack_addr(&R(GET_B(inst)));
       R(GET_A(inst)) = NUMBER_VAL((double)addr); // Store pointer as Number
       DISPATCH();
   }
*/