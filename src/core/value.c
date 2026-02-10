#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"

// Initialize the array to empty state
void initValueArray(ValueArray* array) {
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

// Append a value, growing memory if needed
void writeValueArray(ValueArray* array, Value value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values, 
                                   oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

// Release all memory
void freeValueArray(ValueArray* array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

// Helper to print values during debugging/REPL
void printValue(Value value) {
#ifdef NAN_BOXING
    if (IS_BOOL(value)) {
        printf(AS_BOOL(value) ? "true" : "false");
    } else if (IS_NIL(value)) {
        printf("nil");
    } else if (IS_NUMBER(value)) {
        printf("%g", AS_NUMBER(value)); // %g strips trailing zeros
    } else if (IS_OBJ(value)) {
        printObject(value);
    }
#else
    // Fallback printing logic
    switch (value.type) {
        case VAL_BOOL:   printf(AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NIL:    printf("nil"); break;
        case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
        case VAL_OBJ:    printObject(value); break;
    }
#endif
}

const char* valueTypeName(Value value) {
#ifdef NAN_BOXING
    if (IS_BOOL(value)) return "bool";
    if (IS_NIL(value)) return "nil";
    if (IS_NUMBER(value)) return "number";
    if (!IS_OBJ(value)) return "unknown";

    switch (OBJ_TYPE(value)) {
        case OBJ_STRING: return "string";
        case OBJ_FUNCTION: return "function";
        case OBJ_NATIVE: return "native";
        case OBJ_CLOSURE: return "closure";
        case OBJ_UPVALUE: return "upvalue";
        case OBJ_CLASS: return "class";
        case OBJ_INSTANCE: return "instance";
        case OBJ_BOUND_METHOD: return "bound_method";
        case OBJ_VECTOR: return "vector";
        case OBJ_STREAM: return "stream";
        case OBJ_FUTURE: return "future";
        case OBJ_GENE: return "gene";
        case OBJ_GENOME: return "genome";
        case OBJ_CODE_BLOB: return "code_blob";
        case OBJ_LIST: return "list";
        case OBJ_MAP: return "map";
        case OBJ_CONTINUATION: return "continuation";
        case OBJ_WAIT_GROUP: return "wait_group";
        default: return "object";
    }
#else
    switch (value.type) {
        case VAL_BOOL: return "bool";
        case VAL_NIL: return "nil";
        case VAL_NUMBER: return "number";
        case VAL_OBJ: break;
        default: return "unknown";
    }

    switch (OBJ_TYPE(value)) {
        case OBJ_STRING: return "string";
        case OBJ_FUNCTION: return "function";
        case OBJ_NATIVE: return "native";
        case OBJ_CLOSURE: return "closure";
        case OBJ_UPVALUE: return "upvalue";
        case OBJ_CLASS: return "class";
        case OBJ_INSTANCE: return "instance";
        case OBJ_BOUND_METHOD: return "bound_method";
        case OBJ_VECTOR: return "vector";
        case OBJ_STREAM: return "stream";
        case OBJ_FUTURE: return "future";
        case OBJ_GENE: return "gene";
        case OBJ_GENOME: return "genome";
        case OBJ_CODE_BLOB: return "code_blob";
        case OBJ_LIST: return "list";
        case OBJ_MAP: return "map";
        case OBJ_CONTINUATION: return "continuation";
        case OBJ_WAIT_GROUP: return "wait_group";
        default: return "object";
    }
#endif
}
