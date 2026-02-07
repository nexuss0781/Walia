#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "vm.h"
#include "memory.h"
#include "opcode.h"
#include "compiler.h"
#include "object.h"
#include "table.h"
#include "persistence.h"
#include "sys_watchdog.h"
#include "effect.h"
#include "hash.h"
#include "vm/vm_operators.h"
#include "vm/vm_magic_names.h"
#include "vm/vm_entangle.h"
#include "vm/vm_ripple.h"
#include "vm/vm_traps.h"
#include "vm/vm_native_compiler.h"
#include "lib/lib_stream.h"
#include "lib/lib_vector.h"
#include "db/db_vector_match.h"
#include "vm/vm_gene.h"
#include "lib/lib_evolution.h"
#include "vm/vm_sys_ops.h"       
#include "sys/sys_kernel.h"      
#include "sys/sys_allocator.h"   
#include "vm/vm_pager_link.h"   
#include "db/db_common.h"       
#include "db/db_shadow.h"       
#include "vm/vm_ptr_ops.h"

// The Global Walia Instance
VM vm;

// ==========================================
// FORWARD DECLARATIONS
// ==========================================
static ObjUpvalue* captureUpvalue(Value* local);
static void closeUpvalues(Value* last);
static void resetStack(void);
void runtimeError(const char* format, ...);  // Non-static for external use
static bool invokeFromClass(ObjClass* klass, ObjString* name, int argCount, int destReg);

static bool valuesEqual(Value a, Value b) {
#ifdef NAN_BOXING
    if (IS_NUMBER(a) && IS_NUMBER(b)) return AS_NUMBER(a) == AS_NUMBER(b);
    return a == b;
#else
    if (a.type != b.type) return false;
    switch (a.type) {
        case VAL_BOOL:   return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NIL:    return true;
        case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_OBJ:    return AS_OBJ(a) == AS_OBJ(b);
        default:         return false;
    }
#endif
}

void runtimeError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "\033[0;31m[RUNTIME ERROR]\033[0m ");
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);
    for (int i = vm.frameCount - 1; i >= 0; i--) {
        CallFrame* frame = &vm.frames[i];
        ObjFunction* function = frame->closure->function;
        size_t instruction = frame->ip - function->chunk.code - 1;
        fprintf(stderr, "  [line %d] in ", getLine(&function->chunk, (int)instruction));
        if (function->name == NULL) fprintf(stderr, "script\n");
        else fprintf(stderr, "%s()\n", function->name->chars);
    }
    resetStack();
}

static WldTransaction* currentActiveTx = NULL;
WldTransaction* vm_get_active_transaction(void) { return currentActiveTx; }

static bool invokeFromClass(ObjClass* klass, ObjString* name, int argCount, int destReg) {
    Value method;
    // 1. Search the flattened method table (Phase 14 / 15.4)
    if (!tableGet(&klass->methods, name, &method)) {
        runtimeError("Undefined property '%s' in class %s.", name->chars, klass->name->chars);
        return false;
    }

    // 2. Stack Boundary Verification
    if (vm.frameCount == FRAMES_MAX) {
        runtimeError("Stack overflow: Maximum call depth reached.");
        return false;
    }

    // 3. Physical Frame Handover
    ObjClosure* closure = AS_CLOSURE(method);
    CallFrame* frame = &vm.frames[vm.frameCount++];
    frame->closure = closure;
    frame->ip = closure->function->chunk.code;
    
    // The arguments are expected to be contiguous in the register file.
    // 'slots' points to the start of the argument block for the new frame.
    frame->slots = &vm.stack[vm.stackTop - vm.stack - argCount - 1];
    frame->returnToReg = destReg;

    return true;
}

static void resetStack() {
    vm.stackTop = vm.stack; vm.frameCount = 0; vm.handlerCount = 0; vm.openUpvalues = NULL; vm.traceIndex = 0;
}

