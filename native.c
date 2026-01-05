#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "native.h"
#include "object.h"
#include "table.h"
#include "vm.h"

// clock() -> Returns current CPU time
static Value nativeClock(int argCount, Value* args) {
    (void)argCount; (void)args;
    return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

// print_native(val) -> System level print
static Value nativePrint(int argCount, Value* args) {
    if (argCount != 1) return NIL_VAL;
    printValue(args[0]);
    printf("\n");
    return NIL_VAL;
}

void defineNative(const char* name, NativeFn function) {
    ObjString* string = copyString(name, (int)strlen(name));
    push(OBJ_VAL(string));
    
    ObjNative* native = newNative(function);
    push(OBJ_VAL(native));
    
    tableSet(&vm.globals, string, OBJ_VAL(native));
    
    pop();
    pop();
}

void initNatives() {
    defineNative("clock", nativeClock);
    defineNative("print_native", nativePrint);
}
