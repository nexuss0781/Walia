#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../tooling/tooling_oracle_lexer.h"
#include "../core/scanner.h"
#include "../core/object.h"
#include "../core/memory.h"

// ==========================================
// INTERNAL REGISTRY
// ==========================================

static WldDocSnippet* globalSnippets = NULL;
static int snippetCount = 0;
static int snippetCapacity = 0;

void walia_oracle_init() {
    snippetCount = 0;
    snippetCapacity = 32;
    globalSnippets = ALLOCATE(WldDocSnippet, snippetCapacity);
    printf(">> Walia Oracle: Contract Lexer initialized.\n");
}

// ==========================================
// EXTRACTION ENGINE
// ==========================================

static void register_snippet(WldDocSnippet* snip) {
    if (snippetCount >= snippetCapacity) {
        int old = snippetCapacity;
        snippetCapacity = GROW_CAPACITY(old);
        globalSnippets = GROW_ARRAY(WldDocSnippet, globalSnippets, old, snippetCapacity);
    }
    globalSnippets[snippetCount++] = *snip;
}

void walia_oracle_scan_file(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) return;

    char line[1024];
    int currentLine = 0;
    
    WldDocSnippet currentSnip;
    bool inDocBlock = false;
    memset(&currentSnip, 0, sizeof(WldDocSnippet));

    while (fgets(line, sizeof(line), file)) {
        currentLine++;
        
        // 1. Detect Oracle Indicator (Triple-Slash)
        if (strncmp(line, "///", 3) == 0) {
            if (!inDocBlock) {
                inDocBlock = true;
                currentSnip.lineStart = currentLine;
                memset(currentSnip.body, 0, MAX_SNIPPET_SIZE);
            }

            // Extract content after '///'
            const char* content = line + 3;
            
            // Check for Example tag: '/// example: name'
            if (strstr(content, "example:")) {
                const char* desc = strstr(content, ":") + 1;
                strncpy(currentSnip.description, desc, 255);
            } else {
                // Otherwise, append to the executable body
                strncat(currentSnip.body, content, MAX_SNIPPET_SIZE - strlen(currentSnip.body) - 1);
            }
            continue;
        }

        // 2. Identify the Target (Look-ahead for Declaration)
        if (inDocBlock) {
            // Check if this line starts a function or class
            char* funPtr = strstr(line, "fun ");
            char* classPtr = strstr(line, "class ");
            
            if (funPtr || classPtr) {
                const char* decl = funPtr ? funPtr + 4 : classPtr + 6;
                
                // Extract identifier name
                int i = 0;
                while (decl[i] != '(' && decl[i] != '{' && decl[i] != ' ' && decl[i] != '\n') {
                    currentSnip.targetSymbol[i] = decl[i];
                    i++;
                    if (i >= 255) break;
                }
                currentSnip.targetSymbol[i] = '\0';
                currentSnip.lineEnd = currentLine;

                // 3. Bind and Archive
                register_snippet(&currentSnip);
                
                printf(">> Walia Oracle: Captured executable contract for '%s' (%d lines).\n", 
                       currentSnip.targetSymbol, (currentSnip.lineEnd - currentSnip.lineStart));
                
                // Reset for next block
                inDocBlock = false;
                memset(&currentSnip, 0, sizeof(WldDocSnippet));
            } else if (strlen(line) > 1 && line[0] != ' ' && line[0] != '\t' && line[0] != '\n' && line[0] != '/') {
                // If we hit a non-comment, non-declaration line, the doc block has ended without a parent
                inDocBlock = false;
            }
        }
    }

    fclose(file);
}

WldDocSnippet* walia_oracle_get_snippets(int* count) {
    *count = snippetCount;
    return globalSnippets;
}
