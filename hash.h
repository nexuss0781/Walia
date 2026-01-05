#ifndef WALIA_HASH_H
#define WALIA_HASH_H

#include "common.h"

// ==========================================
// SHA-256 CONFIGURATION
// ==========================================
// We use a fixed-size hash (32 bytes / 256 bits) as the 
// mathematical identity for Walia functions and modules.

#define WALIA_HASH_SIZE 32

typedef struct {
    uint8_t data[64];
    uint32_t datalen;
    uint64_t bitlen;
    uint32_t state[8];
} WaliaHashContext;

// ==========================================
// CRYPTOGRAPHIC API
// ==========================================

/**
 * @brief Initializes a hashing context for a new logic block.
 */
void walia_hash_init(WaliaHashContext* ctx);

/**
 * @brief Updates the hash with a stream of data (e.g., instructions).
 */
void walia_hash_update(WaliaHashContext* ctx, const uint8_t* data, size_t len);

/**
 * @brief Finalizes the hash calculation.
 * @param hash A buffer of at least WALIA_HASH_SIZE bytes.
 */
void walia_hash_final(WaliaHashContext* ctx, uint8_t* hash);

/**
 * @brief Helper to generate a hex string representation of a hash.
 * Used for version identifiers and diagnostics.
 */
void walia_hash_to_string(const uint8_t* hash, char* outStr);

#endif // WALIA_HASH_H
