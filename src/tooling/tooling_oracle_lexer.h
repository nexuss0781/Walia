#ifndef WALIA_TOOLING_ORACLE_LEXER_H
#define WALIA_TOOLING_ORACLE_LEXER_H

#include "../core/common.h"
#include "../tooling/tooling_hash_scanner.h"

// ==========================================
// ORACLE CONTRACT DEFINITION
// ==========================================

#define MAX_SNIPPET_SIZE 4096

/**
 * @brief Represents an executable documentation contract.
 */
typedef struct {
    char targetSymbol[256];  // Parent function or class name
    char description[256];   // The text following '/// example:'
    char body[MAX_SNIPPET_SIZE]; // The executable Walia code
    int  lineStart;
    int  lineEnd;
    uint8_t logicHash[32];   // SHA-256 of the logic being documented
} WldDocSnippet;

// ==========================================
// ORACLE LEXER API
// ==========================================

/**
 * @brief Initializes the Oracle Registry.
 */
void walia_oracle_init();

/**
 * @brief Scans a file specifically for documentation contracts.
 * 
 * Logic:
 * 1. Identify lines starting with '///'.
 * 2. If '///' is followed by a code-indent, capture it as an example.
 * 3. Bind the captured snippet to the NEXT detected 'fun' or 'class'.
 * 4. Log a fault if an example is found with no logical parent.
 */
void walia_oracle_scan_file(const char* path);

/**
 * @brief Retrieves all extracted snippets for the Runner (Phase 10.4.2).
 */
WldDocSnippet* walia_oracle_get_snippets(int* count);

#endif // WALIA_TOOLING_ORACLE_LEXER_H
