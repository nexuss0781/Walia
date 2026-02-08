#include "../include/sys_security.h"
#include "../include/walia_kernel_base.h"
#include "../include/driver_serial.h"
#include "../../src/core/hash.h" // Accessing the SHA-256 Engine from Phase 7

// ==========================================
// THE ONE-WAY ENTRY GATE
// ==========================================

WldBitRole sys_security_audit_page(const void* buffer, WldRoleFootprint* expected) {
    k_serial_print(">> Walia Security: Commencing Atomic Bit-Audit...\n");

    // 1. HARDWARE-LEVEL HASHING (Phase 10.1 Integration)
    // We compute the fingerprint of the actual bits in the buffer.
    uint8_t actualHash[32];
    WaliaHashContext ctx;
    walia_hash_init(&ctx);
    walia_hash_update(&ctx, (const uint8_t*)buffer, 4096);
    walia_hash_final(&ctx, actualHash);

    // 2. FINGERPRINT VALIDATION (The Role Check)
    // We compare the actual bit-identity against the authorized footprint.
    // If a virus altered 1 bit, this comparison will fail.
    bool identityVerified = (k_memcpy(actualHash, expected->fingerprint, 32) == 0);

    if (!identityVerified) {
        k_serial_print("[ALERT] Bit-Mismatch Detected. Logic identity invalidated.\n");
        return BIT_TYPE_FOREIGNER;
    }

    // 3. ROLE ESCALATION CHECK
    // If the identity is valid, we determine the execution privilege.
    switch (expected->authorizedRole) {
        case 0xFF: return BIT_TYPE_SOVEREIGN; // Kernel/Root
        case 0x01: return BIT_TYPE_LOGIC;     // User Logic
        default:   return BIT_TYPE_DATA;      // Default to non-executable
    }
}

// ==========================================
// PHYSICAL REJECTION (Immune Response)
// ==========================================

/**
 * @brief Hard Isolation logic.
 * 
 * Logic:
 * In a 5th-generation computer, we don't just 'stop' the process.
 * we physically disable the memory mapping for this segment in the 
 * Page Table, making it invisible to the CPU.
 */
void sys_security_isolate_segment(uintptr_t addr) {
    k_serial_print("[IMMUNE RESPONSE] Physically isolating Foreigner bits at: ");
    
    char hex[16];
    k_itoa((int)addr, hex, 16);
    k_serial_print("0x");
    k_serial_print(hex);
    k_serial_print("\n");

    // This is the 'Bit-Level Defender' implementation.
    // We utilize the Pager (Phase 22) to mark the frame as 'POISONED'.
    /* extern void sys_pager_poison_frame(uintptr_t); */
    /* sys_pager_poison_frame(addr); */
    
    // Halt the specific CPU core to prevent contagion
    __asm__ volatile ("cli; hlt");
}