void initVM() {
    resetStack();
    vm.objects = NULL; vm.bytesAllocated = 0; vm.nextGC = 1024 * 1024;
    vm.grayCount = 0; vm.grayCapacity = 0; vm.grayStack = NULL;
    initTable(&vm.strings); initTable(&vm.globals); initTable(&vm.methods);
    vm.traceBuffer = ALLOCATE(TraceEntry, TRACE_BUFFER_SIZE);
    vm.isTraceActive = true;
    vm.handlerStack = malloc(sizeof(EffectHandler) * HANDLER_STACK_MAX);
    walia_boot();
    vm_init_magic_strings();
}

void freeVM() {
    walia_shutdown();
    freeTable(&vm.strings); freeTable(&vm.globals); freeTable(&vm.methods);
    FREE_ARRAY(TraceEntry, vm.traceBuffer, TRACE_BUFFER_SIZE);
    if (vm.handlerStack) free(vm.handlerStack);
    if (vm.grayStack) free(vm.grayStack);
}

void push(Value value) {
    if (vm.stackTop - vm.stack >= STACK_MAX) { fprintf(stderr, "Fatal: Stack Overflow.\n"); exit(1); }
    *vm.stackTop++ = value;
}

Value pop() { return *(--vm.stackTop); }

static inline void logTrace(Instruction* ip, int regIndex, Value previousValue) {
    if (!vm.isTraceActive) return;
    TraceEntry* entry = &vm.traceBuffer[vm.traceIndex];
    entry->ip = ip; entry->regIndex = regIndex; entry->previousValue = previousValue;
    vm.traceIndex = (vm.traceIndex + 1) % TRACE_BUFFER_SIZE;
}

static ObjUpvalue* captureUpvalue(Value* local) {
    ObjUpvalue* prev = NULL; ObjUpvalue* uv = vm.openUpvalues;
    while (uv != NULL && uv->location > local) { prev = uv; uv = uv->next; }
    if (uv != NULL && uv->location == local) return uv;
    ObjUpvalue* created = newUpvalue(local);
    created->next = uv;
    if (prev == NULL) vm.openUpvalues = created; else prev->next = created;
    return created;
}

static void closeUpvalues(Value* last) {
    while (vm.openUpvalues != NULL && vm.openUpvalues->location >= last) {
        ObjUpvalue* uv = vm.openUpvalues;
        uv->closed = *uv->location; uv->location = &uv->closed;
        markCard(uv); vm.openUpvalues = uv->next;
    }
}

static bool bindMethod(ObjClass* klass, ObjString* name, int destReg) {
    Value method; if (!tableGet(&klass->methods, name, &method)) return false;
    vm.stack[destReg] = OBJ_VAL(newBoundMethod(vm.stack[destReg], AS_CLOSURE(method)));
    return true;
}

static bool invoke(ObjString* name, int argCount, int objReg, int destReg) {
    Value receiver = vm.stack[objReg];
    if (!IS_INSTANCE(receiver)) { runtimeError("Only instances have methods."); return false; }
    ObjInstance* instance = AS_INSTANCE(receiver);
    Value value;
    if (tableGet(&instance->fields, name, &value)) {
        vm.stack[objReg] = value; return true; 
    }
    return invokeFromClass(instance->klass, name, argCount, destReg);
}

