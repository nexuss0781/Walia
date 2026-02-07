#include <stdio.h>
#include <string.h>

#include "vm/vm_sys_ops.h"
#include "sys/sys_types.h"

// ==========================================
// RAW STACK MANIPULATION
// ==========================================
// These functions assume Little-Endian architecture (Standard x64/ARM).

uint8_t sys_vm_read_u8(Value* slot, uint8_t offset) {
    if (offset > 7) return 0; // Safety clamp
    uint8_t* raw = (uint8_t*)slot;
    return raw[offset];
}

void sys_vm_write_u8(Value* slot, uint8_t offset, uint8_t data) {
    if (offset > 7) return;
    uint8_t* raw = (uint8_t*)slot;
    raw[offset] = data;
}

// 16-bit
uint16_t sys_vm_read_u16(Value* slot, uint8_t offset) {
    if (offset > 6) return 0;
    // In production, handle unaligned access if architecture requires
    return *(uint16_t*)((uint8_t*)slot + offset);
}

// 32-bit
uint32_t sys_vm_read_u32(Value* slot, uint8_t offset) {
    if (offset > 4) return 0;
    return *(uint32_t*)((uint8_t*)slot + offset);
}

// ==========================================
// HARDWARE CASTING ENGINE
// ==========================================

Value sys_vm_cast(Value input, WaliaSysType targetType) {
    // 1. Extract raw number
    if (!IS_NUMBER(input)) return NIL_VAL; // Safety check
    double val = AS_NUMBER(input);

    // 2. Perform C-Level Cast
    switch (targetType) {
        case SYS_TYPE_U8:   return NUMBER_VAL((double)((uint8_t)val));
        case SYS_TYPE_U16:  return NUMBER_VAL((double)((uint16_t)val));
        case SYS_TYPE_U32:  return NUMBER_VAL((double)((uint32_t)val));
        
        case SYS_TYPE_I8:   return NUMBER_VAL((double)((int8_t)val));
        case SYS_TYPE_I16:  return NUMBER_VAL((double)((int16_t)val));
        case SYS_TYPE_I32:  return NUMBER_VAL((double)((int32_t)val));

        // For float types, we just pass through since Value is already double
        case SYS_TYPE_F32:  return NUMBER_VAL((double)((float)val));
        case SYS_TYPE_F64:  return input;

        default: return input;
    }
}

Value sys_vm_bit_op(Value a, Value b, OpCode op) {
    if (!IS_NUMBER(a) || !IS_NUMBER(b)) return NIL_VAL;
    
    uint64_t ua = (uint64_t)AS_NUMBER(a);
    uint64_t ub = (uint64_t)AS_NUMBER(b);
    uint64_t result = 0;

    switch (op) {
        case OP_BIT_AND: result = ua & ub; break;
        case OP_BIT_OR:  result = ua | ub; break;
        case OP_BIT_XOR: result = ua ^ ub; break;
        case OP_BIT_SHL: result = ua << ub; break;
        case OP_BIT_SHR: result = ua >> ub; break;
        default: return NIL_VAL;
    }

    return NUMBER_VAL((double)result);
}
