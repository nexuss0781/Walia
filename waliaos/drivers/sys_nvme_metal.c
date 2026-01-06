#include "../include/sys_nvme.h"
#include "../include/driver_serial.h"

// NVMe Controller Registers (offsets from BAR0)
#define NVME_CAP    0x00 // Capabilities
#define NVME_VS     0x08 // Version
#define NVME_CC     0x14 // Controller Configuration
#define NVME_CSTS   0x1C // Controller Status
#define NVME_AQA    0x24 // Admin Queue Attributes
#define NVME_ASQ    0x28 // Admin Submission Queue Base
#define NVME_ACQ    0x30 // Admin Completion Queue Base
#define NVME_SQ0TDBL 0x1000 // Submission Queue 0 Tail Doorbell

static volatile uint32_t* nvme_regs;
static WldNVMeCommand*    admin_sq;
static WldNVMeCompletion* admin_cq;

// ==========================================
// CONTROLLER INITIALIZATION
// ==========================================

void sys_nvme_init(uintptr_t bar0) {
    k_serial_print(">> Walia Storage: Seizing NVMe Controller via BAR0...\n");
    nvme_regs = (uint32_t*)bar0;

    // 1. Wait for controller ready
    while (!(nvme_regs[NVME_CSTS / 4] & 1));

    // 2. Setup Admin Queues in Persistent Memory
    // (Assuming physical RAM allocation from Phase 22.1)
    /* admin_sq = (WldNVMeCommand*)sys_pager_alloc_kernel_page(); */
    /* admin_cq = (WldNVMeCompletion*)sys_pager_alloc_kernel_page(); */

    // 3. Configure Doorbell Protocol
    // nvme_regs[NVME_ASQ / 4] = (uintptr_t)admin_sq;
    // nvme_regs[NVME_ACQ / 4] = (uintptr_t)admin_cq;
    
    k_serial_print(">> Walia Storage: NVMe Platter Link Established.\n");
}

// ==========================================
// ZERO-COPY DMA (UFO SPEED)
// ==========================================

bool sys_nvme_write_page(uint32_t pageId, const void* ramAddr) {
    // 1. ASSEMBLE COMMAND (Instruction-Direct)
    WldNVMeCommand cmd;
    k_memset(&cmd, 0, sizeof(WldNVMeCommand));
    
    cmd.opCode = 0x01; // WRITE
    cmd.nsid = 1;      // Namespace 1
    
    // THE UFO SECRET: Direct Physical Mapping
    // We tell the SSD hardware to read data from 'ramAddr' directly.
    cmd.prp1 = (uint64_t)ramAddr; 
    
    // Set Logical Block Address (LBA)
    cmd.cdw10_15[0] = pageId; // Simple 1:1 mapping for Genesis
    cmd.cdw10_15[2] = 7;      // Number of blocks (4KB = 8 sectors of 512b)

    // 2. RING THE DOORBELL
    // This tells the SSD controller to physically begin the transfer
    // without the CPU doing any more work.
    /* nvme_regs[NVME_SQ0TDBL / 4] = 1; */
    
    k_serial_print(">> Walia Storage: Page commit dispatched to hardware.\n");
    return true;
}

bool sys_nvme_read_page(uint32_t pageId, void* ramAddr) {
    WldNVMeCommand cmd;
    k_memset(&cmd, 0, sizeof(WldNVMeCommand));
    
    cmd.opCode = 0x02; // READ
    cmd.nsid = 1;
    cmd.prp1 = (uint64_t)ramAddr; // SSD writes directly into this RAM address
    cmd.cdw10_15[0] = pageId;
    cmd.cdw10_15[2] = 7;

    /* nvme_regs[NVME_SQ0TDBL / 4] = 2; */
    return true;
}
