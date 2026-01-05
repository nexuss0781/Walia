# Variables and Orthogonal Persistence

## 1. Sovereign Variable Management
In Walia, variables are more than temporary storage locations; they are the fundamental units of **Sovereign State**. The language distinguishes between local transients and persistent globals, ensuring that critical data survives the lifecycle of the execution process.

## 2. Variable Declaration
The `var` keyword is the sole mechanism for introducing new identifiers into a scope.

```javascript
var identifier = expression;
```

*   **Initialization:** Variables may be initialized at declaration. If no initializer is provided, the variable defaults to `nil`.
*   **Late Binding:** Global variables support late binding, allowing scripts to reference functions or variables defined further down in the source file.

## 3. Lexical Scoping
Walia enforces strict lexical scoping using curly braces `{}` to define blocks.

*   **Block Scoping:** Variables declared inside a block are local to that block and its children.
*   **Shadowing:** A local variable can shadow a variable in an outer scope. However, the Walia Semantic Analyzer will issue a warning if shadowing appears to be accidental (DX Safety).
*   **Register Allocation:** The compiler automatically maps local variables to VM register slots. When a block ends, those slots are immediately marked for reuse, ensuring the stack footprint remains minimal.

## 4. Orthogonal Persistence (The State File)
The defining feature of the Walia language is **Orthogonal Persistence**. This means the "durability" of a variable is determined by its reachability, not by explicit save commands.

### I. The .state Heap Image
When the Walia VM initializes, it maps the `walia.state` file directly into its address space using `mmap`. 
*   **Memory-Mapped IO:** The OS treats the heap memory and the disk file as a single entity.
*   **Binary Imaging:** Data is stored on disk in the exact same binary format it uses in RAM. There is zero overhead for serialization (JSON/XML/SQL).

### II. Persistent Globals
Any variable declared at the top-level (outside of any function or block) is a **Persistent Global**.
*   **Immortality:** Once a global is assigned a value, that value is written to the persistent heap.
*   **Warm Resume:** If you stop the Walia process and restart it, the global variable table is restored instantly. The program "remembers" its previous state without a database.

### III. The Persistence Root
The VM maintains a "Global Root." Any object (List, Map, Closure) reachable from a global variable is considered live and is preserved in the `.state` file during Garbage Collection.

## 5. Checkpointing and Durability
To ensure data integrity, Walia utilizes an atomic checkpointing system.

*   **Card Marking:** The VM uses a "Write Barrier" to track which memory pages have been modified. 
*   **Syncing:** Upon a clean exit or a manual `checkpoint()` call, the VM performs an `msync`, flushing all "dirty" memory pages to the physical storage device.
*   **Atomic Swapping:** The Superblock (the header of the `.state` file) is updated only after a successful flush, ensuring that a power failure mid-execution does not result in a corrupted heap.

## 6. Developer Experience (DX) Benefits
Orthogonal Persistence eliminates the "Impedance Mismatch" between code and storage. 
*   **No ORM Required:** Since the objects themselves are persistent, there is no need for mapping layers.
*   **Simplified Logic:** Developers focus on manipulating variables, trusting that the Walia environment handles the storage.

---
**Status:** Enterprise Specification 1.0  
**Storage Target:** `walia.state` (Binary Heap Image)  

