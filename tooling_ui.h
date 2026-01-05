#ifndef WALIA_TOOLING_UI_H
#define WALIA_TOOLING_UI_H

#include "common.h"
#include "tooling_registry.h"
#include "tooling_hash_scanner.h"

// ==========================================
// UI COLOR & STYLE SCHEME
// ==========================================

#define UI_CYAN    "\033[0;36m"
#define UI_YELLOW  "\033[0;33m"
#define UI_RED     "\033[0;31m"
#define UI_BOLD    "\033[1m"
#define UI_RESET   "\033[0m"
#define UI_NC      "\033[0m" // Fixed: Added alias for No Color
#define UI_CLEAR   "\033[2J\033[H"

// ==========================================
// INTERACTIVE API
// ==========================================

void walia_resolve_conflict(WaliaRegistryEntry* existing, WaliaSymbolBlock* incoming);
void walia_ui_draw_progress(const char* label, int current, int total);
void walia_ui_prompt_rename(char* outName, const char* defaultSuggestion);

#endif // WALIA_TOOLING_UI_H
