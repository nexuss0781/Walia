#ifndef WALIA_OPCODE_H
#define WALIA_OPCODE_H

#include <stdint.h>

typedef enum {
    // 1. DATA & FLOW
    OP_LOAD_CONST, OP_LOAD_BOOL, OP_LOAD_NIL, OP_MOVE,
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,
    OP_NOT, OP_NEGATE, OP_EQUAL, OP_GREATER, OP_LESS,

    // 2. SYSTEMS & BITWISE (Phase 11/17)
    OP_BIT_AND, OP_BIT_OR, OP_BIT_XOR, OP_BIT_SHL, OP_BIT_SHR,
    OP_PTR_READ, OP_PTR_WRITE, OP_ADDRESS_OF,
    OP_SYSCALL, OP_ASM_EXEC,
    OP_MALLOC, OP_MFREE, OP_SIZEOF,

    // 3. LOGIC & CONTROL (Phase 10/14/15)
    OP_JUMP, OP_JUMP_IF_FALSE, OP_LOOP, OP_CALL, OP_RETURN,
    OP_CLASS, OP_METHOD, OP_INVOKE,
    OP_GET_PROPERTY, OP_SET_PROPERTY,
    OP_HANDLE, OP_PERFORM, OP_RESUME,

    // 4. ADVANCED PARADIGMS (Phase 17)
    OP_ENTANGLE,      // Quantum Logic
    OP_ITER_INIT,     // Pipeline Start
    OP_ITER_NEXT,     // Pipeline Step
    OP_JUMP_TABLE,    // Match Dispatch
    OP_MATCH_SIMILAR, // Neural Match
    OP_GENE,          // Genetic Schema
    OP_EVOLVE,        // Breeding

    // 5. DATABASE & SIMD (Phase 5/8)
    OP_DB_LOAD_COL,
    OP_DB_SIMD_FILTER_GT,
    OP_DB_COMMIT,

    // 6. SYSTEM
    OP_PRINT
} OpCode;

typedef uint32_t Instruction;

#define PACK_INSTRUCTION(op, a, b, c) \
    ((uint32_t)(op) | ((uint32_t)(a) << 8) | ((uint32_t)(b) << 16) | ((uint32_t)(c) << 24))

#define GET_OP(inst)  ((inst) & 0xFF)
#define GET_A(inst)   (((inst) >> 8) & 0xFF)
#define GET_B(inst)   (((inst) >> 16) & 0xFF)
#define GET_C(inst)   (((inst) >> 24) & 0xFF)
#define GET_BC(inst)  (((inst) >> 16) & 0xFFFF)

#endif
