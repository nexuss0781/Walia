# Walia Oracle: Ghost Execution and Sandboxing

## 1. Safety-Critical Verification
In a persistent, sovereign environment like Walia, executing untrusted or unverified code—including documentation examples and unit tests—poses a significant risk to the integrity of the physical `.state` and `.wld` files. To mitigate this risk, Walia utilizes the **Ghost Executioner**, a specialized virtual sandbox that provides **High-Fidelity Simulation** with **Zero Persistent Impact**.

The Ghost Executioner allows the Oracle and the Healer to "Dry Run" logic at UFO speeds, ensuring that logical truth is verified without contaminating the sovereign truth.

## 2. The Volatile Sandbox Architecture
The Ghost Executioner is a stripped-down, high-speed variant of the Walia Virtual Machine, implemented within `tooling_simulator.c`. It replicates the execution semantics of the VM but redirects all side-effect-heavy opcodes.

### I. Stack Isolation
The sandbox utilizes a **Volatile Stack** allocated on the standard C-heap, rather than the memory-mapped sovereign stack. 
*   **Result:** Even if a ghost run overflows the stack, the persistent execution environment remains stable.
*   **Speed:** Memory access occurs at native L1/L2 cache speeds, bypassing the OS page-sync overhead.

### II. Opcodes Redirection (The Sovereign Bypass)
Crucial instructions that interact with the physical hardware are re-mapped in the Ghost engine:

| Opcode | Standard Behavior | **Ghost/Sandbox Behavior** |
| :--- | :--- | :--- |
| `OP_DEFINE_GLOBAL` | Writes to `.state` file. | Writes to a temporary volatile Map. |
| `OP_DB_COMMIT` | Atomic `msync` to disk. | NOP (No Operation). Returns Success. |
| `OP_PERFORM` | Captures resumable slice. | Replaced by a mock-effect handler. |
| `OP_SET_UPVALUE` | Modifies persistent closure. | Modifies a transient copy of the closure. |

## 3. Truth-Verification Lifecycle
When the **Oracle Runner** (Phase 10.4.2) identifies a doc-snippet, it initiates the following sandbox cycle:

1.  **Hydration:** The Ghost Engine mounts a **Snapshot of the Registry**. It "knows" what functions and classes exist, but it cannot change them.
2.  **JIT Execution:** The snippet is compiled into bytecode and dispatched to the Ghost loop.
3.  **Assertion Tracking:** Every `assert()` call within the snippet is monitored. If an assertion fails, the Ghost Executioner immediately traps the failure and captures the register state.
4.  **Resource Limiting:** To prevent infinite loops in documentation from hanging the build, the sandbox enforces a **Strict Instruction Cap** (Default: 1,000 instructions).

## 4. Copy-on-Write (CoW) Heaps
For more complex simulations (such as testing a database migration), Walia utilizes the OS kernel's **Copy-on-Write** mechanism.

*   **Logic:** The simulator `fork()`s the memory mapping of the `.wld` file.
*   **Isolation:** The test logic can perform billions of database writes. These writes only update the "Ghost Heap" in RAM. 
*   **Safety:** The original physical file on disk is protected by the kernel and remains 100% untouched.

## 5. Logic Fault Detection
Beyond verifying truth, the Ghost Executioner identifies "Three Killers" that often survive standard syntax checks:

### I. Nil-Drift Detection
The engine identifies variables that transition into a `nil` state and are subsequently used in math or logic operations, triggering a **Warning** in the Diagnostic Ledger before a production crash occurs.

### II. Recursion Depth Sentry
By monitoring the virtual `frameCount`, the sandbox detects recursive functions that lack base-cases or have insufficient stack-margins for multi-trillion parameter neural traversals.

### III. Logic Stagnation
The engine monitors the `OP_LOOP` and `OP_JUMP` frequency. If it detects a loop that performs zero state mutations (no `OP_ADD`, `OP_SET`, etc.), it flags the segment as an **Infinite Logic Fault**.

## 6. Enterprise Utility
Ghost Execution provides the Walia developer with a "Pre-Cognitive" debugger. By simulating logic during the build phase, Walia provides a mathematical guarantee that the code being committed to the persistent heap is physically safe and logically truthful.

---
**Status:** Enterprise Specification 1.1 (Implemented)  
**Engine:** Ghost Simulator (Volatile-Only)  
**Isolation:** Copy-on-Write / Volatile Stack
