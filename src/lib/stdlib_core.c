#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>

#include "../core/common.h"
#include "../core/object.h"
#include "../core/value.h"
#include "../core/table.h"
#include "../core/vm.h"
#include "../core/memory.h"

// Helper defined in native.c
extern void defineNative(const char* name, NativeFn function);

// ==========================================
// ADVANCED MATHEMATICS (SIMD-Friendly)
// ==========================================

static Value mathSqrt(int argCount, Value* args) {
    if (argCount != 1 || !IS_NUMBER(args[0])) return NIL_VAL;
    return NUMBER_VAL(sqrt(AS_NUMBER(args[0])));
}

static Value mathAbs(int argCount, Value* args) {
    if (argCount != 1 || !IS_NUMBER(args[0])) return NIL_VAL;
    return NUMBER_VAL(fabs(AS_NUMBER(args[0])));
}

static Value mathPow(int argCount, Value* args) {
    if (argCount != 2 || !IS_NUMBER(args[0]) || !IS_NUMBER(args[1])) return NIL_VAL;
    return NUMBER_VAL(pow(AS_NUMBER(args[0]), AS_NUMBER(args[1])));
}

// ==========================================
// SOVEREIGN STRING OPERATIONS
// ==========================================

/**
 * @brief Zero-Allocation Substring logic.
 * Utilizes the Walia Intern Table to ensure identical substrings
 * are deduplicated in the persistent heap.
 */
static Value stringSub(int argCount, Value* args) {
    if (argCount != 3 || !IS_STRING(args[0]) || !IS_NUMBER(args[1]) || !IS_NUMBER(args[2])) {
        return NIL_VAL;
    }

    ObjString* source = AS_STRING(args[0]);
    int start = (int)AS_NUMBER(args[1]);
    int length = (int)AS_NUMBER(args[2]);

    if (start < 0 || start >= source->length || length < 0 || (start + length) > source->length) {
        return NIL_VAL;
    }

    // copyString handles the interning and persistent allocation automatically
    return OBJ_VAL(copyString(source->chars + start, length));
}

static Value stringLen(int argCount, Value* args) {
    if (argCount != 1 || !IS_STRING(args[0])) return NIL_VAL;
    return NUMBER_VAL(AS_STRING(args[0])->length);
}

static Value stringFind(int argCount, Value* args) {
    if (argCount != 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) return NIL_VAL;
    
    char* haystack = AS_STRING(args[0])->chars;
    char* needle = AS_STRING(args[1])->chars;
    char* result = strstr(haystack, needle);

    if (result == NULL) return NUMBER_VAL(-1);
    return NUMBER_VAL((double)(result - haystack));
}

// ==========================================
// CROSS-PLATFORM PATH LOGIC
// ==========================================

static Value pathExists(int argCount, Value* args) {
    if (argCount != 1 || !IS_STRING(args[0])) return NIL_VAL;
    struct stat buffer;
    return BOOL_VAL(stat(AS_CSTRING(args[0]), &buffer) == 0);
}

static Value pathJoin(int argCount, Value* args) {
    if (argCount != 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) return NIL_VAL;
    
    ObjString* a = AS_STRING(args[0]);
    ObjString* b = AS_STRING(args[1]);

#ifdef _WIN32
    char sep = '\\';
#else
    char sep = '/';
#endif

    int newLen = a->length + b->length + 1;
    char* newChars = ALLOCATE(char, newLen + 1);
    
    memcpy(newChars, a->chars, a->length);
    newChars[a->length] = sep;
    memcpy(newChars + a->length + 1, b->chars, b->length);
    newChars[newLen] = '\0';

    // Lift into the persistent intern table
    Value result = OBJ_VAL(takeString(newChars, newLen));
    return result;
}

// ==========================================
// CORE REFLECTION & SYSTEM
// ==========================================

static Value waliaTypeOf(int argCount, Value* args) {
    if (argCount != 1) return NIL_VAL;
    
    if (IS_BOOL(args[0]))   return OBJ_VAL(copyString("Boolean", 7));
    if (IS_NIL(args[0]))    return OBJ_VAL(copyString("Nil", 3));
    if (IS_NUMBER(args[0])) return OBJ_VAL(copyString("Number", 6));
    if (IS_OBJ(args[0])) {
        switch (AS_OBJ(args[0])->type) {
            case OBJ_STRING:   return OBJ_VAL(copyString("String", 6));
            case OBJ_FUNCTION: return OBJ_VAL(copyString("Function", 8));
            case OBJ_CLOSURE:  return OBJ_VAL(copyString("Closure", 7));
            case OBJ_NATIVE:   return OBJ_VAL(copyString("NativeFn", 8));
            default:           return OBJ_VAL(copyString("Object", 6));
        }
    }
    return NIL_VAL;
}

// ==========================================
// MASTER INITIALIZATION
// ==========================================

void initWaliaCore() {
    // Math
    defineNative("sqrt", mathSqrt);
    defineNative("abs",  mathAbs);
    defineNative("pow",  mathPow);

    // String
    defineNative("str_sub",  stringSub);
    defineNative("str_len",  stringLen);
    defineNative("str_find", stringFind);

    // Path
    defineNative("path_exists", pathExists);
    defineNative("path_join",   pathJoin);

    // System
    defineNative("type_of", waliaTypeOf);
}
