# Getting Started with Walia

This guide provides the essential steps to initialize the Walia environment, compile the sovereign runtime, and execute your first persistent logic.

## 1. System Requirements
The Walia runtime is developed in C and is designed for POSIX-compliant systems (Linux, macOS, BSD). To build and run Walia, you require:
*   **C Compiler:** GCC 9+ or Clang 10+.
*   **Standard Library:** glibc or musl.
*   **Build Tool:** `make` (optional) or direct shell access.

## 2. Building the Walia Core
Walia is distributed as a set of high-performance C modules. To compile the executable, navigate to the source directory and run the following command:

```bash
gcc -o walia *.c -I. -O3 -Wall -Wextra
```

### Build Flags Explained:
*   `-O3`: Enables maximum optimizations, essential for the Computed Goto dispatch loop.
*   `-Wall -Wextra`: Enables industrial-strength warning levels to ensure code safety.
*   `-o walia`: Produces the final sovereign binary.

## 3. The Walia CLI
The Walia command-line interface is the primary entry point for executing scripts.

**Usage:**
```bash
./walia [filename.wal]
```

When you run a script, Walia automatically performs the following sequence:
1.  **Mounting:** It looks for a `walia.state` file. If found, it maps the persistent heap into the VM.
2.  **Compilation:** It parses the `.wal` source into 32-bit register bytecode.
3.  **Execution:** The VM executes the bytecode within the mapped heap.
4.  **Checkpointing:** Upon termination, it flushes the heap state back to the disk.

## 4. Your First Sovereign Script
Create a file named `hello.wal` and insert the following code:

```javascript
// hello.wal
var message = "Walia Sovereign Logic Initialized.";
print message;

var count = 0;
count = count + 1;
print "Current Execution Count:";
print count;
```

**Execute the script:**
```bash
./walia hello.wal
```

## 5. Understanding Persistence
Walia does not require a database to remember state. To witness **Orthogonal Persistence**, create `persist.wal`:

```javascript
// persist.wal
if (walia_is_warm_boot()) {
    print "Resuming from previous state...";
}

// Any variable in the global scope survives!
var session_id = "SID-" + clock();
print "Session ID created and persisted.";
```

Running this script multiple times will show that the global environment is preserved across sessions in the `walia.state` binary image.

## 6. Environment Cleanup
To reset the Walia environment and clear all persistent variables, simply delete the state file:
```bash
rm walia.state
```

---
**Status:** Enterprise Specification 1.0  
**Binary Target:** `walia` v1.0.0
