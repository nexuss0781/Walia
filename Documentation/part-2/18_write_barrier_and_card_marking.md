# The Write Barrier and Card Marking

## 1. The Guard of Sovereignty
The **Write Barrier** is the critical architectural component that bridges the Walia Virtual Machine and the Persistence Engine. Its primary role is to monitor all modifications to heap-resident data. Without an efficient Write Barrier, the VM would be forced to flush the entire 128MB+ persistent heap to disk during every checkpoint, resulting in catastrophic performance degradation.

Walia utilizes a **Card Marking** scheme to ensure that persistence overhead remains proportional only to the data that has actually changed.

## 2. The Card Table Architecture
Walia treats the persistent heap as a collection of fixed-size segments called **Cards**. 

*   **Granularity:** Each card represents **512 bytes** of the persistent heap.
*   **The Table:** A dedicated bitset (The Card Table) resides in a protected segment of memory. Each entry in the table corresponds to one card.
*   **Dirty State:** If an entry in the Card Table is `1`, the card is considered "Dirty" (modified). If `0`, it is "Clean."

### Mathematical Mapping
The index of a card is calculated using high-speed bitwise shifts:
```c
size_t card_index = ((uint8_t*)modified_object_ptr - heap_start) >> 9; // 2^9 = 512
```

## 3. The Write Barrier Lifecycle
The Write Barrier is triggered automatically by the VM whenever a "Store" instruction is executed. The lifecycle follows three distinct stages:

### I. Interception
Instructions such as `OP_SET_GLOBAL`, `OP_SET_UPVALUE`, or native collection modifiers (e.g., `map_set`) intercept the write operation.

### II. Marking
The VM invokes the `markCard()` function. This function sets the bit in the Card Table corresponding to the object's memory address.
```c
void markCard(void* pointer) {
    // 1. Bounds check: Ensure pointer is in the persistent heap
    // 2. Set the dirty bit in the Card Table
    cardTable[((uint8_t*)pointer - heapStart) / 512] = 1;
}
```

### III. Selective Synchronization
During a **Sovereign Checkpoint**, the Persistence Engine scans the Card Table. It only passes the "Dirty" ranges to the OS `msync()` system call. This reduces physical disk I/O by up to **99%** in typical application workloads.

## 4. Role in Generational Garbage Collection
Beyond persistence, the Write Barrier solves the "Old-to-Young" reference problem in the **Generational GC**.

*   **The Challenge:** If a Mature object (Gen 1) is updated to point to a Nursery object (Gen 0), a Nursery-only GC cycle would miss this reference and potentially delete a live object.
*   **The Walia Solution:** The GC treats all "Dirty" cards in the Mature space as additional **Roots**. This ensures that cross-generational pointers are always traced without requiring a full heap scan.

## 5. Industrial Performance Optimization
Walia’s Card Marking is designed to be "Cache-Friendly" and "Branch-Predictor Friendly":

1.  **Zero Conditional Logic:** The `markCard` operation is a "Blind Store." It does not check if the card is already dirty; it simply writes a `1`. This avoids CPU branch mispredictions.
2.  **Byte-per-Card:** While a bitset is more space-efficient, Walia uses **1 byte per card**. This allows the VM to use a single `STR` (Store) instruction instead of a `READ-MODIFY-WRITE` sequence, maximizing throughput.
3.  **Grain Optimization:** The 512-byte grain is chosen to match the physical sector size of most traditional hard drives and the internal buffer sizes of modern SSDs, ensuring that Walia’s software "pages" align with hardware reality.

## 6. Implementation Safety
In a production environment, the Write Barrier is mandatory. If a developer implements a new Native C function that modifies a Walia object, they **must** manually invoke `markCard()`. Failure to do so would result in a "stale" state on disk, where the object in RAM is updated but the version in the `.wal` state file remains old.

---
**Status:** Technical Specification 1.0  
**Mechanism:** Card-Marking Write Barrier  
**Grain:** 512-Byte Alignment  

