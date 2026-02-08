#include <stdio.h>
#include <string.h>

#include "../vm/vm_magic_names.h"
#include "../core/memory.h"
#include "../core/object.h"
#include "../core/vm.h"

ObjString* vm_magic_strings[MAGIC_SENTINEL];

static const char* raw_names[] = {
    "init",
    "__add__", "__sub__", "__mul__", "__div__", "__mod__",
    "__neg__", "__not__",
    "__eq__", "__lt__", "__gt__",
    "__get__", "__set__", "__call__", "__str__",
    "__iter__", "__next__"
};

void vm_init_magic_strings() {
    for (int i = 0; i < MAGIC_SENTINEL; i++) {
        vm_magic_strings[i] = copyString(raw_names[i], (int)strlen(raw_names[i]));
    }
    
    printf(">> Walia VM: Meta-Object Protocol initialized [%d Magic Methods]\n", MAGIC_SENTINEL);
}
