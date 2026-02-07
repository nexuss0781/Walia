# Standard Library Reference ("The Batteries")

## 1. Philosophy: Native Performance
The Walia Standard Library is built on the "Sovereign Battery" principle. Unlike many dynamic languages that implement their standard libraries in the language itself, Walia’s core collections and primitives are implemented in **Optimized C**. These are exposed to the VM as Native Functions, providing near-native execution speed while remaining fully integrated with Walia’s **Orthogonal Persistence** and **Garbage Collection**.

## 2. Core Globals

### `print(value)`
Outputs the string representation of a value to the standard output.
*   **Behavior:** Automatically handles all Walia types, including complex nested objects like Lists and Maps.
*   **Persistence:** Safe to call during any phase of execution.

### `clock()`
Returns the elapsed processor time in seconds as a Number.
*   **Usage:** Primary tool for benchmarking and high-resolution timing.

### `type_of(value)`
Returns a String representing the internal type of the provided value.
*   **Outputs:** `"Number"`, `"String"`, `"Boolean"`, `"Nil"`, `"Function"`, `"Closure"`, `"List"`, `"Map"`.

## 3. Persistent Collections
All standard collections in Walia are **Persistence-Aware**. When these objects are modified, the VM’s Write Barrier automatically marks the corresponding memory pages in the `.state` file for synchronization.

### `List`
A dynamic, 0-indexed array of values.
*   **`List()`**: Constructor. Returns a new, empty persistent list.
*   **`list_add(list, value)`**: Appends a value to the end of the list. O(1) amortized.
*   **`list_get(list, index)`**: Retrieves the value at the specified index. O(1).
*   **`list_len(list)`**: Returns the number of items in the list.

### `Map`
A high-performance associative array (Hash Map).
*   **`Map()`**: Constructor. Returns a new, empty persistent map.
*   **`map_set(map, key, value)`**: Inserts or updates the key with the provided value. O(1).
*   **`map_get(map, key)`**: Retrieves the value associated with the key. Returns `nil` if not found. O(1).
*   **`map_del(map, key)`**: Removes the key from the map.

## 4. Persistence Primitives
These functions allow the script to interact with the underlying Walia Sovereign Heap.

### `walia_checkpoint()`
Manually triggers an atomic flush of the current VM state to the `walia.state` file.
*   **Note:** While Walia performs automatic checkpoints on clean exit, this is used for ensuring durability during long-running processes.

### `walia_is_warm_boot()`
Returns `true` if the current session was resumed from an existing `.state` file, and `false` if the system performed a "Cold Boot" (fresh initialization).

## 5. Effect Primitives

### `resume(continuation, value)`
The mechanism used inside a `handle` block to return control to a suspended `perform` call.
*   **continuation:** The `ObjContinuation` captured by the VM.
*   **value:** The data to be injected back into the original call site.

## 6. Mathematical Operations
Standard arithmetic is handled via operators (`+`, `-`, `*`, `/`), but the library provides additional C-native math functions:
*   **`math_sqrt(n)`**, **`math_pow(base, exp)`**, **`math_abs(n)`**.

---
**Status:** Enterprise Specification 1.0  
**Implementation:** C-Native Standard Library
