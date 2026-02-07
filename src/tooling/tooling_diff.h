#ifndef WALIA_TOOLING_DIFF_H
#define WALIA_TOOLING_DIFF_H

#include "core/common.h"

// ==========================================
// VISUAL COLOR SCHEME
// ==========================================

#define DIFF_RED    "\033[0;31m"
#define DIFF_GREEN  "\033[0;32m"
#define DIFF_RESET  "\033[0m"
#define DIFF_BOLD   "\033[1m"

// ==========================================
// DIFF ENGINE API
// ==========================================

/**
 * @brief Computes and prints a line-by-line diff between two logic blocks.
 * 
 * Logic:
 * 1. Fetches old content from .walia/history via oldHash.
 * 2. Compares against the current 'newContent' string.
 * 3. Prints a unified diff to stdout with Red/Green highlighting.
 * 
 * @param symbolName Name of the function or class.
 * @param oldHash    SHA-256 fingerprint of the previous version.
 * @param newContent Current raw source code of the block.
 */
void walia_diff_visualize(const char* symbolName, uint8_t* oldHash, const char* newContent);

#endif // WALIA_TOOLING_DIFF_H
