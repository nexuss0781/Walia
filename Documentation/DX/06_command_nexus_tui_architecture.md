# Visual Sovereignty: The Command Nexus (TUI)

## 1. The Sovereign Cockpit
The **Walia Command Nexus (WCN)** is the high-fidelity, real-time Textual User Interface (TUI) that serves as the central cockpit for the Walia ecosystem. In a Walia environment, observability is not a separate log file; it is a cinematic, real-time visualization of the substrate’s pulse. Every action—from a millisecond file scan to a multi-trillion neuron similarity search—is rendered in a unified dashboard designed for "UFO-grade" operational transparency.

## 2. Rendering Engine: The ANSI Canvas
To ensure zero dependencies and maximum portability across Linux and Termux, the Command Nexus utilizes raw **ANSI Escape Sequences** instead of heavy graphics libraries or `ncurses`.

### I. Double-Buffered Architecture
To eliminate flickering and visual artifacts during high-speed updates (60fps), the Nexus implements a **Double-Buffered Rendering Pipeline**:
1.  **The Virtual Buffer:** All widgets draw their current state to a memory-resident `WldCanvas` structure.
2.  **Diff Optimization:** The engine compares the virtual buffer to the previous frame and identifies only the character cells that have changed.
3.  **Atomic Flush:** The changes are concatenated into a single string and written to `stdout` in one operation, minimizing the bandwidth required for remote SSH sessions.

### II. Dynamic Layout Reflow
The Nexus utilizes `ioctl` to monitor terminal dimensions. If a developer resizes their terminal window, the widgets automatically calculate new relative coordinates, ensuring the "Sovereign Cockpit" remains functional on any screen size.

## 3. The Sovereign Widget Suite
The HUD is composed of specialized visual modules, each representing a specific Walia "Party":

### I. The PageMap (Substrate Health)
A grid visualization of the persistent heap file.
*   **Cyan Cells ('S'):** Active SQL Table segments.
*   **Magenta Cells ('N'):** Active NoSQL Collection segments.
*   **Red Cells ('#'):** Dirty Shadow Pages (pending atomic commit).
*   **Gray Cells ('.'):** Free space available in the `.wld` or `.state` file.

### II. The NeuralGauge (AI Pulse)
A vertical telemetry bar monitoring the **Vector Wing**.
*   **Level Descent:** Visualizes the HNSW graph jumps from Level 16 down to Level 0.
*   **SIMD Pulse:** Flickers during AVX-512/NEON calculation cycles.
*   **Similarity Meter:** Real-time display of the current winning cosine similarity score.

### III. The HeapTank (GC Monitoring)
A dual-tank display showing the state of the "Reaper" (Garbage Collector).
*   **Nursery (Gen 0):** A rapidly filling and draining progress bar representing short-lived transients.
*   **Mature (Gen 1):** A slow-moving gauge representing long-term persistent state.

### IV. Execution Lanes (Parallelism)
Horizontal activity bars for every CPU core.
*   **Saturation:** Shows exactly which cores are busy with JIT compilation versus SIMD processing.
*   **Work-Stealing:** Animates the transfer of tasks between lanes, providing visual proof of MPP efficiency.

## 4. Performance Metrics (The HUD Header)
The top of the Nexus displays the "Sovereign Heartbeat":
*   **IPS:** Instructions Per Second.
*   **Throughput:** GB/s of data streaming through the Pager.
*   **Integrity:** Current health score from the **Ripple Engine**.

## 5. Non-Blocking Operation
The Command Nexus runs in a **Dedicated Background Thread**. 
*   **Logic Isolation:** The Walia Virtual Machine never waits for the UI to render. 
*   **Telemetry Buffer:** All visual data is fetched from the **Sovereign Event Bus** (Phase 10.3 Group 1). If the terminal is slow, the Nexus automatically samples the stream, ensuring that 100% of the CPU logic is focused on the developer's code.

## 6. Enterprise Utility: The Black Box
Because Walia is persistent, the Command Nexus can be configured as a "Black Box" recorder. It saves its TUI frames into a hidden segment of the persistent heap, allowing developers to "Replay" the visualization of a crash hours after it occurred.

---
**Status:** Enterprise Specification 1.1 (Implemented)  
**Interface:** ANSI-TUI (Zero-Dependency)  
**Refresh Rate:** 60 FPS Optimized
