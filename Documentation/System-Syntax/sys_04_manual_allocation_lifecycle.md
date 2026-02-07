
# Manual Resource Mastery: The Sovereign Allocator

## 1. Escaping the Garbage Collector
The Walia Garbage Collector (GC) is excellent for business logic, but it introduces non-determinism. A systems engineer cannot tolerate a "Stop-the-World" pause while servicing a hardware interrupt or rendering a 60fps UI.

**Phase 11.5** introduces the **Sovereign Allocator**—a manual memory management subsystem that exists side-by-side with the managed heap. Memory allocated here is **Invisible to the GC**. It is yours to create, yours to use, and yours to destroy.

## 2. The Lifecycle Primitives

### Allocation (`alloc`)
Requests a contiguous block of raw memory from the persistent heap.
```javascript
// Request 1KB of raw storage
var buffer: ptr = alloc(1024);
```
*   **Persistent:** Unlike C's `malloc` which vanishes on reboot, `alloc` carves space from the memory-mapped `.wld` file. If you allocate a buffer and the power fails, the buffer exists upon restart (marked as "Allocated" in the Free List).
*   **Zero-Initialization:** For security, Walia guarantees the block is zeroed out before return.

### Release (`release`)
Returns the block to the Free List.
```javascript
release buffer;
```
*   **Immediate:** The memory is marked free instantly.
*   **Coalescing:** The allocator automatically merges the freed block with adjacent free blocks to prevent fragmentation.

## 3. The `sizeof` Operator
To allocate memory for complex structures, you need to know their exact physical size.
```javascript
layout Node {
    next: ptr;
    val:  u32;
}

// Dynamically allocate a Node struct
var node: ptr = alloc(sizeof(Node));
```

## 4. Arenas (Region-Based Management)
For high-performance scenarios (like a game loop or a web request handler), allocating and freeing individual objects is too slow. Walia supports **Arenas**.

```javascript
// Create a temporary scratchpad
var frame_arena = Arena(1024 * 1024); // 1MB

// Ultra-fast bump allocation inside the arena
var v1 = frame_arena.alloc(64);
var v2 = frame_arena.alloc(128);

// Instant cleanup
frame_arena.release_all();
```
*   **UFO Speed:** Arena allocation is just a pointer increment. Deallocation is setting the pointer back to zero. It is effectively free.

## 5. The Leak Sentry
Manual memory management introduces the risk of **Memory Leaks**. Walia mitigates this with the **Leak Sentry**.

*   **Tracking:** In Debug Mode, every `alloc` records the filename and line number in a hidden header.
*   **Reporting:** On program exit, the VM scans the manual heap. If any blocks are still allocated, it prints a report:
    ```text
    [LEAK DETECTED] 1024 bytes allocated at network.wal:45 were never released.
    ```

## 6. Best Practices: Hybrid Development
The power of Walia lies in mixing modes.
1.  Use **Managed Objects** (Strings, Maps) for configuration and high-level logic.
2.  Use **Manual Memory** (`alloc`) for large data buffers, image processing, and IO rings.
3.  Use **Arenas** for per-frame or per-request temporary data.

This hybrid approach gives you the safety of Python with the raw power of C, controllable line-by-line.

---
**Status:** Enterprise Specification 1.2  
**Algorithm:** Best-Fit Free List / Bump-Pointer Arenas  
**Persistence:** Durable Manual Heap