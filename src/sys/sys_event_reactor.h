#ifndef WALIA_SYS_EVENT_REACTOR_H
#define WALIA_SYS_EVENT_REACTOR_H

#include "../web/wld_event_common.h"

// ==========================================
// REACTOR ENGINE SPECIFICATION
// ==========================================

/**
 * @brief The Sovereign Reactor Substrate.
 * Manages the connection to the Kernel I/O rings.
 */
typedef struct {
    int  poll_fd;           // epoll/kqueue descriptor
    int  uring_fd;          // io_uring descriptor (if supported)
    bool use_uring;
    
    // Shared Memory Rings (for io_uring path)
    void* sq_ptr;           // Submission Queue
    void* cq_ptr;           // Completion Queue
} WldReactor;

// ==========================================
// REACTOR API
// ==========================================

/**
 * @brief Initializes the low-level I/O reactor.
 * Probes the kernel for io_uring support and falls back to epoll if needed.
 */
SerResult sys_event_reactor_init(WldReactor* reactor);

/**
 * @brief Registers a file descriptor for non-blocking monitoring.
 */
SerResult sys_event_reactor_add(WldReactor* reactor, int fd, WldEventType type);

/**
 * @brief Reaps completed I/O operations from the kernel.
 * 
 * UFO SPEED: This is a non-blocking "harvest" pass that collects 
 * ready-events and populates the Walia Event Horizon.
 * 
 * @param outEvents Array to store reaped completions.
 * @param maxEvents Size of the outEvents array.
 * @return int Number of events harvested.
 */
int sys_event_reactor_poll(WldReactor* reactor, WldEvent* outEvents, int maxEvents);

/**
 * @brief Shutdown and release kernel resources.
 */
void sys_event_reactor_shutdown(WldReactor* reactor);

#endif // WALIA_SYS_EVENT_REACTOR_H
