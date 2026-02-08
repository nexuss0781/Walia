#ifndef WALIA_TOOLING_ORACLE_GEN_H
#define WALIA_TOOLING_ORACLE_GEN_H

#include "../core/common.h"
#include "../tooling/tooling_oracle_lexer.h"

// ==========================================
// MANUAL EXPORT CONFIGURATION
// ==========================================

#define WALIA_MANUAL_OUTPUT "Documentation/sovereign_manual.md"

// ==========================================
// ORACLE GENERATOR API
// ==========================================

/**
 * @brief Exports all verified logic blocks into the Enterprise Manual.
 * 
 * Logic:
 * 1. Opens the target Markdown file.
 * 2. Writes the "Sovereign Certificate of Truth" header.
 * 3. Iterates through all snippets verified by the Runner (Phase 10.4.2).
 * 4. Injects Red/Green status badges and Logic Hashes.
 */
void walia_oracle_generate_manual();

/**
 * @brief Helper to format a SHA-256 hash into a Markdown-friendly string.
 */
void walia_oracle_format_hash(uint8_t* hash, char* outStr);

#endif // WALIA_TOOLING_ORACLE_GEN_H
