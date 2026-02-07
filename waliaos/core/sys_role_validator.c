#include "../include/sys_security.h"
#include "../include/walia_kernel_base.h"
#include "../include/driver_serial.h"
#include "core/object.h"
#include "core/compiler.h"

// ==========================================
// SOVEREIGN LOGIC AUDIT
// ==========================================

/**
 * @brief Validates a function block before execution.
 * 
 * UFO-GRADE SECURITY:
 * This is the 'Role Check Footprint'. We verify the SHA-256 
 * of the bytecode instructions *while they are in the persistent heap*.
 */
bool sys_security_validate_logic(ObjFunction* func, uint32_t requiredRole) {
    k_serial_print(">> Walia Logic Sentry: Auditing node '");
    if (func->name) k_serial_print(func->name->chars);
    k_serial_print("'...\n");

    // 1. ASSEMBLE FOOTPRINT
    // In Walia OS, the footprint is stored as metadata in the ObjFunction 
    // (See Phase 18 Object.h update).
    WldRoleFootprint footprint;
    
    // We retrieve the 'Immutable Identity' from the function's 
    // content-addressable hash (Phase 10.1).
    extern void sys_get_logic_identity(ObjFunction*, uint8_t*);
    sys_get_logic_identity(func, footprint.fingerprint);
    
    footprint.authorizedRole = func->arity; // Simplified role mapping for Phase 23
    footprint.entryPoint = (uint32_t)(uintptr_t)func->chunk.code;

    // 2. TRIGGER ATOMIC BIT-AUDIT
    // We call the Physical Filter (Phase 23.1 Group 1) to verify 
    // that the bits in RAM currently match the recorded identity.
    WldBitRole role = sys_security_audit_page(func->chunk.code, &footprint);

    // 3. ENFORCE ROLE BOUNDARY
    if (role == BIT_TYPE_FOREIGNER) {
        k_serial_print("[CRITICAL] Logic Hijack Detected! SHA-256 Mismatch.\n");
        sys_security_isolate_segment((uintptr_t)func->chunk.code);
        return false;
    }

    if (role == BIT_TYPE_DATA) {
        k_serial_print("[FAULT] Attempted execution of non-logic bits.\n");
        return false;
    }

    // 4. SECURITY HANDOVER
    // If the function is a 'Sovereign' type (Kernel logic), 
    // it is permitted to use the raw ASM and Syscall opcodes.
    if (role == BIT_TYPE_SOVEREIGN && requiredRole <= 0xFF) {
        k_serial_print(">> Status: SOVEREIGN_IDENTITY_VERIFIED. Execution Permitted.\n");
        return true;
    }

    // Standard User Logic check
    if (role == BIT_TYPE_LOGIC && requiredRole <= 0x01) {
        return true;
    }

    k_serial_print("[FAULT] Insufficient Role Authorization for logic node.\n");
    return false;
}

// ==========================================
// JIT ESCALATION SENTRY
// ==========================================

/**
 * @brief Hook for the Tier-2 Native Compiler.
 * Ensures that thunks are only generated for verified logic.
 */
bool sys_security_can_escalate(ObjFunction* func) {
    // A function must pass the Logic Sentry audit before it is 
    // allowed to be turned into raw x86_64/ARM64 machine code.
    return sys_security_validate_logic(func, 0x01);
}
