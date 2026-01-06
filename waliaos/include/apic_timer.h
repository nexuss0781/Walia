#ifndef WALIA_APIC_TIMER_H
#define WALIA_APIC_TIMER_H

#include "walia_kernel_base.h"

// LAPIC Register Offsets (Standard x86_64)
#define LAPIC_TIMER_INIT    0x380
#define LAPIC_TIMER_CURRENT 0x390
#define LAPIC_TIMER_DIV     0x3E0
#define LAPIC_LVT_TIMER     0x320

/**
 * @brief Calibrates and starts the hardware preemption timer.
 * @param quantum_ms The time-slice in milliseconds (e.g., 1ms).
 */
void k_timer_init(uint32_t quantum_ms);

/**
 * @brief The Hardware Interrupt Handler.
 * Called by the CPU when the timer reaches zero.
 */
void k_timer_handler();

#endif // WALIA_APIC_TIMER_H
