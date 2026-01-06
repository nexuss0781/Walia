#include "../include/sys_pager_phys.h"
#include "../include/driver_serial.h"

// The Master Map of 2GB Silicon
static WldRAMFrame frame_table[RAM_2GB_PAGES];
static uintptr_t   free_frame_cursor;

void sys_pager_phys_init() {
    k_serial_print(">> Walia Pager: Mapping 2GB Physical Substrate...\n");

    // Initialize all frames as Free and Dimensionless
    for (uint32_t i = 0; i < RAM_2GB_PAGES; i++) {
        frame_table[i].logic_hash_prefix = 0;
        frame_table[i].reference_count = 0;
        frame_table[i].flags = 0;
    }

    // Frames start after the Kernel image (defined in linker.ld)
    extern uint64_t _kernel_end;
    free_frame_cursor = (uintptr_t)&_kernel_end;
    
    k_serial_print(">> Walia Pager: Logic Folding Engine Online.\n");
}

uintptr_t sys_pager_allocate_frame(uint32_t hash) {
    // 1. DEDUPLICATION SEARCH (UFO Speed)
    // We check the frame table for an existing logical match.
    // This is how we make 2GB behave like an Ocean.
    if (hash != 0) {
        for (uint32_t i = 0; i < RAM_2GB_PAGES; i++) {
            if (frame_table[i].logic_hash_prefix == hash && frame_table[i].reference_count > 0) {
                // LOGIC FOLD: Point multiple virtual addresses to one physical spot
                frame_table[i].reference_count++;
                return (uintptr_t)(i * PAGE_SIZE);
            }
        }
    }

    // 2. PHYSICAL ALLOCATION (Fallback)
    // If no match, we carve a new 4KB slice from the RAM.
    uintptr_t newFrame = free_frame_cursor;
    free_frame_cursor += PAGE_SIZE;

    uint32_t idx = newFrame / PAGE_SIZE;
    frame_table[idx].logic_hash_prefix = hash;
    frame_table[idx].reference_count = 1;
    frame_table[idx].flags = 0x01; // Marked as Active

    return newFrame;
}

void sys_pager_mark_cold(uintptr_t addr) {
    uint32_t idx = addr / PAGE_SIZE;
    frame_table[idx].flags |= 0x04; // Set Cold Bit for Sentry
}
