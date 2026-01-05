# The Sovereign REPL and Time-Travel Diagnostics

## 1. The Interactive State Portal
The Walia REPL (Read-Eval-Print Loop) is not merely a transient scratchpad; it is an interactive portal to the **Sovereign Persistent Heap**. Unlike traditional interpreters where variables vanish upon session termination, the Walia REPL is **State-Aware**. By utilizing Orthogonal Persistence, every variable defined and every closure created during an interactive session is physically committed to the `walia.state` image.

This architecture transforms the REPL into a "Live Lab" where developers can build, test, and evolve multi-trillion neuron structures incrementally without ever losing progress.

## 2. REPL Meta-Commands
The REPL provides a set of dot-prefixed (`.`) commands for managing the sovereign environment with industrial precision.

| Command | Action | Description |
| :--- | :--- | :--- |
| `.exit` | **Checkpoint** | Flushes all session mutations to disk and performs an orderly shutdown. |
| `.undo [n]` | **Retroactive** | Reverts the last `n` instructions using the Time-Travel Trace Buffer. |
| `.state` | **Inspect** | Displays real-time statistics of the persistent heap (Bytes, Objects, GC health). |
| `.clear` | **Visual** | Purges the terminal buffer while maintaining the logical heap state. |

## 3. Deterministic Time-Travel (`.undo`)
The most advanced feature of the Walia REPL is its ability to perform **Retroactive Logic Correction**. If a developer enters a line of code that produces an undesired mutation in a persistent data structure, they can use the `.undo` command to "Rewind" the universe.

### I. The Mechanism: Mutation Logging
Walia achieves time travel through the **Sovereign Trace Buffer**.
1.  **Recording:** Before every register mutation (e.g., `OP_ADD` or `OP_SET_GLOBAL`), the VM records a `TraceEntry` containing the Instruction Pointer, the Target Register Index, and the **Previous Value** stored in that register.
2.  **Reversion:** When `.undo` is called, the VM pops entries from the circular trace buffer and restores the previous values to the registers, effectively "un-computing" the logic.

### II. UFO-Grade Fidelity
Because Walia stores raw 8-byte bit patterns, a `rewind` operation is a 100% faithful reconstruction of the past. There is no "approximation"—the bits in the persistent heap are restored to their exact state at that specific instruction tick.

## 4. Ghost Execution (The REPL Sentry)
To protect the sovereign heap from accidental corruption, the REPL utilizes **Ghost Execution** (Phase 10.4). 

*   **Logic:** When a developer hits `Enter`, the REPL first compiles the line and executes it in the **Volatile Sandbox**.
*   **Safety Check:** If the line triggers a `nil` dereference or an infinite loop, the REPL intercepts the fault, reports it via the **Sovereign HUD**, and refuses to commit the change to the persistent state.
*   **Commit:** Only if the Ghost Run succeeds is the logic executed on the live heap and saved to disk.

## 5. Visual Integration: The Nexus HUD
During a REPL session, the **Command Nexus** (Phase 10.3) provides a real-time heads-up display of the impact of each command.
*   **PageMap:** Developers can see the physical blocks of a `Map` or `List` being allocated in the `.wld` file as they type.
*   **NeuralGauge:** When experimenting with vector math, the SIMD throughput and HNSW hops are visualized instantly.
*   **Lanes:** Multi-threaded database scans triggered from the REPL are shown as active execution lanes, demonstrating Walia's parallel power in an interactive context.

## 6. Remote Attachment and Hot-Swapping
The Walia CLI supports "Attaching" a REPL session to a running production substrate.
*   **Sovereign Context:** A developer can log in to a production server via the REPL (`db.login`) and inspect live data structures.
*   **Live Patching:** Using the `walia_hot_swap` primitive from the REPL, a developer can update a function's logic in the **Persistent Method Table** while the system is running, achieving 100% uptime for critical AI services.

## 7. Operational DX Benefits
*   **Incremental AI Training:** Build and refine neural vectors one-by-one, knowing they are saved after every line.
*   **Safe Experimentation:** Use `.undo` as a "Safety Net" for risky database mutations.
*   **Contextual Discovery:** Use the REPL to query the **Sovereign Registry** and find where any function in the 60k+ file project is defined without leaving the shell.

---
**Status:** Enterprise Specification 1.1 (Implemented)  
**Interface:** Persistent Sovereign Shell  
**Capability:** Deterministic Time-Travel / Ghost-Guarded Commit

