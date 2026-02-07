#ifndef WALIA_OPCODE_H
#define WALIA_OPCODE_H

#include <stdint.h>

typedef enum {
    // 1. DATA & FLOW
    OP_LOAD_CONST, OP_LOAD_BOOL, OP_LOAD_NIL,
    OP_MOVE, OP_ADD, OP_SUB, OP_MUL,
    OP_DIV, OP_NOT, OP_NEGATE, OP_EQUAL,
    OP_GREATER, OP_LESS, OP_JUMP,
    OP_JUMP_IF_FALSE, OP_LOOP, OP_CALL,
    
    // 2. CLOSURES & SCOPE
    OP_CLOSURE, OP_GET_UPVALUE, OP_SET_UPVALUE,
    OP_RETURN, OP_PRINT, OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL, OP_SET_GLOBAL,
    
    // 3. EFFECTS & CONTINUATIONS
    OP_HANDLE, OP_PERFORM, OP_RESUME,
    
    // 4. OBJECT ORIENTATION
    OP_CLASS, OP_INHERIT, OP_METHOD,
    OP_GET_PROPERTY, OP_SET_PROPERTY, OP_INVOKE,
    
    // 5. ADVANCED PARADIGMS
    OP_ENTANGLE, OP_ITER_INIT, OP_ITER_NEXT,
    OP_JUMP_TABLE, OP_MATCH_SIMILAR,
    
    // 6. BITWISE & LOW-LEVEL
    OP_BIT_AND, OP_BIT_OR, OP_BIT_XOR, OP_BIT_SHL, OP_BIT_SHR,
    OP_PTR_READ, OP_PTR_WRITE, OP_ADDRESS_OF,
    OP_GENE, OP_EVOLVE,
    
    // 7. SYSTEM & MEMORY
    OP_SYSCALL, OP_ASM_EXEC,
    OP_MALLOC, OP_MFREE, OP_SIZEOF,
    
    // 8. DATABASE & SIMD
    OP_DB_SIMD_FILTER_GT, OP_DB_LOAD_COL, OP_DB_COMMIT,
    
    // 9. SYSTEM IO & CASTING
    OP_SYS_LOAD_U8,  OP_SYS_STORE_U8,
    OP_SYS_LOAD_U16, OP_SYS_STORE_U16,
    OP_SYS_LOAD_U32, OP_SYS_STORE_U32,
    OP_SYS_LOAD_U64, OP_SYS_STORE_U64,
    OP_SYS_CAST_lbl // Note: The VM uses OP_SYS_CAST_lbl in the table, but let's call it OP_SYS_CAST
} OpCode;

#define OP_SYS_CAST OP_SYS_CAST_lbl

typedef uint32_t Instruction;

#define PACK_INSTRUCTION(op, a, b, c) \
    ((uint32_t)(op) | ((uint32_t)(a) << 8) | ((uint32_t)(b) << 16) | ((uint32_t)(c) << 24))

#define GET_OP(inst)  ((inst) & 0xFF)
#define GET_A(inst)   (((inst) >> 8) & 0xFF)
#define GET_B(inst)   (((inst) >> 16) & 0xFF)
#define GET_C(inst)   (((inst) >> 24) & 0xFF)
#define GET_BC(inst)  (((inst) >> 16) & 0xFFFF)

#endif
