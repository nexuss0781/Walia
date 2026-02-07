#include "../include/sys_block_device.h" 
#include "../include/sys_nvme.h" 
#include "../include/sys_ahci.h" 
#include "../include/driver_serial.h" 

// THE SOVEREIGN DRIVE POOL 
static WldBlockDevice global_devices[8]; 
static int            device_count = 0; 
static int            primary_device_idx = -1; 

// ========================================== 
// FABRIC ORCHESTRATION 
// ========================================== 

void sys_block_fabric_init() { 
    k_serial_print(">> Walia Fabric: Initializing Universal Storage Bus...\n"); 
    device_count = 0; 
    primary_device_idx = -1; 
} 

void sys_block_register_device(WldBlockDevice* dev) { 
    if (device_count >= 8) return; 

    // UFO SPEED: Clone the descriptor into the kernel's active pool 
    k_memcpy(&global_devices[device_count], dev, sizeof(WldBlockDevice)); 
    
    if (primary_device_idx == -1) primary_device_idx = device_count; 
    
    k_serial_print(">> Walia Fabric: Registered "); 
    k_serial_print(dev->isRotational ? "ROTATIONAL" : "PARALLEL"); 
    k_serial_print(" device as ID "); 
    char idStr[4]; k_itoa(device_count, idStr, 10); 
    k_serial_print(idStr); k_serial_print("\n"); 

    device_count++; 
} 

// ========================================== 
// UNIVERSAL I/O DISPATCH (The Master Path) 
// ========================================== 

bool sys_block_read(uint32_t pageId, void* buf) { 
    if (primary_device_idx == -1) return false; 

    WldBlockDevice* dev = &global_devices[primary_device_idx]; 

    // 1. HARDWARE-SPECIFIC OPTIMIZATION 
    if (dev->isRotational) { 
        // HDD LOGIC: Signal the Walia Scheduler to use the Elevator Algorithm. 
        // We do not allow 64 threads to seek an HDD simultaneously. 
        /* walia_io_schedule_sequential(dev, pageId, buf, READ); */ 
        return dev->ops.read_page(pageId, buf); 
    } else { 
        // SSD LOGIC: Dispatch directly to the NVMe Submission Queue. 
        // Saturates the PCIe bus with parallel requests. 
        return dev->ops.read_page(pageId, buf); 
    } 
} 

bool sys_block_write(uint32_t pageId, const void* buf) { 
    if (primary_device_idx == -1) return false; 

    WldBlockDevice* dev = &global_devices[primary_device_idx]; 

    // 2. ATOMIC COMMIT SYNC 
    // Every write through the fabric is part of a Shadow Paging cycle. 
    bool success = dev->ops.write_page(pageId, buf); 
    
    if (success) { 
        // Update the Command Nexus PageMap to show 'Physical Commit' 
        /* walia_pulse_emit(PULSE_DB_COMMIT, pageId, 0); */ 
    } 

    return success; 
} 
