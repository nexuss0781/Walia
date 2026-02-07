# Buffer Pool and Zero-Lock Concurrency

## 1. Role of the Buffer Pool
In a traditional database, the Buffer Pool is responsible for caching disk blocks in RAM. Since WaliaDB utilizes **Memory Mapping (`mmap`)**, the Operating System kernel handles the physical movement of data. However, the WaliaDB **Buffer Pool Manager** serves a specialized enterprise role: it manages **Logical-to-Physical Resolution** and ensures **Concurrent Access Synchronization**.

The Buffer Pool is the high-level gatekeeper that prevents "Cache Trashing" and provides the parallel engines (SQL and NoSQL) with thread-safe, direct memory pointers to the sovereign heap.

## 2. LRU Replacement Logic
To maintain a deterministic memory footprint, WaliaDB utilizes a **Least Recently Used (LRU)** replacement policy. Even if the `.wld` file is several terabytes in size, the VM only "tracks" a specific number of active logical pages (Default: 1024) in the Pool.

### The Replacement Lifecycle:
1.  **Acquisition:** When an engine requests a `PageID`, the Pool checks if it is already resident.
2.  **Clock Increment:** Every access updates a monotonic `lastAccess` timestamp on the pool frame.
3.  **Eviction:** If the pool is full, the manager identifies the frame with the oldest timestamp that is not currently "Pinned."
4.  **Sync-on-Evict:** If the page being evicted is "Dirty," the Pool Manager forces an `msync` to physical storage before the slot is reused, maintaining the chain of persistence.

## 3. Pin Counting and Thread Safety
In an MPP (Massively Parallel Processing) environment, multiple CPU cores may attempt to read or write to the same database page simultaneously. WaliaDB manages this via **Pin Counting**.

*   **Pinned State:** When a worker thread acquires a page, its `pinCount` is incremented.
*   **Immutability:** A pinned page cannot be evicted from the pool. This ensures that a long-running SQL Join or Vector Search never loses its memory pointer mid-operation.
*   **Release:** Once the logic is complete, the thread calls `db_pool_release`, making the page eligible for future eviction.

## 4. Zero-Lock Parallelism
The hallmark of WaliaDB's **UFO-grade** performance is its ability to scale across all CPU cores with **Zero Lock Contention** during read operations.

### I. Shadow-Paging Synergy
Because WaliaDB uses **Shadow Paging**, the "Current Sovereign State" of any page is immutable. 
*   **Writers:** Create a new physical shadow copy (Phase 3).
*   **Readers:** Use the Buffer Pool to acquire the last *committed* physical page.
*   **Result:** 64+ threads can read from the same B+ Tree root or HNSW entry point without ever waiting for a mutex or semaphore.

### II. Thread-Local Page Resolution
Each worker thread in the **Parallel Dispatcher** (Phase 5) utilizes the Buffer Pool to resolve Logical IDs to Physical pointers. Since the resolution is a read-only lookup into the Shadow Table, the contention at the hardware level is minimized, allowing the CPU to reach the theoretical limit of the RAM bandwidth.

## 5. Pool Specifications

| Feature | Specification |
| :--- | :--- |
| **Pool Size** | 1024 Logical Slots (Tunable via `WLD_POOL_SIZE`). |
| **Replacement** | Monotonic Clock LRU. |
| **Safety** | Recursive Pin Counting. |
| **Throughput** | UFO-Grade (Direct Pointer Handover). |

## 6. Implementation Interface (`db_pool.c`)
The Buffer Pool exposes a minimalist C API to the Walia runtime, ensuring that the overhead of "Getting a Page" is reduced to a few integer comparisons and an array lookup.

```c
// UFO Speed Acquisition
void* db_pool_acquire(WldBufferPool* pool, WldTransaction* tx, PageID id, bool forWrite);

// Sovereign Release
void db_pool_release(WldBufferPool* pool, PageID id);
```

By keeping the management logic in C and the data pointers in the memory-mapped heap, WaliaDB ensures that the transition between "Database Storage" and "Virtual Machine Registers" is seamless and nearly instantaneous.

---
**Status:** Enterprise Specification 1.0 (Implemented)  
**Algorithm:** Monotonic LRU Cache  
**Concurrency:** Zero-Lock Parallel / Pin-Protected  

