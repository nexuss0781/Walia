#include <stdio.h>
#include <string.h>

#include "memory.h"
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
        // Will be expanded in Phase 2 for strings/objects
        printf("<obj>"); 
    }
#else
    // Fallback printing logic
    switch (value.type) {
        case VAL_BOOL:   printf(AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NIL:    printf("nil"); break;
        case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
        case VAL_OBJ:    printf("<obj>"); break;
    }
#endif
}
