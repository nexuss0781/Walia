#ifndef WALIA_VM_MAGIC_NAMES_H
#define WALIA_VM_MAGIC_NAMES_H

#include "core/object.h"

// ==========================================
// MAGIC METHOD REGISTRY
// ==========================================

typedef enum {
    MAGIC_INIT,      
    MAGIC_ADD,       
    MAGIC_SUB,       
    MAGIC_MUL,       
    MAGIC_DIV,       
    MAGIC_MOD,       
    MAGIC_NEG,       
    MAGIC_NOT,       
    
    MAGIC_EQ,        
    MAGIC_LT,        
    MAGIC_GT,        
    
    MAGIC_GET,       
    MAGIC_SET,       
    MAGIC_CALL,      
    MAGIC_STR,
    
    // Phase 13.2: Iterator Protocol
    MAGIC_ITER,      // "__iter__"
    MAGIC_NEXT,      // "__next__"
    
    MAGIC_SENTINEL   
} MagicMethod;

extern ObjString* vm_magic_strings[MAGIC_SENTINEL];

void vm_init_magic_strings();

#endif // WALIA_VM_MAGIC_NAMES_H
