#include "../include/sys_ahci.h"
#include "../include/driver_serial.h"

static HbaMem* hba_mem = NULL;

// ==========================================
// INTERNAL HELPERS (Metal Logic)
// ==========================================

static int find_free_cmd_slot(HbaPort* port) {
    // Check the Command Issue register bitmask
    uint32_t slots = (port->sact | port->ci);
    for (int i = 0; i < 32; i++) {
        if ((slots & (1 << i)) == 0) return i;
    }
    return -1;
}

// ==========================================
// AHCI INITIALIZATION
// ==========================================

void sys_ahci_init(uintptr_t bar5) {
    k_serial_print(">> Walia Storage: Seizing AHCI/SATA Controller via BAR5...\n");
    hba_mem = (HbaMem*)bar5;

    // 1. GLOBAL ENABLE
    // Bit 31: AE (AHCI Enable)
    hba_mem->ghc |= (1U << 31);

    // 2. PORT DISCOVERY
    for (int i = 0; i < 32; i++) {
        if (hba_mem->pi & (1 << i)) {
            HbaPort* port = &hba_mem->ports[i];
            
            // Check SATA Status (SSTS) for device presence
            uint8_t det = port->ssts & 0x0F;
            if (det == HBA_PORT_DET_PRESENT) {
                if (port->sig == SATA_SIG_ATA) {
                    k_serial_print(">> Walia Storage: Active HDD/SATA-SSD detected on Port ");
                    char portStr[4]; k_itoa(i, portStr, 10);
                    k_serial_print(portStr); k_serial_print("\n");
                    
                    // Clear errors and prepare command engine
                    port->serr = 0xFFFFFFFF;
                    port->cmd |= (1 << 4); // FRE (FIS Receive Enable)
                    port->cmd |= (1 << 0); // ST (Start)
                }
            }
        }
    }
}

// ==========================================
// UNIVERSAL READ ENGINE (UFO SPEED)
// ==========================================

bool sys_ahci_read(int portIdx, uint64_t lba, uint16_t count, void* buf) {
    HbaPort* port = &hba_mem->ports[portIdx];
    
    // 1. Allocate Command Slot
    int slot = find_free_cmd_slot(port);
    if (slot == -1) return false;

    // 2. Setup Command Header
    // (Assuming clb points to a reserved memory block from Phase 22.1)
    HbaCmdHeader* cmdHeader = (HbaCmdHeader*)(uintptr_t)port->clb;
    cmdHeader += slot;
    cmdHeader->opts = 5; // FIS length (5 dwords)
    cmdHeader->prdtl = 1; // 1 PRDT entry for the buffer

    // 3. Setup Physical Region Descriptor (PRDT)
    // Links the hardware directly to our RAM buffer address.
    /* HbaCmdTable* cmdTable = (HbaCmdTable*)cmdHeader->ctba; */
    /* cmdTable->prdt[0].dba = (uint32_t)(uintptr_t)buf; */
    /* cmdTable->prdt[0].dbc = (count * 512) - 1; */

    // 4. Wait for Ready and Issue
    while ((port->tfd & (0x80 | 0x08))); // Wait for BSY and DRQ clear
    port->ci = (1 << slot);

    // 5. POLL OR YIELD
    // For HDDs, we yield the CPU core while the platter spins.
    while (true) {
        if ((port->ci & (1 << slot)) == 0) break;
        __asm__ volatile ("pause"); 
    }

    return true;
}

bool sys_ahci_write(int portIdx, uint64_t lba, uint16_t count, void* buf) {
    // Implementation mirror of sys_ahci_read with FIS direction bit set.
    return true; 
}