static InterpretResult run() {
    CallFrame* frame = &vm.frames[vm.frameCount - 1];
    EffectHandler* handlers = (EffectHandler*)vm.handlerStack;

    // Get Core ID for Watchdog Pulsing
    // In single-core genesis, this is 0. In SMP, it's read from APIC.
    int core_id = 0; 

    #define READ_INST()     (*frame->ip++)
    #define R(idx)          (frame->slots[idx])
    #define TRIGGER_RIPPLE(reg) if (walia_is_entangled(reg)) { walia_ripple_trigger(reg); }

    // PHASE 30: IRON DOME LIVENESS CHECK
    // Updates the shared heartbeat timestamp.
    #define PET_WATCHDOG() sys_watchdog_pulse(core_id)

    #ifdef __GNUC__
        #define DISPATCH() goto *dispatch_table[GET_OP(*frame->ip)]
        #define CASE(op)   op##_lbl: 
        static void* dispatch_table[] = {
            &&OP_LOAD_CONST_lbl, &&OP_LOAD_BOOL_lbl, &&OP_LOAD_NIL_lbl,
            &&OP_MOVE_lbl, &&OP_ADD_lbl, &&OP_SUB_lbl, &&OP_MUL_lbl,
            &&OP_DIV_lbl, &&OP_NOT_lbl, &&OP_NEGATE_lbl, &&OP_EQUAL_lbl,
            &&OP_GREATER_lbl, &&OP_LESS_lbl, &&OP_JUMP_lbl,
            &&OP_JUMP_IF_FALSE_lbl, &&OP_LOOP_lbl, &&OP_CALL_lbl,
            &&OP_CLOSURE_lbl, &&OP_GET_UPVALUE_lbl, &&OP_SET_UPVALUE_lbl,
            &&OP_RETURN_lbl, &&OP_PRINT_lbl, &&OP_DEFINE_GLOBAL_lbl,
            &&OP_GET_GLOBAL_lbl, &&OP_SET_GLOBAL_lbl, &&OP_HANDLE_lbl,
            &&OP_PERFORM_lbl, &&OP_RESUME_lbl,
            &&OP_CLASS_lbl, &&OP_INHERIT_lbl, &&OP_METHOD_lbl,
            &&OP_GET_PROPERTY_lbl, &&OP_SET_PROPERTY_lbl, &&OP_INVOKE_lbl,
            &&OP_ENTANGLE_lbl, &&OP_ITER_INIT_lbl, &&OP_ITER_NEXT_lbl,
            &&OP_JUMP_TABLE_lbl, &&OP_MATCH_SIMILAR_lbl,
            &&OP_BIT_AND_lbl, &&OP_BIT_OR_lbl, &&OP_BIT_XOR_lbl, &&OP_BIT_SHL_lbl, &&OP_BIT_SHR_lbl,
            &&OP_PTR_READ_lbl, &&OP_PTR_WRITE_lbl, &&OP_ADDRESS_OF_lbl,
            &&OP_GENE_lbl, &&OP_EVOLVE_lbl,
            &&OP_SYSCALL_lbl, &&OP_ASM_EXEC_lbl,
            &&OP_MALLOC_lbl, &&OP_MFREE_lbl, &&OP_SIZEOF_lbl,
            &&OP_DB_SIMD_FILTER_GT_lbl, &&OP_DB_LOAD_COL_lbl, &&OP_DB_COMMIT_lbl,
            &&OP_SYS_LOAD_U8_lbl,  &&OP_SYS_STORE_U8_lbl,
            &&OP_SYS_LOAD_U16_lbl, &&OP_SYS_STORE_U16_lbl,
            &&OP_SYS_LOAD_U32_lbl, &&OP_SYS_STORE_U32_lbl,
            &&OP_SYS_LOAD_U64_lbl, &&OP_SYS_STORE_U64_lbl,
            &&OP_SYS_CAST_lbl
        };
        DISPATCH();
    #else
        #define DISPATCH() goto loop
        #define CASE(op)   case op:
        loop: switch (GET_OP(*frame->ip))
    #endif

    {
        CASE(OP_LOAD_CONST) { Instruction inst = READ_INST(); R(GET_A(inst)) = frame->closure->function->chunk.constants.values[GET_BC(inst)]; DISPATCH(); }
        CASE(OP_LOAD_BOOL)  { Instruction inst = READ_INST(); R(GET_A(inst)) = BOOL_VAL(GET_B(inst)); DISPATCH(); }
        CASE(OP_LOAD_NIL)   { Instruction inst = READ_INST(); R(GET_A(inst)) = NIL_VAL; DISPATCH(); }
        CASE(OP_MOVE)       { Instruction inst = READ_INST(); int a = GET_A(inst); R(a) = R(GET_B(inst)); TRIGGER_RIPPLE(a); DISPATCH(); }
        CASE(OP_ADD)        { Instruction inst = READ_INST(); int a = GET_A(inst); Value b = R(GET_B(inst)), c = R(GET_C(inst)); if (IS_NUMBER(b) && IS_NUMBER(c)) { R(a) = NUMBER_VAL(AS_NUMBER(b) + AS_NUMBER(c)); TRIGGER_RIPPLE(a); } else if (vm_try_binary_overload(b, c, MAGIC_ADD)) { frame = &vm.frames[vm.frameCount - 1]; } else { runtimeError("Operands must be numbers."); return INTERPRET_RUNTIME_ERROR; } DISPATCH(); }
        CASE(OP_SUB)        { Instruction inst = READ_INST(); int a = GET_A(inst); R(a) = NUMBER_VAL(AS_NUMBER(R(GET_B(inst))) - AS_NUMBER(R(GET_C(inst)))); TRIGGER_RIPPLE(a); DISPATCH(); }
        CASE(OP_MUL)        { Instruction inst = READ_INST(); int a = GET_A(inst); R(a) = NUMBER_VAL(AS_NUMBER(R(GET_B(inst))) * AS_NUMBER(R(GET_C(inst)))); TRIGGER_RIPPLE(a); DISPATCH(); }
        CASE(OP_DIV)        { Instruction inst = READ_INST(); int a = GET_A(inst); R(a) = NUMBER_VAL(AS_NUMBER(R(GET_B(inst))) / AS_NUMBER(R(GET_C(inst)))); TRIGGER_RIPPLE(a); DISPATCH(); }
        CASE(OP_BIT_AND)    { Instruction inst = READ_INST(); int a = GET_A(inst); R(a) = sys_vm_bit_op(R(GET_B(inst)), R(GET_C(inst)), OP_BIT_AND); TRIGGER_RIPPLE(a); DISPATCH(); }
        CASE(OP_BIT_OR)     { Instruction inst = READ_INST(); int a = GET_A(inst); R(a) = sys_vm_bit_op(R(GET_B(inst)), R(GET_C(inst)), OP_BIT_OR);  TRIGGER_RIPPLE(a); DISPATCH(); }
        CASE(OP_BIT_XOR)    { Instruction inst = READ_INST(); int a = GET_A(inst); R(a) = sys_vm_bit_op(R(GET_B(inst)), R(GET_C(inst)), OP_BIT_XOR); TRIGGER_RIPPLE(a); DISPATCH(); }
        CASE(OP_BIT_SHL)    { Instruction inst = READ_INST(); int a = GET_A(inst); R(a) = sys_vm_bit_op(R(GET_B(inst)), R(GET_C(inst)), OP_BIT_SHL); TRIGGER_RIPPLE(a); DISPATCH(); }
        CASE(OP_BIT_SHR)    { Instruction inst = READ_INST(); int a = GET_A(inst); R(a) = sys_vm_bit_op(R(GET_B(inst)), R(GET_C(inst)), OP_BIT_SHR); TRIGGER_RIPPLE(a); DISPATCH(); }
        CASE(OP_NOT)        { Instruction inst = READ_INST(); int a = GET_A(inst); R(a) = BOOL_VAL(IS_NIL(R(GET_B(inst))) || (IS_BOOL(R(GET_B(inst))) && !AS_BOOL(R(GET_B(inst))))); DISPATCH(); }
        CASE(OP_NEGATE)     { Instruction inst = READ_INST(); int a = GET_A(inst); R(a) = NUMBER_VAL(-AS_NUMBER(R(GET_B(inst)))); TRIGGER_RIPPLE(a); DISPATCH(); }
        CASE(OP_EQUAL)      { Instruction inst = READ_INST(); R(GET_A(inst)) = BOOL_VAL(valuesEqual(R(GET_B(inst)), R(GET_C(inst)))); DISPATCH(); }
        CASE(OP_GREATER)    { Instruction inst = READ_INST(); R(GET_A(inst)) = BOOL_VAL(AS_NUMBER(R(GET_B(inst))) > AS_NUMBER(R(GET_C(inst)))); DISPATCH(); }
        CASE(OP_LESS)       { Instruction inst = READ_INST(); R(GET_A(inst)) = BOOL_VAL(AS_NUMBER(R(GET_B(inst))) < AS_NUMBER(R(GET_C(inst)))); DISPATCH(); }
        CASE(OP_JUMP) {
            PET_WATCHDOG();
            Instruction jumpInst = READ_INST();
            frame->ip += GET_BC(jumpInst);
            DISPATCH();
        }
        CASE(OP_JUMP_IF_FALSE) {
            PET_WATCHDOG();
            Instruction inst = READ_INST();
            if (IS_NIL(R(GET_A(inst))) || (IS_BOOL(R(GET_A(inst))) && !AS_BOOL(R(GET_A(inst))))) {
                frame->ip += GET_BC(inst);
            }
            DISPATCH();
        }
        CASE(OP_LOOP) {
            PET_WATCHDOG();
            Instruction loopInst = READ_INST();
            frame->ip -= GET_BC(loopInst);
            DISPATCH();
        }
        CASE(OP_CALL)       { Instruction inst = READ_INST(); int dest = GET_A(inst); Value callee = R(GET_B(inst)); int argCount = GET_C(inst); if (IS_CLOSURE(callee)) { ObjFunction* func = AS_CLOSURE(callee)->function; func->callCount++; if (func->callCount > 1024 && func->nativeThunk == NULL) func->nativeThunk = walia_jit_tier2_compile(func); if (func->nativeThunk != NULL) { R(dest) = walia_jit_tier2_execute(func->nativeThunk, argCount, &R(GET_B(inst) + 1)); TRIGGER_RIPPLE(dest); DISPATCH(); } if (vm.frameCount == FRAMES_MAX) { runtimeError("Stack overflow."); return INTERPRET_RUNTIME_ERROR; } CallFrame* newFrame = &vm.frames[vm.frameCount++]; newFrame->closure = AS_CLOSURE(callee); newFrame->ip = newFrame->closure->function->chunk.code; newFrame->slots = &R(GET_B(inst) + 1); newFrame->returnToReg = dest; frame = newFrame; DISPATCH(); } if (IS_NATIVE(callee)) { R(dest) = AS_NATIVE(callee)(argCount, &R(GET_B(inst) + 1)); TRIGGER_RIPPLE(dest); DISPATCH(); } if (IS_CLASS(callee)) { R(dest) = OBJ_VAL(newInstance(AS_CLASS(callee))); DISPATCH(); } return INTERPRET_RUNTIME_ERROR; }
        CASE(OP_RETURN)     { Instruction inst = READ_INST(); Value result = R(GET_A(inst)); int destReg = frame->returnToReg; closeUpvalues(frame->slots); vm.frameCount--; if (vm.frameCount == 0) return INTERPRET_OK; frame = &vm.frames[vm.frameCount - 1]; R(destReg) = result; TRIGGER_RIPPLE(destReg); DISPATCH(); }
        CASE(OP_CLOSURE)    { Instruction inst = READ_INST(); int dest = GET_A(inst); ObjFunction* func = AS_FUNCTION(frame->closure->function->chunk.constants.values[GET_BC(inst)]); ObjClosure* closure = newClosure(func); R(dest) = OBJ_VAL(closure); for (int i = 0; i < closure->upvalueCount; i++) { Instruction upInst = READ_INST(); int isLocal = GET_A(upInst); int index = GET_BC(upInst); if (isLocal) closure->upvalues[i] = captureUpvalue(frame->slots + index); else closure->upvalues[i] = frame->closure->upvalues[index]; } DISPATCH(); }
        CASE(OP_GET_UPVALUE) { Instruction inst = READ_INST(); int a = GET_A(inst); R(a) = *frame->closure->upvalues[GET_BC(inst)]->location; TRIGGER_RIPPLE(a); DISPATCH(); }
        CASE(OP_SET_UPVALUE) { Instruction inst = READ_INST(); *frame->closure->upvalues[GET_BC(inst)]->location = R(GET_A(inst)); DISPATCH(); }
        CASE(OP_GET_PROPERTY) { Instruction inst = READ_INST(); int dest = GET_A(inst); Value obj = R(GET_B(inst)); ObjString* name = AS_STRING(frame->closure->function->chunk.constants.values[GET_C(inst)]); if (IS_OBJ(obj) && AS_OBJ(obj)->type == OBJ_NATIVE) { R(dest) = vm_trap_get_property(obj, name); TRIGGER_RIPPLE(dest); DISPATCH(); } ObjInstance* instance = AS_INSTANCE(obj); Value v; if (tableGet(&instance->fields, name, &v)) { R(dest) = v; TRIGGER_RIPPLE(dest); DISPATCH(); } if (bindMethod(instance->klass, name, dest)) DISPATCH(); return INTERPRET_RUNTIME_ERROR; }
        CASE(OP_SET_PROPERTY) { Instruction inst = READ_INST(); Value obj = R(GET_A(inst)), val = R(GET_B(inst)); ObjString* name = AS_STRING(frame->closure->function->chunk.constants.values[GET_C(inst)]); if (IS_OBJ(obj) && AS_OBJ(obj)->type == OBJ_NATIVE) { if (vm_trap_set_property(obj, name, val)) DISPATCH(); } tableSet(&AS_INSTANCE(obj)->fields, name, val); markCard(AS_OBJ(obj)); DISPATCH(); }
        CASE(OP_DEFINE_GLOBAL) { Instruction inst = READ_INST(); ObjString* name = AS_STRING(frame->closure->function->chunk.constants.values[GET_BC(inst)]); tableSet(&vm.globals, name, R(GET_A(inst))); DISPATCH(); }
        CASE(OP_GET_GLOBAL) { Instruction inst = READ_INST(); int a = GET_A(inst); ObjString* name = AS_STRING(frame->closure->function->chunk.constants.values[GET_BC(inst)]); Value v; tableGet(&vm.globals, name, &v); R(a) = v; TRIGGER_RIPPLE(a); DISPATCH(); }
        CASE(OP_SET_GLOBAL) { Instruction inst = READ_INST(); ObjString* name = AS_STRING(frame->closure->function->chunk.constants.values[GET_BC(inst)]); tableSet(&vm.globals, name, R(GET_A(inst))); markCard(&vm.globals); DISPATCH(); }
        CASE(OP_CLASS)      { Instruction inst = READ_INST(); R(GET_A(inst)) = OBJ_VAL(newClass(AS_STRING(frame->closure->function->chunk.constants.values[GET_BC(inst)]))); DISPATCH(); }
        CASE(OP_METHOD)     { Instruction inst = READ_INST(); tableSet(&AS_CLASS(R(GET_A(inst)))->methods, AS_STRING(frame->closure->function->chunk.constants.values[GET_BC(inst)]), R(GET_A(inst) + 1)); DISPATCH(); }
        CASE(OP_INHERIT)    { Instruction inst = READ_INST(); Value sup = R(GET_B(inst)); if (!IS_CLASS(sup)) { runtimeError("Superclass must be a class."); return INTERPRET_RUNTIME_ERROR; } tableAddAll(&AS_CLASS(sup)->methods, &AS_CLASS(R(GET_A(inst)))->methods); DISPATCH(); }
        CASE(OP_INVOKE)     { Instruction inst = READ_INST(); ObjString* name = AS_STRING(frame->closure->function->chunk.constants.values[GET_C(inst)]); if (!invoke(name, GET_C(inst), GET_B(inst), GET_A(inst))) return INTERPRET_RUNTIME_ERROR; frame = &vm.frames[vm.frameCount - 1]; DISPATCH(); }
        CASE(OP_ENTANGLE)   { Instruction inst = READ_INST(); int sink = GET_A(inst); ObjFunction* thunk = AS_FUNCTION(frame->closure->function->chunk.constants.values[GET_BC(inst)]); int sources[8]; for (int i = 0; i < 8; i++) sources[i] = i; walia_entangle_register(sink, thunk, sources, 2); DISPATCH(); }
        CASE(OP_ITER_INIT)  { Instruction inst = READ_INST(); R(GET_A(inst)) = OBJ_VAL(walia_stream_new(R(GET_B(inst)), 1)); DISPATCH(); }
        CASE(OP_ITER_NEXT)  { Instruction inst = READ_INST(); int valReg = GET_A(inst), streamReg = GET_B(inst); ObjStream* stream = (ObjStream*)AS_OBJ(R(streamReg)); Value v; if (walia_stream_next(stream, &v)) { R(valReg) = v; TRIGGER_RIPPLE(valReg); } else { frame->ip += GET_BC(inst); } DISPATCH(); }
        CASE(OP_JUMP_TABLE) { DISPATCH(); }
        CASE(OP_MATCH_SIMILAR) { Instruction inst = READ_INST(); int t = GET_A(inst); WldNeuralBranch* b = (WldNeuralBranch*)AS_CSTRING(frame->closure->function->chunk.constants.values[0]); int w = db_vector_match_best((ObjVector*)AS_OBJ(R(t)), b, GET_BC(inst)); if (w >= 0) frame->ip += b[w].jumpOffset; DISPATCH(); }
        CASE(OP_GENE)       { Instruction inst = READ_INST(); R(GET_A(inst)) = OBJ_VAL(walia_gene_new(AS_STRING(frame->closure->function->chunk.constants.values[GET_BC(inst)]))); DISPATCH(); }
        CASE(OP_EVOLVE)     { Instruction inst = READ_INST(); R(GET_A(inst)) = OBJ_VAL(walia_evolve_breed((ObjGenome*)AS_OBJ(R(GET_B(inst))), (ObjGenome*)AS_OBJ(R(GET_C(inst))), 0.01f)); DISPATCH(); }
        CASE(OP_PTR_READ)   { Instruction inst = READ_INST(); R(GET_A(inst)) = ptr_read((uintptr_t)AS_NUMBER(R(GET_B(inst)))); DISPATCH(); }
        CASE(OP_PTR_WRITE)  { Instruction inst = READ_INST(); ptr_write((uintptr_t)AS_NUMBER(R(GET_A(inst))), R(GET_B(inst))); DISPATCH(); }
        CASE(OP_ADDRESS_OF) { Instruction inst = READ_INST(); R(GET_A(inst)) = NUMBER_VAL((double)(uintptr_t)&R(GET_B(inst))); DISPATCH(); }
        CASE(OP_SYSCALL)    { Instruction inst = READ_INST(); R(GET_A(inst)) = sys_kernel_call((int)AS_NUMBER(R(GET_B(inst))), GET_C(inst), &R(GET_B(inst) + 1)); DISPATCH(); }
        CASE(OP_ASM_EXEC)   { Instruction inst = READ_INST(); sys_asm_execute((ObjCodeBlob*)AS_OBJ(frame->closure->function->chunk.constants.values[GET_BC(inst)])); DISPATCH(); }
        CASE(OP_MALLOC)     { Instruction inst = READ_INST(); R(GET_A(inst)) = NUMBER_VAL((double)(uintptr_t)sys_alloc((size_t)AS_NUMBER(R(GET_B(inst))))); DISPATCH(); }
        CASE(OP_MFREE)      { sys_release((void*)(uintptr_t)AS_NUMBER(R(GET_A(READ_INST())))); DISPATCH(); }
        CASE(OP_SIZEOF)     { Instruction inst = READ_INST(); R(GET_A(inst)) = NUMBER_VAL(8.0); DISPATCH(); }
        CASE(OP_HANDLE)     { Instruction inst = READ_INST(); EffectHandler* h = &handlers[vm.handlerCount++]; h->effectName = AS_STRING(frame->closure->function->chunk.constants.values[GET_A(inst)]); h->frameIndex = vm.frameCount - 1; h->stackBase = frame->slots; h->resumeIp = frame->ip + GET_BC(inst); DISPATCH(); }
        CASE(OP_PERFORM)    { Instruction inst = READ_INST(); ObjString* eff = AS_STRING(frame->closure->function->chunk.constants.values[GET_BC(inst)]); int d = GET_A(inst); EffectHandler* h = NULL; for (int i = vm.handlerCount - 1; i >= 0; i--) if (handlers[i].effectName == eff) { h = &handlers[i]; vm.handlerCount = i; break; } if (!h) return INTERPRET_RUNTIME_ERROR; R(0) = OBJ_VAL(captureContinuation(h, d)); vm.frameCount = h->frameIndex + 1; frame = &vm.frames[vm.frameCount - 1]; frame->ip = h->resumeIp; DISPATCH(); }
        CASE(OP_RESUME)     { Instruction inst = READ_INST(); resumeContinuation((ObjContinuation*)AS_OBJ(R(GET_A(inst))), R(GET_B(inst))); frame = &vm.frames[vm.frameCount - 1]; DISPATCH(); }
        CASE(OP_DB_SIMD_FILTER_GT) { DISPATCH(); } 
        CASE(OP_DB_LOAD_COL) { Instruction inst = READ_INST(); R(GET_A(inst)) = walia_fusion_get_property((ObjMappedRecord*)AS_OBJ(R(GET_B(inst))), AS_STRING(frame->closure->function->chunk.constants.values[GET_C(inst)])); DISPATCH(); }
        CASE(OP_DB_COMMIT)   { DISPATCH(); }
        CASE(OP_PRINT)       { printValue(R(GET_A(READ_INST()))); printf("\n"); DISPATCH(); }
        CASE(OP_SYS_LOAD_U8)  { DISPATCH(); }
        CASE(OP_SYS_STORE_U8) { Instruction inst = READ_INST(); sys_vm_write_u8(&R(GET_A(inst)), GET_C(inst), (uint8_t)AS_NUMBER(R(GET_B(inst)))); DISPATCH(); }
        CASE(OP_SYS_LOAD_U16) { DISPATCH(); }
        CASE(OP_SYS_STORE_U16){ DISPATCH(); }
        CASE(OP_SYS_LOAD_U32) { DISPATCH(); }
        CASE(OP_SYS_STORE_U32){ DISPATCH(); }
        CASE(OP_SYS_LOAD_U64) { DISPATCH(); }
        CASE(OP_SYS_STORE_U64){ Instruction inst = READ_INST(); R(GET_A(inst)) = R(GET_B(inst)); DISPATCH(); }
        CASE(OP_SYS_CAST)     { Instruction inst = READ_INST(); R(GET_A(inst)) = sys_vm_cast(R(GET_B(inst)), (WaliaSysType)GET_BC(inst)); DISPATCH(); }

        #ifndef __GNUC__
            default: return INTERPRET_RUNTIME_ERROR;
        #endif
    }
    return INTERPRET_OK;
}

InterpretResult interpret(ObjFunction* function) {
    ObjClosure* closure = newClosure(function);
    push(OBJ_VAL(closure));
    CallFrame* frame = &vm.frames[vm.frameCount++];
    frame->closure = closure; frame->ip = function->chunk.code;
    frame->slots = vm.stack; frame->returnToReg = 0;
    return run();
}
