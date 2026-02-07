#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tooling/tooling_ui.h"
#include "tooling/tooling_diff.h"

// ==========================================
// INTERNAL UI UTILITIES
// ==========================================

static void print_banner(const char* title) {
    printf("%s%s================================================\n", UI_BOLD, UI_CYAN);
    printf(" WALIA SOVEREIGN ASSISTANT : %s\n", title);
    printf("================================================%s\n\n", UI_RESET);
}

// ==========================================
// CONFLICT RESOLUTION (The "Struggle")
// ==========================================

void walia_resolve_conflict(WaliaRegistryEntry* existing, WaliaSymbolBlock* incoming) {
    printf(UI_CLEAR);
    print_banner("SYMBOL COLLISION DETECTED");

    printf("%sConflict Found:%s Global symbol '%s%s%s' exists in multiple files.\n\n", 
           UI_RED, UI_RESET, UI_BOLD, existing->name, UI_RESET);

    printf("  [1] %s%s%s\n", UI_YELLOW, existing->filePath, UI_RESET);
    printf("      (Existing Logic - Line %d)\n\n", existing->startLine);

    printf("  [2] %s%s%s\n", UI_YELLOW, incoming->filePath, UI_RESET);
    printf("      (Incoming Logic - Line %d)\n\n", incoming->startLine);

    printf("------------------------------------------------\n");
    printf("Select an action to preserve Global Uniqueness:\n");
    printf("  1. Rename #1 (Existing symbol)\n");
    printf("  2. Rename #2 (Incoming symbol)\n");
    printf("  3. Auto-Resolve (Use context-aware names)\n");
    printf("  4. Manual Review (Open in Editor)\n");
    printf("------------------------------------------------\n");
    printf("%sAction Required > %s", UI_BOLD, UI_RESET);

    int choice;
    if (scanf("%d", &choice) != 1) choice = 3; 

    char newName[256];
    switch (choice) {
        case 1:
            walia_ui_prompt_rename(newName, existing->name);
            break;
        case 2:
            walia_ui_prompt_rename(newName, incoming->name);
            break;
        case 3:
            break;
        case 4: {
            char cmd[1024];
            snprintf(cmd, sizeof(cmd), "${EDITOR:-vi} %s", incoming->filePath);
            if (system(cmd) == -1) { /* error handling */ }
            break;
        }
    }

    printf("\nPress [ENTER] to re-verify the project...");
    // Fixed: Explicitly ignore result to satisfy enterprise warnings
    (void)getchar(); 
    (void)getchar(); 
}
// ==========================================
// REFACTORING HELPERS
// ==========================================

void walia_ui_prompt_rename(char* outName, const char* defaultName) {
    printf("\nEnter new unique name (default: %s_2): ", defaultName);
    char input[256];
    if (scanf("%s", input) == 1) {
        strcpy(outName, input);
    } else {
        sprintf(outName, "%s_2", defaultName);
    }
}

void walia_ui_draw_progress(const char* label, int current, int total) {
    float percent = (float)current / total * 100;
    int width = 30;
    int pos = (int)(width * current / total);

    printf("\r%s%-15s%s [", UI_BOLD, label, UI_RESET);
    for (int i = 0; i < width; i++) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %d%%", (int)percent);
    fflush(stdout);
    if (current == total) printf("\n");
}
