#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys/sys_ffi_jit.h"
#include "sys/sys_ffi_alloc.h"
#include "core/memory.h"
#include "core/vm.h"

// ==========================================
// X86_64 MACHINE CODE EMITTER
// ==========================================

// System V ABI Integer Registers
static const uint8_t gpr_map[] = { 0xBF, 0xBE, 0xBA, 0xB9, 0xB8, 0xB9 }; // RDI, RSI, RDX, RCX, R8, R9

/**
 * @brief Assembler Helper: Emits bytes into a buffer.
 */
static void emit_byte(uint8_t** ptr, uint8_t byte) {
    **ptr = byte;
    (*ptr)++;
}

static void emit_qword(uint8_t** ptr, uint64_t val) {
    memcpy(*ptr, &val, 8);
    *ptr += 8;
}

/**
 * @brief The JIT Emitter Core.
 * Translates the signature into raw x86_64 instructions.
 */
static void generate_x64_thunk(uint8_t* buffer, void* target, WldFFISignature* sig) {
    uint8_t* p = buffer;

    // 1. PROLOGUE
    emit_byte(&p, 0x55); // push rbp
    emit_byte(&p, 0x48); emit_byte(&p, 0x89); emit_byte(&p, 0xE5); // mov rbp, rsp

    // 2. ARGUMENT MARSHALING
    emit_byte(&p, 0x49); emit_byte(&p, 0x89); emit_byte(&p, 0xFF); // mov r15, rdi

    for (int i = 0; i < sig->argCount; i++) {
        if (sig->args[i].regClass == CLASS_INTEGER) {
            // Physically utilize the GPR map and reg_code
            uint8_t reg_code = gpr_map[i];
            
            // mov <reg>, [r15 + offset]
            emit_byte(&p, 0x49); 
            emit_byte(&p, 0x8B); 
            emit_byte(&p, 0x47 + (i * 8)); 
            
            // Finalize register target
            p[-1] |= (reg_code & 0x07); 
        }
    }

    // 3. THE CALL
    emit_byte(&p, 0x48); emit_byte(&p, 0xB8); // mov rax, <addr>
    emit_qword(&p, (uintptr_t)target);
    emit_byte(&p, 0xFF); emit_byte(&p, 0xD0); // call rax

    // 4. EPILOGUE
    emit_byte(&p, 0x5D); // pop rbp
    emit_byte(&p, 0xC3); // ret
}

// ==========================================
// PUBLIC JIT API
// ==========================================

ObjSovereignThunk* ffi_jit_compile(void* cFunc, WldFFISignature sig) {
    // 1. Allocate persistent object
    ObjSovereignThunk* thunk = (ObjSovereignThunk*)reallocate(NULL, 0, sizeof(ObjSovereignThunk));
    thunk->obj.type = OBJ_NATIVE;
    thunk->obj.isMarked = false;
    thunk->obj.next = vm.objects;
    vm.objects = (Obj*)thunk;

    thunk->cFuncPtr = cFunc;
    thunk->signature = sig;

    // 2. Allocate and generate executable machine code
    thunk->thunkSize = 128; // Standard thunk size
    thunk->thunkCode = ffi_alloc_code_segment(thunk->thunkSize);
    
    generate_x64_thunk((uint8_t*)thunk->thunkCode, cFunc, &thunk->signature);

    // 3. Finalize W^X Protection
    ffi_finalize_code_segment(thunk->thunkCode, thunk->thunkSize);

    markCard(thunk);
    return thunk;
}

Value ffi_jit_execute(ObjSovereignThunk* thunk, int argCount, Value* args) {
    // Safety check: ensure arity matches the JIT-compiled signature
    if (argCount != thunk->signature.argCount) return NIL_VAL;

    // Define the function pointer to our generated thunk
    // The thunk signature: Value (*thunk)(Value* args)
    Value (*execute)(Value*) = (Value (*)(Value*))thunk->thunkCode;

    // UFO GRADE SPEED: Direct CPU branch to the thunk
    return execute(args);
}
