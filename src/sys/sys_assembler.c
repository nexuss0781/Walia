#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "sys/sys_assembler.h"
#include "core/memory.h"
#include "core/vm.h"

// ==========================================
// ARCHITECTURE DETECTION
// ==========================================

#if defined(__x86_64__)
    #define WALIA_ARCH_X64
#elif defined(__aarch64__)
    #define WALIA_ARCH_ARM
#endif

// ==========================================
// INSTRUCTION ENCODING (UFO-Grade JIT)
// ==========================================

/**
 * @brief Simple mnemonic-to-binary encoder.
 * In production, this utilizes a full Trie-based instruction set.
 * For Phase 11.4, we implement the core System Mnemonics.
 */
static size_t encode_instruction(const char* mnemonic, uint8_t* out) {
#ifdef WALIA_ARCH_X64
    // x86_64 Encoding
    if (strcmp(mnemonic, "syscall") == 0) {
        out[0] = 0x0F; out[1] = 0x05;
        return 2;
    }
    if (strcmp(mnemonic, "ret") == 0) {
        out[0] = 0xC3;
        return 1;
    }
    if (strcmp(mnemonic, "nop") == 0) {
        out[0] = 0x90;
        return 1;
    }
#elif defined(WALIA_ARCH_ARM)
    // ARM64 Encoding
    if (strcmp(mnemonic, "svc #0") == 0) {
        out[0] = 0x01; out[1] = 0x00; out[2] = 0x00; out[3] = 0xD4;
        return 4;
    }
    if (strcmp(mnemonic, "ret") == 0) {
        out[0] = 0xC0; out[1] = 0x03; out[2] = 0x5F; out[3] = 0xD6;
        return 4;
    }
#endif
    return 0;
}

// ==========================================
// MASTER ASSEMBLER ENGINE
// ==========================================

ObjCodeBlob* sys_assemble(const char* source) {
    uint8_t tempBuffer[4096];
    size_t totalBytes = 0;

    // 1. Simple Tokenization (Line by Line)
    char* srcCopy = strdup(source);
    char* line = strtok(srcCopy, "\n;");
    
    while (line != NULL) {
        // Strip whitespace
        while (*line == ' ' || *line == '\t') line++;
        if (*line != '\0') {
            size_t bytes = encode_instruction(line, tempBuffer + totalBytes);
            if (bytes == 0) {
                fprintf(stderr, "Walia Assembler: Unknown mnemonic '%s'\n", line);
            }
            totalBytes += bytes;
        }
        line = strtok(NULL, "\n;");
    }
    free(srcCopy);

    // 2. Persistent Executable Allocation
    // We allocate from the persistent heap but must align to page boundaries
    // to satisfy mprotect() requirements.
    long pageSize = sysconf(_SC_PAGESIZE);
    size_t allocSize = (sizeof(ObjCodeBlob) + totalBytes + pageSize - 1) & ~(pageSize - 1);
    
    ObjCodeBlob* blob = (ObjCodeBlob*)reallocate(NULL, 0, allocSize);
    blob->obj.type = OBJ_NATIVE; // Executable blobs are treated as Native logic
    blob->obj.isMarked = false;
    blob->obj.next = vm.objects;
    vm.objects = (Obj*)blob;
    
    blob->size = totalBytes;
    memcpy(blob->code, tempBuffer, totalBytes);

    // 3. APPLY SOVEREIGN EXECUTION PRIVILEGES
    // This allows the CPU to physically run the code stored in the heap.
    if (mprotect(blob, allocSize, PROT_READ | PROT_EXEC) == -1) {
        perror("Walia Security: Failed to set executable permissions on code blob");
    }

    markCard(blob);
    return blob;
}

// ==========================================
// EXECUTION HOOK
// ==========================================

void sys_asm_execute(ObjCodeBlob* blob) {
    if (blob == NULL || blob->size == 0) return;

    // Define a function pointer to the raw machine code
    void (*machine_code)() = (void (*)())blob->code;

    // UFO GRADE SPEED: Direct CPU jump
    machine_code();
}
