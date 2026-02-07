#include "../include/sys_pcie.h"
#include "../include/driver_serial.h"

// ==========================================
// LOW-LEVEL PORT I/O (The Metal)
// ==========================================

static inline void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// ==========================================
// CONFIGURATION SPACE ACCESS
// ==========================================

uint32_t sys_pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    // Create configuration address as per PCI spec:
    // [31: Enable Bit | 23-16: Bus | 15-11: Slot | 10-8: Function | 7-2: Register Offset]
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write address to port 0xCF8
    outl(PCI_CONFIG_ADDRESS, address);
    
    // Read data from port 0xCFC
    return inl(PCI_CONFIG_DATA);
}

// ==========================================
// SOVEREIGN BUS SCAN
// ==========================================

int sys_pci_scan(WldPciDevice* out, int max) {
    k_serial_print(">> Walia Bus Sentry: Initiating Motherboard Discovery...\n");
    int count = 0;

    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t slot = 0; slot < 32; slot++) {
            for (uint8_t func = 0; func < 8; func++) {
                uint32_t reg0 = sys_pci_read_config(bus, slot, func, 0);
                uint16_t vendor = reg0 & 0xFFFF;
                
                if (vendor == 0xFFFF) continue; // No device present

                WldPciDevice* dev = &out[count++];
                dev->bus = (uint8_t)bus;
                dev->slot = slot;
                dev->func = func;
                dev->vendor_id = vendor;
                dev->device_id = (reg0 >> 16) & 0xFFFF;

                // Identify Class/Subclass
                uint32_t reg8 = sys_pci_read_config(bus, slot, func, 8);
                dev->class_code = (reg8 >> 24) & 0xFF;
                dev->subclass   = (reg8 >> 16) & 0xFF;

                // UFO SPEED: Direct MMIO Identification
                // Read BAR0 to find where the device is mapped in physical RAM
                dev->bar[0] = sys_pci_read_config(bus, slot, func, 0x10);

                if (count >= max) return count;
            }
        }
    }

    k_serial_print(">> Walia Bus Sentry: Discovery complete.\n");
    return count;
}
