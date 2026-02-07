#ifndef WALIA_SYS_SECURITY_H
#define WALIA_SYS_SECURITY_H

#include "walia_kernel_base.h"

// ==========================================
// BIT-LEVEL IDENTITY DEFINITIONS
// ==========================================

typedef enum {
    BIT_TYPE_DATA,      // Non-executable payloads (Numbers, Tensors)
    BIT_TYPE_LOGIC,     // Standard bytecode
    BIT_TYPE_SOVEREIGN, // Kernel-level instructions (Highest Role)
    BIT_TYPE_FOREIGNER  // Unauthorized/Untrusted bit patterns
} WldBitRole;

/**
 * @brief Represents the cryptographic footprint of a logic block.
 * Uses the SHA-256 fingerprint from Phase 10.1.
 */
typedef struct {
    uint8_t  fingerprint[32]; // SHA-256 Logic Identity
    uint32_t authorizedRole;  // RBAC Role Level (Admin, User, Driver)
    uint32_t entryPoint;      // Physical offset of the first instruction
} WldRoleFootprint;

// ==========================================
// SOVEREIGN GATE API
// ==========================================

/**
 * @brief The One-Way Entry Gate.
 * Performs a bit-audit on a physical memory page before it is mapped.
 * 
 * Logic:
 * 1. Scans the 4KB page for forbidden instruction patterns.
 * 2. Validates the SHA-256 hash against the Sovereign Registry.
 * 3. Returns the detected BitRole.
 */
WldBitRole sys_security_audit_page(const void* buffer, WldRoleFootprint* expected);

/**
 * @brief Physically isolates a memory segment identified as 'Foreigner'.
 */
void sys_security_isolate_segment(uintptr_t physicalAddr);

#endif // WALIA_SYS_SECURITY_H
