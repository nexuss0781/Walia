#ifndef WALIA_VALUE_H
#define WALIA_VALUE_H

#include <string.h>
#include "common.h"

// Forward declaration to allow macros to cast to the object header
struct Obj;

// ==========================================
// NAN-BOXING REPRESENTATION
// ==========================================

#ifdef NAN_BOXING

#define QNAN     ((uint64_t)0x7ffc000000000000)
#define TAG_NIL   1
#define TAG_BOOL  2

typedef uint64_t Value;

// Type Checks
#define IS_BOOL(value)      (((value) | 1) == TRUE_VAL)
#define IS_NIL(value)       ((value) == NIL_VAL)
#define IS_NUMBER(value)    (((value) & QNAN) != QNAN) 
#define IS_OBJ(value)       (((value) & (0x8000000000000000 | QNAN)) == (0x8000000000000000 | QNAN))

// Conversions
#define NUMBER_VAL(num)     numToValue(num)
#define BOOL_VAL(b)         ((Value)(QNAN | TAG_BOOL | (b)))
#define NIL_VAL             ((Value)(QNAN | TAG_NIL))
#define OBJ_VAL(obj)        (Value)(0x8000000000000000 | QNAN | (uint64_t)(uintptr_t)(obj))

// Extractions (Use struct Obj* explicitly to avoid typedef race)
#define AS_BOOL(value)      ((value) == TRUE_VAL)
#define AS_NUMBER(value)    valueToNum(value)
#define AS_OBJ(value)       ((struct Obj*)(uintptr_t)((value) & ~(0x8000000000000000 | QNAN)))

#define FALSE_VAL           ((Value)(QNAN | TAG_BOOL | 0))
#define TRUE_VAL            ((Value)(QNAN | TAG_BOOL | 1))

#endif

typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;

static inline Value numToValue(double num) {
    Value value;
    memcpy(&value, &num, sizeof(double));
    return value;
}

static inline double valueToNum(Value value) {
    double num;
    memcpy(&num, &value, sizeof(Value));
    return num;
}

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);

#endif // WALIA_VALUE_H
