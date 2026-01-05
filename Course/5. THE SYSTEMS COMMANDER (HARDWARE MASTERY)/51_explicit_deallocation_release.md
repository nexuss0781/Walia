
# Tier V: The Systems Commander
## Module 51: Explicit Deallocation - `release`

### 1. The Duty of the Commander
In standard programming, the system eventually cleans up after you. In Systems Mode, you are the absolute authority. If you claim territory using `alloc()`, you must eventually return it to the system when you are finished. 

Walia provides the **`release`** keyword for this purpose. It physically deallocates a block of memory from the persistent heap, making that space available for future logic. Mastering `release` is essential for building stable, long-running systems that do not exhaust the computer's resources.

### 2. The `release` Keyword
The `release` command takes a pointer (`ptr`) as its argument. It informs the Sovereign Allocator that the memory at that address is no longer needed.

*   **Syntax:** `release pointer_variable;`
*   **Context:** Like all memory manipulation, `release` must be executed within an **`unsafe`** block.

```Walia
unsafe {
    // 1. Claim 512 bytes
    var buffer: ptr = alloc(512);
    
    // 2. Perform your logic here...
    
    // 3. Return the memory to the substrate
    release buffer;
    
    print "Resource successfully returned to the heap.";
}
```

### 3. The Lifecycle of a Block
To manage memory professionally, you should follow the **Sovereign Lifecycle**:

1.  **Allocation:** Request only as much as you need.
2.  **Verification:** Ensure the pointer is not `nil`.
3.  **Execution:** Perform your high-speed logic.
4.  **Release:** Clean up as soon as the data is no longer required.

### 4. Coalescing: The Silent Efficiency
When you call `release`, the Walia engine doesn't just mark the block as empty. It performs **Coalescing**.
*   **Logic:** The engine checks if the neighbors of the freed block are also free.
*   **Action:** It merges them into a single, larger block.
*   **Benefit:** This prevents "Fragmentation," where your memory becomes full of small, useless gaps. It ensures your persistent storage remains organized and ready for massive data ingress.

### 5. Example 1: The Temporary Processor
Commonly, you need a large amount of memory only for a few seconds (e.g., during a data sort or network packet burst).

```Walia
fun process_burst(data_size) {
    unsafe {
        // Claim temporary workspace
        var temp_scratchpad = alloc(data_size);
        
        // ... heavy processing logic ...
        
        // Immediate cleanup ensures other threads can use this space
        release temp_scratchpad;
    }
}
```

### 6. Example 2: Safeguarding Against Exhaustion
In a persistent environment, failing to release memory is more dangerous than in standard languages because the "Leak" survives a reboot.

```Walia
unsafe {
    var i = 0;
    while (i < 100) {
        var b = alloc(1024);
        // If we forgot 'release b' here, 100KB would be lost forever 
        // in our .wld file until we manually purged it.
        
        release b; // This maintains a zero-waste substrate
        i = i + 1;
    }
}
```

### 7. Visualizing Release: The PageMap
To verify your cleanup logic, watch the **Command Nexus HUD**:
*   **PageMap:** When you call `release` on a large block, you will see segments of the grid turn from Cyan (Occupied) back to **Gray (Free)**.
*   **Telemetry:** The `GC_RECLAIM` metric will trigger a pulse, showing the exact number of bytes returned to the Sovereign Substrate.

### 8. The Danger Zone: Use-After-Release
Once you call `release buffer`, the variable `buffer` still holds the old memory address, but that memory no longer belongs to you.
*   **Rule:** Never try to read or write to a pointer after it has been released.
*   **Best Practice:** Set the variable to `nil` immediately after releasing it to prevent accidental access.

```Walia
unsafe {
    release buffer;
    buffer = nil; // The Sovereign Guard: prevents accidental use.
}
```

### 9. Summary
1.  **`release p;`:** Manually returns a block of memory to the heap.
2.  **Persistence:** The freed status is physically saved to the disk image.
3.  **Coalescing:** The engine merges adjacent free blocks to maximize density.
4.  **Responsibility:** Failure to release creates persistent leaks in your sovereign territory.

---
**Next Step:** To allocate exactly what you need, you must know how big things are. In the next module, we will explore **Memory Introspection**, learning to use the `sizeof` keyword to measure the physical dimensions of your data.
