# Visual Sovereignty: Telemetry Probes and Pulses

## 1. The Nervous System of Walia
Observability in Walia is implemented as a high-speed "Nervous System" that connects the low-level C-substrate to the cinematic Command Nexus HUD. To achieve total transparency without violating the "UFO-grade" performance requirement, Walia utilizes a specialized telemetry architecture based on **Surgical Probes** and a **Lock-Free Pulse Bus**.

This system ensures that every atomic action—from a single B-Tree split to a multi-trillion neuron SIMD calculation—is recorded with zero impact on the execution thread's critical path.

## 2. The Lock-Free Pulse Bus
At the core of Walia’s telemetry is a **Multi-Producer Single-Consumer (MPSC)** circular ring buffer. 

*   **Atomic Emission:** Subsystems (DB, VM, GC) emit "Pulses" using the `atomic_fetch_add` instruction. This allows 64+ CPU cores to report their activity simultaneously without ever waiting for a mutex or lock.
*   **Zero-Pressure Buffer:** The bus has a fixed capacity of 64,000 pulses. If the HUD falls behind, the buffer simply wraps around. This ensures that telemetry can **never** block or slow down the production logic.
*   **Cache-Line Optimization:** The `WldPulse` structure is exactly 16 bytes. Precisely four pulses fit within a standard 64-byte L1 cache line, maximizing the hardware's throughput during high-velocity bursts.

## 3. Surgical Probes (Instrumentation)
Instrumentation is performed via "Surgical Macros" injected directly into the core C source code. These probes are designed to be "Physical-First": they translate hardware events into visual telemetry.

### I. Data Probes (The Pager)
*   `PROBE_DB_READ(pageId)`: Emitted whenever the Pager maps a segment into RAM.
*   `PROBE_DB_WRITE(pageId)`: Emitted when the Shadow Pager creates a new Copy-on-Write version.
*   `PROBE_DB_COMMIT(txId)`: Emitted during the atomic superblock swap.

### II. Neural Probes (The Vector Wing)
*   `PROBE_NEURAL_HOP(nodeId, level)`: Captures the traversal path through the HNSW graph.
*   `PROBE_NEURAL_SIMD(count)`: Reports the number of dimensions processed in a single AVX-512/NEON cycle.

### III. Sovereign Probes (The Reaper)
*   `PROBE_GC_MARK(obj)`: Visualizes the reachability trace.
*   `PROBE_GC_RECLAIM(bytes)`: Reports the physical memory returned to the persistent heap.

## 4. The Pulse Data Packet
Every event in the Walia ecosystem is distilled into a compact 128-bit packet:

| Field | Bits | Role |
| :--- | :--- | :--- |
| **Type** | 32 | Identifies the subsystem (e.g., `PULSE_NEURAL_HOP`). |
| **TargetID** | 32 | The specific resource (e.g., `PageID` or `ThreadID`). |
| **Payload** | 64 | Micro-benchmarking data or state bitmasks. |

## 5. Adaptive Sampling (Handling Trillion-Scale Load)
In a multi-trillion neuron search, the engine may perform billions of similarity calculations per second. Standard logging would crash the terminal. Walia implements **Adaptive Sampling** at the bus level:

1.  **Threshold Detection:** If the write-index of the bus advances too rapidly, the HUD consumer automatically skips intermediate pulses.
2.  **Representative Aliasing:** The system ensures that even when sampling, "Critical Events" (like a GC cycle or a Commit) are always prioritized for rendering.
3.  **Result:** The developer sees a "Smooth Pulse" on the HUD that accurately represents the hardware's intensity without overflowing the terminal buffer.

## 6. Zero-Overhead "Dark Mode"
When the Command Nexus is not active (e.g., running in a background CI/CD pipeline), the probes resolve to a single atomic boolean check. 
*   **Cost:** ~1 CPU cycle.
*   **Impact:** Undetectable in standard benchmarks.

This allows Walia to keep its telemetry "Always-On" at the source code level, providing the ability to "Attach" a visual HUD to any running process at any time to diagnose performance drift in production.

---
**Status:** Enterprise Specification 1.1 (Implemented)  
**Bus Type:** Atomic Circular MPSC  
**Instrumentation:** Macro-Injected Probes
