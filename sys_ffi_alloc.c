#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "sys_ffi_alloc.h"

// ==========================================
// INTERNAL UTILITIES
// ==========================================

/**
 * @brief Rounds a size up to the nearest system page boundary.
 */
static size_t align_to_page(size_t size) {
    long pageSize = sysconf(_SC_PAGESIZE);
    return (size + pageSize - 1) & ~(pageSize - 1);
}

// ==========================================
// SOVEREIGN MEMORY MANAGEMENT
// ==========================================

void* ffi_alloc_code_segment(size_t size) {
    size_t alignedSize = align_to_page(size);

    // 1. Request anonymous memory from the Kernel.
    // Initial permissions: Read + Write (to allow JIT emission)
    void* ptr = mmap(NULL, alignedSize, 
                     PROT_READ | PROT_WRITE, 
                     MAP_ANONYMOUS | MAP_PRIVATE, 
                     -1, 0);

    if (ptr == MAP_FAILED) {
        perror("Walia FFI: Failed to allocate executable memory substrate");
        return NULL;
    }

    return ptr;
}

bool ffi_finalize_code_segment(void* ptr, size_t size) {
    size_t alignedSize = align_to_page(size);

    // 2. HARDWARE HANDOVER: Change permissions to Read + Execute.
    // This removes the Write permission (W^X Protection).
    if (mprotect(ptr, alignedSize, PROT_READ | PROT_EXEC) == -1) {
        perror("Walia Security: Failed to set PROT_EXEC on code thunk");
        return false;
    }

    return true;
}

void ffi_free_code_segment(void* ptr, size_t size) {
    if (ptr == NULL) return;
    size_t alignedSize = align_to_page(size);

    if (munmap(ptr, alignedSize) == -1) {
        perror("Walia FFI: munmap failure");
    }
}