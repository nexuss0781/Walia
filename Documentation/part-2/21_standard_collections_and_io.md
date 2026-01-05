# Standard Collections and Sovereign I/O

## 1. Introduction
The Walia Standard Library provides high-performance, C-native data structures and I/O primitives. Unlike standard high-level languages where collections are implemented in the language itself, Walia’s core "Batteries" are integrated directly into the Virtual Machine substrate. This ensures that even the most complex data structures benefit from **NaN-Boxing efficiency**, **Zero-Copy access**, and **Orthogonal Persistence**.

## 2. Persistent Collections
All Walia collections are "Sovereign." They reside in the memory-mapped `.state` heap and are protected by the VM's internal Write Barrier.

### I. Persistent List (`List`)
The Walia `List` is a dynamic, 0-indexed array optimized for rapid append operations and random access.

*   **Internal Layout:** A contiguous block of NaN-boxed 64-bit values.
*   **Performance:** $O(1)$ amortized append; $O(1)$ access.
*   **API Functions:**
    *   `List()`: Returns a new persistent list.
    *   `list_add(list, value)`: Appends an item.
    *   `list_get(list, index)`: Retrieves an item; returns `nil` if out of bounds.
    *   `list_len(list)`: Returns the current element count.
    *   `list_pop(list)`: Removes and returns the final element.

### II. Persistent Map (`Map`)
The Walia `Map` is a high-speed associative array (Hash Map) using open addressing and linear probing.

*   **Internal Layout:** An interned string-to-value table.
*   **Deduplication:** Key lookups utilize pointer-equality of interned strings for maximum throughput.
*   **API Functions:**
    *   `Map()`: Returns a new persistent map.
    *   `map_set(map, key, value)`: Associates a value with a string key.
    *   `map_get(map, key)`: Retrieves the value; returns `nil` if missing.
    *   `map_del(map, key)`: Removes the entry from the persistent table.

## 3. Sovereign I/O Operations
Walia handles Input/Output through a "Safe Abstraction Layer" designed to work in harmony with **Algebraic Effects**. While raw I/O is performed in C, it is often wrapped in Walia scripts to provide resumable error handling.

### I. File System Access
Walia treats files as streams that can be managed within the persistent lifecycle.
*   **`io_read(path)`**: Reads the entire content of a file into a Walia string.
*   **`io_write(path, data)`**: Writes a string to a file.
*   **`io_exists(path)`**: Returns a Boolean indicating file presence.

### II. Networking (Basic)
Walia includes C-native wrappers for non-blocking network communication.
*   **`net_request(url)`**: A primitive for performing HTTP GET requests, typically used inside a `handle` block to manage network latency as an effect.

## 4. The Write Barrier Requirement
Because collections are persistent, every modification triggers a **Card Marking** event. When you call `map_set(my_map, "id", 101)`, the VM executes the following C-logic internally:

1.  Calculate the hash of `"id"`.
2.  Locate the slot in the persistent table.
3.  Update the 64-bit word.
4.  **`markCard(slot_address)`**: Signals the persistence engine to sync this segment during the next checkpoint.

This mechanism ensures that your `Map` of 10 million users remains consistent on disk without the developer ever calling a `save()` or `commit()` function.

## 5. Memory-Mapped Streams
For high-velocity data processing, Walia supports memory-mapped I/O. This allows a Walia script to treat a multi-gigabyte file on disk as if it were a simple `List` of bytes, leveraging the Operating System’s page cache for "UFO-grade" data throughput.

## 6. Usage Example: Persistent Registry
```javascript
// Initialization runs only on Cold Boot
var registry = registry or Map();

fun registerUser(name, data) {
    map_set(registry, name, data);
    print "User " + name + " persisted to sovereign heap.";
}

registerUser("Alpha", 1001);
// Even if the process terminates here, registry["Alpha"] is saved.
```

---
**Status:** Enterprise Specification 1.0  
**Backend:** C-Native Primitives  
**Persistence:** Write-Barrier Protected  

