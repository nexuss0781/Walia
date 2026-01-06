#include "../include/sys_fabric.h"
#include "../include/sys_topology.h"
#include "../include/driver_serial.h"

// The Global Fabric Matrix
// Array of mailboxes, one for each physical CPU core.
static WldMailbox* global_mailboxes;
static int         fabric_core_count = 0;

// ==========================================
// FABRIC BOOTSTRAP
// ==========================================

void sys_fabric_init(int total_cores) {
    k_serial_print(">> Walia Fabric: Initializing Inter-Core Interconnect...\n");

    fabric_core_count = total_cores;

    // Allocate mailboxes in the Sovereign Segment
    extern uint8_t* heapStart;
    global_mailboxes = (WldMailbox*)(heapStart + (1024 * 1024 * 30)); // Offset 30MB

    for (int i = 0; i < total_cores; i++) {
        atomic_init(&global_mailboxes[i].head, 0);
        atomic_init(&global_mailboxes[i].tail, 0);
        k_memset(global_mailboxes[i].buffer, 0, sizeof(WldFabricPulse) * MAILBOX_SIZE);
    }

    k_serial_print(">> Walia Fabric: Sovereign Interconnect established [Wait-Free Mode].\n");
}

// ==========================================
// ATOMIC DELIVERY (UFO SPEED)
// ==========================================

bool sys_fabric_send(int dest, uint32_t type, uint64_t payload) {
    if (dest >= fabric_core_count) return false;

    WldMailbox* box = &global_mailboxes[dest];

    // 1. SLOT RESERVATION
    // We use atomic fetch-add to 'claim' a slot in the target core's ring.
    // This allows multiple cores to send to the same target simultaneously.
    uint32_t current_head = atomic_fetch_add_explicit(&box->head, 1, memory_order_relaxed);
    uint32_t index = current_head % MAILBOX_SIZE;

    // 2. PAYLOAD DEPOSIT
    WldFabricPulse* pulse = &box->buffer[index];
    pulse->type = type;
    pulse->source_id = 0; // In production, resolved via Local APIC ID
    pulse->payload = payload;

    // 3. HARDWARE SIGNAL (Optional)
    // In high-intensity MPP, we emit an IPI (Inter-Processor Interrupt)
    // to wake up the destination core if it is idling in HLT.
    /* sys_apic_signal_core(dest); */

    return true;
}

int sys_fabric_reap(WldFabricPulse* out, int max) {
    // Determine which core we are running on
    /* int my_id = sys_topology_get_current_core(); */
    int my_id = 0; // Simplified for MVP

    WldMailbox* box = &global_mailboxes[my_id];

    uint32_t current_tail = atomic_load_explicit(&box->tail, memory_order_relaxed);
    uint32_t current_head = atomic_load_explicit(&box->head, memory_order_acquire);

    int count = 0;
    while (current_tail < current_head && count < max) {
        uint32_t index = current_tail % MAILBOX_SIZE;
        out[count++] = box->buffer[index];
        current_tail++;
    }

    // Advance the tail atomically to clear the mailbox
    atomic_store_explicit(&box->tail, current_tail, memory_order_release);

    return count;
}
