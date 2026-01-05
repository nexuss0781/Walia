#ifndef WALIA_SYS_AHCI_H
#define WALIA_SYS_AHCI_H

#include "walia_kernel_base.h"

// ==========================================
// AHCI HARDWARE SPECIFICATIONS
// ==========================================

#define SATA_SIG_ATA    0x00000101  // SATA drive
#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE  1

/**
 * @brief HBA Port Register Map.
 * Matches the physical memory layout of the SATA controller.
 */
typedef struct {
    uint32_t clb;       // Command list base address (low)
    uint32_t clbu;      // Command list base address (high)
    uint32_t fb;        // FIS base address (low)
    uint32_t fbu;       // FIS base address (high)
    uint32_t is;        // Interrupt status
    uint32_t ie;        // Interrupt enable
    uint32_t cmd;       // Command and status
    uint32_t reserved0;
    uint32_t tfd;       // Task file data
    uint32_t sig;       // Signature
    uint32_t ssts;      // SATA status
    uint32_t sctl;      // SATA control
    uint32_t serr;      // SATA error
    uint32_t sact;      // SATA active
    uint32_t ci;        // Command issue
} HbaPort;

/**
 * @brief Host Bus Adapter Memory Map.
 */
typedef struct {
    uint32_t cap;       // Host capabilities
    uint32_t ghc;       // Global host control
    uint32_t is;        // Interrupt status
    uint32_t pi;        // Ports implemented
    uint32_t vs;        // Version
    HbaPort  ports[32]; // 32 ports max
} HbaMem;

/**
 * @brief Command Header (32 bytes).
 */
typedef struct {
    uint8_t  opts;      // FIS length, direction, etc.
    uint8_t  atapi;
    uint16_t prdtl;     // Physical region descriptor table length
    uint32_t prdbc;     // Physical region descriptor byte count
    uint32_t ctba;      // Command table descriptor base address
    uint32_t ctbau;
    uint32_t reserved[4];
} HbaCmdHeader;

// ==========================================
// AHCI API
// ==========================================

/**
 * @brief Initializes the AHCI controller found on the PCIe bus.
 * @param bar5 The MMIO base address of the AHCI controller.
 */
void sys_ahci_init(uintptr_t bar5);

/**
 * @brief Performs a hardware-direct sector read.
 */
bool sys_ahci_read(int portIdx, uint64_t startLba, uint16_t count, void* buf);

/**
 * @brief Performs a hardware-direct sector write.
 */
bool sys_ahci_write(int portIdx, uint64_t startLba, uint16_t count, void* buf);

#endif // WALIA_SYS_AHCI_H
