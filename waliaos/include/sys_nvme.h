#ifndef WALIA_SYS_NVME_H
#define WALIA_SYS_NVME_H

#include "walia_kernel_base.h"

// ==========================================
// NVMe HARDWARE LAYOUTS (Standard 1.4+)
// ==========================================

/**
 * @brief NVMe Admin/Submission Queue Entry (64 bytes).
 * Designed for immediate SIMD-accelerated submission.
 */
typedef struct __attribute__((packed)) {
    uint8_t  opCode;
    uint8_t  flags;
    uint16_t commandId;
    uint32_t nsid;        // Namespace ID
    uint64_t reserved;
    uint64_t metadata;
    uint64_t prp1;        // Physical Region Page (Pointer to data)
    uint64_t prp2;
    uint32_t cdw10_15[6]; // Command-specific data
} WldNVMeCommand;

/**
 * @brief NVMe Completion Queue Entry (16 bytes).
 */
typedef struct __attribute__((packed)) {
    uint32_t result;
    uint32_t reserved;
    uint16_t sqHead;
    uint16_t sqId;
    uint16_t commandId;
    uint16_t status;
} WldNVMeCompletion;

// ==========================================
// NVMe API
// ==========================================

/**
 * @brief Initializes the NVMe controller discovered on the PCIe bus.
 * @param bar0 The physical MMIO address of the controller.
 */
void sys_nvme_init(uintptr_t bar0);

/**
 * @brief Reads a 4KB sector directly into a Sovereign Slotted Page.
 * UFO SPEED: Performs a zero-copy DMA transfer.
 */
bool sys_nvme_read_page(uint32_t pageId, void* ramAddr);

/**
 * @brief Writes a Sovereign Slotted Page to the physical platter.
 */
bool sys_nvme_write_page(uint32_t pageId, const void* ramAddr);

#endif // WALIA_SYS_NVME_H
