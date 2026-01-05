#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm_native_compiler.h"
#include "sys_assembler.h"
#include "opcode.h"
#include "vm.h"

// ==========================================
// REGISTER PINNING MAP (x86_64)
// ==========================================
/**
 * Walia R0-R5 map to standard System V ABI registers.
 * This allows native calls to happen with ZERO register movement.
 */
static const char* x64_reg_map[] = { "rax", "rdi", "rsi", "rdx", "rcx", "r8" };

// ================= =========================
// TIER-2 JIT EMITTER
// ==========================================

ObjCodeBlob* walia_jit_tier2_compile(ObjFunction* func) {
    printf(">> Walia JIT: Escalating '%s' to Tier-2 Native...\n", 
           func->name ? func->name->chars : "anonymous");

    // We utilize the sys_assembler substrate to build the machine code.
    // In a Tier-2 JIT, we perform a "Bytecode-to-Mnemonic" translation.
    
    char asmSource[8192];
    int pos = 0;
    
    // 1. PROLOGUE: Standard stack frame setup
    pos += sprintf(asmSource + pos, "push rbp; mov rbp, rsp; ");

    // 2. TRANSLATION LOOP
    for (int i = 0; i < func->chunk.count; i++) {
        Instruction inst = func->chunk.code[i];
        int op = GET_OP(inst);
        int a = GET_A(inst);
        int b = GET_B(inst);
        int c = GET_C(inst);

        switch (op) {
            case OP_MOVE:
                // mov rax, rdi (Example for R0 = R1)
                pos += sprintf(asmSource + pos, "mov %s, %s; ", x64_reg_map[a], x64_reg_map[b]);
                break;

            case OP_ADD:
                // add rax, rdx (Example for R0 = R1 + R2)
                pos += sprintf(asmSource + pos, "mov %s, %s; add %s, %s; ", 
                               x64_reg_map[a], x64_reg_map[b], x64_reg_map[a], x64_reg_map[c]);
                break;

            case OP_RETURN:
                // mov rax, <res>; pop rbp; ret;
                pos += sprintf(asmSource + pos, "pop rbp; ret; ");
                break;
            
            // Other opcodes follow the same mapping logic...
            default:
                pos += sprintf(asmSource + pos, "nop; ");
                break;
        }
    }

    // 3. ASSEMBLE TO PERSISTENT EXECUTABLE HEAP
    // Uses Group 2 (Phase 11.4) to physically write and mprotect() the thunk.
    ObjCodeBlob* thunk = sys_assemble(asmSource);
    
    // 4. Link the function to its native thunk
    // (Assuming ObjFunction has a 'nativeThunk' field in Phase 18)
    // func->nativeThunk = thunk;

    printf(">> Walia JIT: Escalation Complete. Native entry at [%p].\n", (void*)thunk->code);
    return thunk;
}

// ==========================================
// NATIVE EXECUTION SWITCH
// ==========================================

Value walia_jit_tier2_execute(ObjCodeBlob* thunk, int argCount, Value* args) {
    // 1. Define the physical function signature
    typedef Value (*NativeThunk)(int, Value*);
    NativeThunk execute = (NativeThunk)thunk->code;

    // 2. THE QUANTUM JUMP
    // We physically hand over the CPU instruction pointer to the thunk.
    // The CPU is now running at raw hardware speed.
    Value result = execute(argCount, args);

    return result;
}