#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "tooling_hash_scanner.h"
#include "scanner.h"
#include "hash.h"
#include "memory.h"

static void extract_blocks(const char* path, const char* source) {
    initScanner(source);
    
    for (;;) {
        Token token = scanToken();
        if (token.type == TOKEN_EOF) break;

        if (token.type == TOKEN_FUN || token.type == TOKEN_CLASS) {
            WaliaBlockType type = (token.type == TOKEN_FUN) ? BLOCK_FUNCTION : BLOCK_CLASS;
            int startLine = token.line; // Correctly using the line from the token

            Token nameToken = scanToken();
            if (nameToken.type != TOKEN_IDENTIFIER) continue;

            char symbolName[256];
            int nameLen = nameToken.length > 255 ? 255 : nameToken.length;
            memcpy(symbolName, nameToken.start, nameLen);
            symbolName[nameLen] = '\0';

            while (token.type != TOKEN_LEFT_BRACE && token.type != TOKEN_EOF) {
                token = scanToken();
            }
            if (token.type == TOKEN_EOF) break;

            const char* bodyStart = token.start + 1;
            int braceDepth = 1;

            while (braceDepth > 0) {
                token = scanToken();
                if (token.type == TOKEN_LEFT_BRACE) braceDepth++;
                if (token.type == TOKEN_RIGHT_BRACE) braceDepth--;
                if (token.type == TOKEN_EOF) break;
            }

            const char* bodyEnd = token.start;
            int endLine = token.line;

            WaliaHashContext ctx;
            walia_hash_init(&ctx);
            walia_hash_update(&ctx, (uint8_t*)bodyStart, (size_t)(bodyEnd - bodyStart));
            
            uint8_t hashResult[32];
            walia_hash_final(&ctx, hashResult);

            printf(">> Walia Sentry: Identified %s '%s' in %s [Lines %d-%d]\n", 
                   (type == BLOCK_FUNCTION ? "Function" : "Class"),
                   symbolName, path, startLine, endLine);
        }
    }
}

// ... rest of the file (walia_scan_file, walia_scan_project) remains as before
void walia_scan_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) return;
    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(size + 1);
    if (fread(buffer, sizeof(char), size, file) < size) {
        free(buffer); fclose(file); return;
    }
    buffer[size] = '\0';
    fclose(file);
    extract_blocks(path, buffer);
    free(buffer);
}

void walia_scan_project(const char* dirName) {
    DIR* dir = opendir(dirName);
    if (!dir) return;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirName, entry->d_name);
        struct stat st;
        if (stat(path, &st) == -1) continue;
        if (S_ISDIR(st.st_mode)) walia_scan_project(path);
        else {
            const char* ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".wal") == 0) walia_scan_file(path);
        }
    }
    closedir(dir);
}
