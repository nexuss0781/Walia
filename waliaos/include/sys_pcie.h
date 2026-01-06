#ifndef WALIA_SYS_PCIE_H
#define WALIA_SYS_PCIE_H

#include "walia_kernel_base.h"

// ==========================================
// PCIe HARDWARE PORTS (x86_64 Standard)
// ==========================================
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

// ==========================================
// PCIe DEVICE DESCRIPTOR
// ==========================================

/**
 * @brief Represents a discovered physical device.
 */
typedef struct {
    uint8_t  bus;
    uint8_t  slot;
    uint8_t  func;
    
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t  class_code;
    uint8_t  subclass;
    
    // Base Address Registers (MMIO entry points)
    uintptr_t bar[6];
} WldPciDevice;

// ==========================================
// PCI METAL API
// ==========================================

/**
 * @brief Reads a 32-bit register from the PCI configuration space.
 */
uint32_t sys_pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

/**
 * @brief Scans the entire PCIe bus to identify hardware 'Roles'.
 * 
 * UFO SPEED: Performs a parallel discovery pass across the motherboard.
 */
int sys_pci_scan(WldPciDevice* out_buffer, int max_count);

/**
 * @brief Returns the physical MMIO address of a device register.
 */
uintptr_t sys_pci_get_bar(uint8_t bus, uint8_t slot, uint8_t func, int bar_index);

#endif // WALIA_SYS_PCIE_H
