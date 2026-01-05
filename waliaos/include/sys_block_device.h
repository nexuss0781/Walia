#ifndef WALIA_SYS_BLOCK_DEVICE_H 
#define WALIA_SYS_BLOCK_DEVICE_H 

#include "walia_kernel_base.h" 

// ========================================== 
// BLOCK DEVICE TRAITS 
// ========================================== 

typedef enum { 
    BLOCK_TYPE_NVME,    // High-parallelism SSD 
    BLOCK_TYPE_AHCI,    // Sequential/Legacy SATA 
    BLOCK_TYPE_VIRTUAL  // RAM-disk or Network-mapped 
} WldBlockType; 

struct WldBlockDevice; 

/** 
 * @brief Function pointer table for hardware-agnostic I/O. 
 */ 
typedef struct { 
    bool (*read_page)(uint32_t pageId, void* buf); 
    bool (*write_page)(uint32_t pageId, const void* buf); 
    void (*flush)(void); 
} WldBlockOps; 

/** 
 * @brief The Sovereign Block Descriptor. 
 */ 
typedef struct { 
    uint32_t     deviceId; 
    WldBlockType type; 
    bool         isRotational; // True for HDD (Triggers Elevator Sched) 
    uint64_t     totalSectors; 
    WldBlockOps  ops;          // Bound hardware operations 
    void*        privateData;  // Pointer to controller-specific state 
} WldBlockDevice; 

// ========================================== 
// BLOCK FABRIC API 
// ========================================== 

/** 
 * @brief Initializes the Global Block Fabric. 
 */ 
void sys_block_fabric_init(); 

/** 
 * @brief Registers a physical controller into the Fabric. 
 */ 
void sys_block_register_device(WldBlockDevice* dev); 

/** 
 * @brief The Primary I/O Entry Point. 
 * 
 * Logic: 
 * 1. Identifies the target device for the PageID. 
 * 2. If device is rotational, it queues the request for the Elevator. 
 * 3. If device is parallel (SSD), it dispatches immediately. 
 */ 
bool sys_block_read(uint32_t pageId, void* buf); 
bool sys_block_write(uint32_t pageId, const void* buf); 

#endif // WALIA_SYS_BLOCK_DEVICE_H 
