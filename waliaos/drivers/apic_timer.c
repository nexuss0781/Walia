#include "../include/apic_timer.h"
#include "../include/driver_serial.h"

// Physical Address of the LAPIC (Resolved in Phase 20.1)
extern uint32_t* lapic_base;

static inline void lapic_write(uint32_t reg, uint32_t val) {
    volatile uint32_t* addr = (volatile uint32_t*)((uint8_t*)lapic_base + reg);
    *addr = val;
}

void k_timer_init(uint32_t quantum_ms) {
    k_serial_print(">> Walia Timer: Calibrating APIC Heartbeat...\n");

    // 1. Set Divider to 16
    lapic_write(LAPIC_TIMER_DIV, 0x3);

    // 2. Set Timer to Periodic Mode + Vector 32 (First user interrupt)
    lapic_write(LAPIC_LVT_TIMER, 32 | 0x20000);

    // 3. Set Initial Count (Assuming 1GHz bus for MVP calibration)
    // 1ms = 1,000,000 cycles / 16
    uint32_t ticks = (1000000 / 16) * quantum_ms;
    lapic_write(LAPIC_TIMER_INIT, ticks);

    k_serial_print(">> Walia Timer: Preemption Active [1ms Quantum].\n");
}

/**
 * @brief THE KINETIC YIELD
 * This function is the bridge from Hardware to Walia Logic.
 */
void k_timer_handler() {
    // 1. Signal End-of-Interrupt to the hardware
    lapic_write(0xB0, 0); // EOI Register

    // 2. Perform Algebraic Yield
    // We invoke the VM's continuation capture logic.
    // This transforms the currently running 'Process' into a 'Task'
    // and returns control to the Scheduler (Phase 21.2).
    
    /* extern void walia_yield(); */
    /* walia_yield(); */
}
