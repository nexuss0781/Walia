# Orthogonal Persistence Mechanics

## 1. Defining Orthogonal Persistence
In the Walia runtime, **Orthogonal Persistence** is the principle where the durability of data is a property of the data itself, not the way it is accessed. Unlike traditional systems that require explicit serialization (JSON, XML) or external databases (SQL), Walia treats the entire Virtual Machine heap as a permanent, sovereign entity. 

If an object is reachable from a **Global Root**, it is mathematically guaranteed to survive process termination, system restarts, and power failures.

## 2. The Memory-Mapped Heap (`mmap`)
Walia achieves zero-overhead persistence by replacing the standard volatile heap (`malloc`) with a **Memory-Mapped File (`mmap`)**. 

*   **Mechanism:** The VM requests a large, contiguous block of virtual address space from the Operating System, backed by the `walia.state` file.
*   **MAP_SHARED:** By using the `MAP_SHARED` flag, Walia instructs the kernel that any modification to a memory address in the heap is a modification to the underlying file.
*   **Zero-Serialization:** Because the RAM and the Disk share the same binary image, there is no "translation" step. An `ObjString` on the disk is identical to an `ObjString` in the CPU cache.

## 3. The Sovereign Heap Layout
The `walia.state` file is structured into three distinct segments:

| Segment | Role | Persistence Level |
| :--- | :--- | :--- |
| **Superblock** | Metadata, Versioning, and Root Pointer. | Permanent |
| **Mature Space** | Long-lived objects and global state. | Permanent |
| **Nursery** | Short-lived transients (Gen 0). | Volatile-to-Permanent |

### The Superblock Anchor
At offset `0x00` of every `.state` file is the **Superblock**. It contains the `Value` representing the **Global Root Table**. When Walia boots, it reads this single 64-bit word to instantly restore the entire web of persistent objects.

## 4. Reachability and the Global Root
Persistence in Walia is governed by **Reachability**. 

1.  **The Root:** The VM maintains a hidden Hash Table known as the Global Root.
2.  **The Trace:** During a Garbage Collection cycle, the VM starts at this Root and follows every pointer (Strings, Closures, Lists).
3.  **Survival:** Any object not reachable from the Root is considered "Garbage" and reclaimed. Any object that *is* reachable is preserved in the persistent segment.

This ensures that the `walia.state` file only grows to accommodate useful data, preventing "Persistence Bloat."

## 5. Dirty Tracking (Card Marking)
To avoid the massive performance penalty of writing the entire 128MB+ heap to disk every time a single variable changes, Walia utilizes **Card Marking**.

*   **The Card Table:** A separate, compact bitset where 1 bit represents a 512-byte "page" (card) of the heap.
*   **The Write Barrier:** Every instruction that modifies a heap object (e.g., `OP_SET_GLOBAL` or `OP_SET_UPVALUE`) triggers a C-level write barrier:
    ```c
    void markCard(void* pointer) {
        size_t offset = (uint8_t*)pointer - heapStart;
        cardTable[offset / 512] = 1; // Mark as Dirty
    }
    ```
*   **Selective Sync:** During a checkpoint, Walia only flushes the "Dirty" cards to the physical disk, making the persistence overhead proportional only to the amount of data changed.

## 6. Atomic Checkpointing (`msync`)
Walia ensures data integrity through **Atomic Checkpointing**.

1.  **Flush:** The VM calls `msync(MS_SYNC)`, forcing the OS to commit all dirty cards to physical storage.
2.  **Timestamp:** Only *after* the flush is confirmed does the VM update the timestamp and checksum in the Superblock.
3.  **Crash Recovery:** If the system crashes during the flush, the Superblock checksum will fail the next boot, and Walia will revert to the last known-good state, preventing the loading of "partial" or corrupted data.

## 7. Industry Implications
By moving persistence into the language substrate, Walia eliminates the **Impedance Mismatch** between application logic and data storage. Developers write code as if RAM were infinite and permanent, while the Walia engine handles the complexities of hardware I/O and data durability in the background.

---
**Status:** Technical Specification 1.0  
**Logic:** Memory-Mapped Shared Heap  
**Integrity:** Card-Marked Atomic Checkpoints
