
# Tier V: The Systems Commander
## Module 50: The Sovereign Allocator - `alloc()`

### 1. Reclaiming Territory
In standard Walia logic, when you create a variable or an object, the engine automatically decides where to put it in memory. This is convenient but lacks the precision required for systems engineering. To build drivers, buffers, or custom data structures, you must be able to request a specific amount of raw physical space.

Walia provides the **Sovereign Allocator**, accessed via the `alloc()` keyword. This allows you to claim a contiguous block of bytes from the persistent heap that is entirely under your command.

### 2. The `alloc()` Keyword
The `alloc()` function requests a block of memory of a specific size. It returns a **Pointer** (`ptr`) to the start of that memory.

*   **Syntax:** `var my_buffer = alloc(size_in_bytes);`
*   **Context:** Because you are manually managing memory, `alloc()` must be called within an **`unsafe`** block.

```Walia
unsafe {
    // Request a raw block of 1024 bytes (1 KB)
    var data_block: ptr = alloc(1024);
    
    print "Allocated 1KB of Sovereign territory.";
}
```

### 3. The Advantage of Contiguity
Standard objects in many languages are scattered across RAM. Memory allocated via `alloc()` is **Contiguous** (one unbroken piece).
*   **Hardware Speed:** The CPU can read through contiguous memory much faster than scattered memory because it stays within the same cache lines.
*   **Predictability:** You know exactly where every byte is relative to the start of the block.

### 4. Immortality of Manual Blocks
A unique feature of the Walia Sovereign Allocator is that it is **Persistent**. 
When you call `alloc(1024)`, the engine carves that space out of the physical `.wld` file. 

*   **Persistence:** If you allocate a buffer and the computer loses power, that buffer still exists in your project's storage. 
*   **Re-Anchoring:** On restart, your `ptr` variable will still point to that exact physical block of data, allowing for instant resumption of complex I/O tasks.

### 5. Deterministic Memory
Memory allocated through `alloc()` is **Invisible to the Garbage Collector (GC)**. 
*   **Non-Blocking:** The "Reaper" (GC) will never pause your program to check on this memory. 
*   **Authority:** The memory remains yours until you explicitly give it back. This is essential for systems where even a millisecond of delay is unacceptable.

### 6. Example: The Persistent Data Buffer
Let's allocate a block of memory and verify its persistence across a session.

```Walia
// 1. Enter the systems mode
unsafe {
    // 2. Allocate 64 bytes (the size of one hardware cache line)
    var secure_buffer: ptr = alloc(64);
    
    // 3. We have claimed the memory. 
    // In the next modules, we will learn to write data into it.
    print "Memory address secured.";
}
```

### 7. Visualizing Allocation: The PageMap
To see the physical impact of your allocation, run Walia with the HUD:
*   **PageMap:** When you call `alloc()` for a large amount of memory, you will see new blocks light up in the grid.
*   **Heap Tanks:** Unlike standard variables, `alloc()` does not affect the `NURSERY` tank. It carves directly from the **Sovereign Substrate**, maintaining a stable memory profile for your managed code.

### 8. Summary
1.  **`alloc(n)`:** Requests `n` bytes of raw, contiguous memory.
2.  **Pointer Return:** It returns a `ptr` (physical address) to the territory.
3.  **Manual Control:** Bypasses the Garbage Collector for deterministic performance.
4.  **Persistent by Nature:** Manually allocated blocks are stored in the `.wld` heap and survive reboots.

---
**Next Step:** Claiming memory is the first half of the responsibility. To be a good commander, you must also know when to retreat. In the next module, we will learn about **Explicit Deallocation**, using the `release` keyword to return memory to the system.
