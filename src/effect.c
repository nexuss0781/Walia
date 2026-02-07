#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "effect.h"
#include "core/memory.h"
#include "core/object.h"
#include "core/value.h"
#include "core/vm.h"
#include "core/gc.h" // Fixed: Added for markValue and markObject definitions

// ==========================================
// CONTINUATION LIFECYCLE
// ==========================================

/**
 * @brief Internal allocator for the resumable state.
 */
static ObjContinuation* allocateContinuation() {
    ObjContinuation* continuation = (ObjContinuation*)reallocate(NULL, 0, sizeof(ObjContinuation));
    
    // Initialize Sovereign Object Header
    continuation->obj.type = OBJ_CONTINUATION; 
    continuation->obj.isMarked = false;
    continuation->obj.next = vm.objects;
    vm.objects = (Obj*)continuation;

    continuation->frameCount = 0;
    continuation->capturedFrames = NULL;
    continuation->stackCount = 0;
    continuation->capturedStack = NULL;
    continuation->returnToReg = 0;

    return continuation;
}
// ==========================================
// STACK SLICING (Capture Logic)
// ==========================================

ObjContinuation* captureContinuation(EffectHandler* handler, int returnToReg) {
    ObjContinuation* cont = allocateContinuation();
    cont->returnToReg = returnToReg;

    // 1. Calculate the Physical Boundaries of the Slice
    int startFrameIndex = (handler != NULL) ? (handler->frameIndex + 1) : 0;
    int endFrameIndex = vm.frameCount;
    cont->frameCount = endFrameIndex - startFrameIndex;

    // 2. Snapshot the CallFrames (Metadata)
    cont->capturedFrames = ALLOCATE(CallFrame, cont->frameCount);
    for (int i = 0; i < cont->frameCount; i++) {
        cont->capturedFrames[i] = vm.frames[startFrameIndex + i];

        // POINTER SWIZZLING:
        // Convert absolute stack addresses into relative offsets to ensure
        // the continuation is persistent and relocatable.
        if (handler != NULL) {
            size_t offset = (Value*)vm.frames[startFrameIndex + i].slots - handler->stackBase;
            cont->capturedFrames[i].slots = (Value*)offset;
        }
    }

    // 3. Snapshot the Register Values (Data)
    Value* base = (handler != NULL) ? handler->stackBase : vm.stack;
    cont->stackCount = (int)(vm.stackTop - base);
    
    // Allocate persistent storage for the registers
    cont->capturedStack = ALLOCATE(Value, cont->stackCount);
    memcpy(cont->capturedStack, base, sizeof(Value) * cont->stackCount);

    markCard(cont);
    return cont;
}

// ==========================================
// STACK SPLICING (Resumption Logic)
// ==========================================

void resumeContinuation(ObjContinuation* cont, Value result) {
    // 1. Verify Stack Capacity
    if (vm.frameCount + cont->frameCount >= FRAMES_MAX) {
        fprintf(stderr, "Walia Runtime Error: Stack overflow during effect resumption.\n");
        return;
    }

    // 2. Splice point
    Value* splicePoint = vm.stackTop;

    // 3. Restore Stack segments
    memcpy(vm.stackTop, cont->capturedStack, sizeof(Value) * cont->stackCount);
    vm.stackTop += cont->stackCount;

    // 4. Restore CallFrames and Swizzle pointers
    for (int i = 0; i < cont->frameCount; i++) {
        CallFrame* frame = &vm.frames[vm.frameCount++];
        *frame = cont->capturedFrames[i];
        
        // Convert relative offsets back to absolute VM stack addresses
        size_t offset = (size_t)frame->slots;
        frame->slots = splicePoint + offset;
    }

    // 5. Inject Result into the 'perform' register
    CallFrame* topFrame = &vm.frames[vm.frameCount - 1];
    topFrame->slots[cont->returnToReg] = result;
}

// ==========================================
// GC & MEMORY INTEGRITY
// ==========================================

void markContinuation(ObjContinuation* cont) {
    // Tracing the captured stack for GC reachability
    for (int i = 0; i < cont->stackCount; i++) {
        markValue(cont->capturedStack[i]);
    }

    // Tracing closures within the captured frames
    for (int i = 0; i < cont->frameCount; i++) {
        markObject((Obj*)cont->capturedFrames[i].closure);
    }
}

void freeContinuation(ObjContinuation* cont) {
    if (cont->capturedFrames != NULL) {
        FREE_ARRAY(CallFrame, cont->capturedFrames, cont->frameCount);
    }
    if (cont->capturedStack != NULL) {
        FREE_ARRAY(Value, cont->capturedStack, cont->stackCount);
    }
}

// ==========================================
// EFFECT INVOCATION
// ==========================================

Value perform_effect(const char* effectName) {
    // 1. PHYSICAL IDENTITY RESOLUTION
    // Intern the string to ensure O(1) pointer equality for the search
    ObjString* name = copyString(effectName, (int)strlen(effectName));
    
    // 2. SOVEREIGN SEARCH
    // We walk the typed handler stack from the most recent to the root.
    EffectHandler* foundHandler = NULL;
    int handlerIndex = -1;

    for (int i = vm.handlerCount - 1; i >= 0; i--) {
        // Direct member access now physically possible via typed VM struct
        if (vm.handlerStack[i].effectName == name) {
            foundHandler = &vm.handlerStack[i];
            handlerIndex = i;
            break;
        }
    }

    // 3. FAULT PROTECTION
    if (foundHandler == NULL) {
        // Halt if the effect is not caught (Truth-or-Death Enforcement)
        extern void runtimeError(const char* format, ...);
        runtimeError("Sovereign Violation: Unhandled Effect '%s'.", effectName);
        return NIL_VAL;
    }

    // 4. LOGIC CAPTURE (Stack Slicing)
    // Capture every frame and register between the current point and the handler.
    ObjContinuation* continuation = captureContinuation(foundHandler, 0);

    // 5. ATOMIC STATE TRANSITION
    // Reset the VM's frame and stack context to the handler's defined home.
    vm.frameCount = foundHandler->frameIndex + 1;
    CallFrame* handlerFrame = &vm.frames[vm.frameCount - 1];
    handlerFrame->ip = foundHandler->resumeIp;

    // Consume the handler stack up to the reaped level
    vm.handlerCount = handlerIndex;

    // 6. SILICON HANDOVER
    // Return the continuation as the result of the handler's entry.
    return OBJ_VAL(continuation);
}
