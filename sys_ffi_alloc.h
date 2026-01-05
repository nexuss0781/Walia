#ifndef WALIA_SYS_FFI_ALLOC_H
#define WALIA_SYS_FFI_ALLOC_H

#include "common.h"

// ==========================================
// EXECUTABLE MEMORY API
// ==========================================

/**
 * @brief Allocates a block of memory capable of holding executable machine code.
 * 
 * Logic:
 * 1. Utilizes mmap() to request a page with PROT_READ | PROT_WRITE.
 * 2. Ensures the memory is aligned to the system page size.
 * 
 * @param size The required size in bytes.
 * @return void* Pointer to the writable memory block.
 */
void* ffi_alloc_code_segment(size_t size);

/**
 * @brief Transitions a code segment from Writable to Executable.
 * 
 * This implements the W^X security protocol. Once finalized, the code 
 * can be executed by the CPU but can no longer be modified, preventing 
 * code-injection attacks.
 * 
 * @param ptr The base address of the code segment.
 * @param size The size of the segment.
 * @return bool True if the hardware permissions were successfully updated.
 */
bool ffi_finalize_code_segment(void* ptr, size_t size);

/**
 * @brief Releases executable memory back to the Operating System.
 */
void ffi_free_code_segment(void* ptr, size_t size);

#endif // WALIA_SYS_FFI_ALLOC_H