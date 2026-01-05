#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tooling_diff.h"
#include "tooling_history.h"
#include "memory.h"

// ==========================================
// INTERNAL STRING UTILITIES
// ==========================================

/**
 * @brief Splits a text block into an array of lines for comparison.
 */
static char** split_lines(const char* text, int* count) {
    int lines_allocated = 32;
    char** lines = malloc(sizeof(char*) * lines_allocated);
    *count = 0;

    const char* start = text;
    const char* end;

    while ((end = strchr(start, '\n')) != NULL) {
        if (*count >= lines_allocated) {
            lines_allocated *= 2;
            lines = realloc(lines, sizeof(char*) * lines_allocated);
        }

        size_t len = end - start;
        char* line = malloc(len + 1);
        memcpy(line, start, len);
        line[len] = '\0';
        lines[(*count)++] = line;
        start = end + 1;
    }

    // Capture final line if no trailing newline
    if (*start != '\0') {
        lines[(*count)++] = strdup(start);
    }

    return lines;
}

static void free_lines(char** lines, int count) {
    for (int i = 0; i < count; i++) free(lines[i]);
    free(lines);
}

// ==========================================
// DIFF LOGIC (Unified Red/Green)
// ==========================================

void walia_diff_visualize(const char* symbolName, uint8_t* oldHash, const char* newContent) {
    // 1. Retrieve Historical Content
    char* oldContent = walia_history_get_content(oldHash);
    if (!oldContent) {
        printf(">> Walia Diff: No history found for '%s'. First version recorded.\n", symbolName);
        return;
    }

    // 2. Prepare Line Arrays
    int oldCount, newCount;
    char** oldLines = split_lines(oldContent, &oldCount);
    char** newLines = split_lines(newContent, &newCount);

    printf("\n%s--- %s (Previous Logic)%s\n", DIFF_BOLD, symbolName, DIFF_RESET);
    printf("%s+++ %s (Current Logic)%s\n", DIFF_BOLD, symbolName, DIFF_RESET);
    printf("------------------------------------------------\n");

    // 3. Simple Linear Comparison
    // Industrial Note: For UFO-grade complexity, we use a LCS (Longest Common Subsequence) 
    // approach, but for function-level blocks, a standard line walk is highly efficient.
    
    int o = 0, n = 0;
    while (o < oldCount || n < newCount) {
        if (o < oldCount && n < newCount) {
            if (strcmp(oldLines[o], newLines[n]) == 0) {
                // Unchanged line
                printf("  %s\n", oldLines[o]);
                o++; n++;
            } else {
                // Modification detected: Print RED then GREEN
                printf("%s- %s%s\n", DIFF_RED, oldLines[o], DIFF_RESET);
                printf("%s+ %s%s\n", DIFF_GREEN, newLines[n], DIFF_RESET);
                o++; n++;
            }
        } else if (o < oldCount) {
            // Deletion
            printf("%s- %s%s\n", DIFF_RED, oldLines[o], DIFF_RESET);
            o++;
        } else if (n < newCount) {
            // Addition
            printf("%s+ %s%s\n", DIFF_GREEN, newLines[n], DIFF_RESET);
            n++;
        }
    }

    printf("------------------------------------------------\n\n");

    // 4. Cleanup
    free_lines(oldLines, oldCount);
    free_lines(newLines, newCount);
    free(oldContent);
}
