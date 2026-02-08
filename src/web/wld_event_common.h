#ifndef WALIA_WLD_EVENT_COMMON_H
#define WALIA_WLD_EVENT_COMMON_H

#include "../core/common.h"
#include "../core/value.h"

// ==========================================
// REACTOR CONFIGURATION
// ==========================================

#define WLD_EVENT_QUEUE_SIZE 65536 // 64k concurrent events (Power of 2)
#define WLD_MAX_FDS          1024  // Maximum open file descriptors
#define WLD_IO_TIMEOUT_MS    5000  // Default I/O timeout

// ==========================================
// EVENT TYPE REGISTRY
// ==========================================

typedef enum {
    WLD_EVENT_NONE = 0,
    WLD_EVENT_READ,        // Socket/File data readiness
    WLD_EVENT_WRITE,       // Buffer drain readiness
    WLD_EVENT_ACCEPT,      // Incoming connection
    WLD_EVENT_CONNECT,     // Outgoing connection established
    WLD_EVENT_TIMER,       // Timing wheel expiration
    WLD_EVENT_SIGNAL,      // OS Signal (SIGINT, etc)
    WLD_EVENT_CUSTOM       // User-defined algebraic effects
} WldEventType;

// ==========================================
// THE SOVEREIGN EVENT PACKET
// ==========================================
/**
 * @brief Represents an entry in the persistent Event Horizon.
 * 
 * Architecture:
 * This struct is designed to fit exactly in 32 bytes, ensuring 
 * that 2 events fit perfectly in a single 64-byte L1 cache line.
 */
typedef struct {
    uint32_t     fd;          // OS File Descriptor
    uint32_t     type;        // Castable to WldEventType
    
    // The "Bridge" to the Effect Engine:
    // This value stores the captured ObjContinuation pointer.
    // When the reactor reaps this event, it dispatches this 
    // continuation to the VM for resumption.
    Value        continuation; 

    uint64_t     timestamp;   // Creation time for timeout logic
    uint64_t     payload;     // Buffer pointer or metadata
} WldEvent;

// ==========================================
// REACTOR STATUS CODES
// ==========================================

typedef enum {
    SER_OK = 0,
    SER_ERR_QUEUE_FULL,
    SER_ERR_POLL_FAILED,
    SER_ERR_KERNEL_INCOMPATIBLE,
    SER_ERR_INVALID_FD,
    SER_ERR_IO,
    SER_ERR_MMAP_FAILED
} SerResult;

#endif // WALIA_WLD_EVENT_COMMON_H
