#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"

// Helper to allocate the object header.
#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

// Helper for flexible array struct allocation (String + chars)
#define ALLOCATE_OBJ_VAR(type, extraSize, objectType) \
    (type*)allocateObject(sizeof(type) + extraSize, objectType)

static Obj* allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0, size);
    object->type = type;
    
    // GC Hook: Add to the front of the global linked list of objects.
    object->next = vm.objects;
    vm.objects = object;
    
    return object;
}

ObjClass* newClass(ObjString* name) {
    ObjClass* klass = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
    klass->name = name;
    initTable(&klass->methods);
    return klass;
}

ObjInstance* newInstance(ObjClass* klass) {
    ObjInstance* instance = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
    instance->klass = klass;
    initTable(&instance->fields);
    return instance;
}

ObjBoundMethod* newBoundMethod(Value receiver, ObjClosure* method) {
    ObjBoundMethod* bound = ALLOCATE_OBJ(ObjBoundMethod, OBJ_BOUND_METHOD);
    bound->receiver = receiver;
    bound->method = method;
    return bound;
}

// ==========================================
// HASHING ALGORITHM (FNV-1a)
// ==========================================
static uint32_t hashString(const char* key, int length) {
    uint32_t hash = 2166136261u; // FNV Offset Basis
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;        // FNV Prime
    }
    return hash;
}

// ==========================================
// STRING MANAGEMENT
// ==========================================

static ObjString* allocateString(char* chars, int length, uint32_t hash) {
    // 1. ALLOCATE memory for the struct + the characters in one block.
    ObjString* string = ALLOCATE_OBJ_VAR(ObjString, length + 1, OBJ_STRING);
    
    string->length = length;
    string->hash = hash;
    
    // Copy the characters into the flexible array slot
    memcpy(string->chars, chars, length);
    string->chars[length] = '\0'; // Null terminate for safety

    // 2. INTERN the string immediately.
    // We add it to the VM's global string table.
    tableSet(&vm.strings, string, NIL_VAL);

    return string;
}

ObjString* copyString(const char* chars, int length) {
    uint32_t hash = hashString(chars, length);

    // DEDUPLICATION: Check if we already have this string.
    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL) return interned;

    // If not, allocate new memory.
    // Note: allocateString handles the actual copying into the struct.
    return allocateString((char*)chars, length, hash);
}

ObjString* takeString(char* chars, int length) {
    uint32_t hash = hashString(chars, length);
    
    // DEDUPLICATION: Check if we already have this string.
    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL) {
        FREE_ARRAY(char, chars, length + 1); // Free the duplicate input
        return interned;
    }

    return allocateString(chars, length, hash);
}

// ==========================================
// FUNCTION CONSTRUCTORS
// ==========================================

ObjFunction* newFunction() {
    ObjFunction* function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->upvalueCount = 0;
    function->name = NULL;
    function->nativeThunk = NULL;
    function->callCount = 0;
    function->isAsync = false;
    memset(function->fingerprint, 0, 32);
    initChunk(&function->chunk);
    return function;
}

ObjNative* newNative(NativeFn function) {
    ObjNative* native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
    native->function = function;
    return native;
}

ObjClosure* newClosure(ObjFunction* function) {
    ObjUpvalue** upvalues = ALLOCATE(ObjUpvalue*, function->upvalueCount);
    for (int i = 0; i < function->upvalueCount; i++) {
        upvalues[i] = NULL;
    }

    ObjClosure* closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
    closure->function = function;
    closure->upvalues = upvalues;
    closure->upvalueCount = function->upvalueCount;
    return closure;
}

ObjUpvalue* newUpvalue(Value* slot) {
    ObjUpvalue* upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
    upvalue->location = slot;
    upvalue->closed = NIL_VAL;
    upvalue->next = NULL;
    return upvalue;
}

// ==========================================
// DEBUG PRINTING
// ==========================================

void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
        case OBJ_FUNCTION:
            printf("<fn %s>", AS_FUNCTION(value)->name ? AS_FUNCTION(value)->name->chars : "script");
            break;
        case OBJ_NATIVE:
            printf("<native fn>");
            break;
        case OBJ_CLOSURE:
            printf("<closure %s>", AS_CLOSURE(value)->function->name ? AS_CLOSURE(value)->function->name->chars : "script");
            break;
        case OBJ_UPVALUE:
            printf("upvalue");
            break;
        case OBJ_CLASS:
            printf("<class %s>", AS_CLASS(value)->name->chars);
            break;
        case OBJ_INSTANCE:
            printf("<instance of %s>", AS_INSTANCE(value)->klass->name->chars);
            break;
        case OBJ_BOUND_METHOD:
            printf("<method %s>", AS_BOUND_METHOD(value)->method->function->name->chars);
            break;
        case OBJ_LIST:
            printf("<list>");
            break;
        case OBJ_MAP:
            printf("<map>");
            break;
        case OBJ_FUTURE:
            printf("<future>");
            break;
        case OBJ_STREAM:
            printf("<stream>");
            break;
        case OBJ_CONTINUATION:
            printf("<continuation>");
            break;
        case OBJ_VECTOR:
            printf("<vector>");
            break;
        case OBJ_CODE_BLOB:
            printf("<code blob>");
            break;
        case OBJ_WAIT_GROUP:
            printf("<waitgroup>");
            break;
        case OBJ_GENE:
            printf("<gene %s>", AS_CLASS(value)->name->chars); 
            break;
        case OBJ_GENOME:
            printf("<individual>");
            break;
    }
}
