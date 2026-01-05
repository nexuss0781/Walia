# Generational Garbage Collection: The Sovereign Reaper

## 1. The Generational Hypothesis
The Walia Garbage Collector (GC) is built on the **Weak Generational Hypothesis**, which states that most objects "die young." In a high-performance environment, it is inefficient to scan the entire 128MB persistent heap every time memory is needed. Instead, Walia categorizes the heap into distinct generations to minimize latency and CPU overhead.

## 2. Heap Segmentation
Walia divides the persistent heap into three logical generations:

| Generation | Name | Allocation Strategy | Collection Frequency |
| :--- | :--- | :--- | :--- |
| **Gen 0** | **Nursery** | Fast Bump-Pointer | Very Frequent (Micro-pauses) |
| **Gen 1** | **Mature** | Freelist / Slot Reuse | Infrequent (Full Cycle) |
| **Gen 2** | **Static** | Immortal / Interned | Never (except on Shutdown) |

### I. The Nursery (Gen 0)
New objects (Strings, Lists, Closures) are born in the Nursery. 
*   **Speed:** Allocation is a single CPU `ADD` instruction (incrementing the `heapNext` pointer).
*   **Logic:** When the Nursery is full, the GC performs a "Minor Cycle." Objects that survive the Nursery are promoted to the Mature space.

### II. The Mature Space (Gen 1)
Long-lived objects (Global variables, persistent data structures) live here. 
*   **Logic:** Reclaimed using a **Non-Moving Mark-and-Sweep** algorithm. 
*   **Persistence Safety:** Unlike "Compacting" collectors that move objects to fill holes, Walia’s Mature GC is non-moving. This ensures that memory addresses saved in the `.state` file remain valid across sessions.

## 3. The Marking Phase (Tracing Reachability)
Walia utilizes a **Three-Color Abstraction** for marking objects during a GC cycle:

1.  **White:** Candidate for collection (not yet reached).
2.  **Gray:** Reached by the GC, but its children (references) haven't been scanned yet.
3.  **Black:** Reached and fully scanned. These objects are guaranteed to survive the cycle.

### The Gray Stack
To prevent C-level stack overflow when traversing deep data structures (e.g., a list with 1 million nodes), Walia uses an explicit **Gray Stack** resident in the persistent heap. This allows the GC to pause and resume marking without losing its progress.

## 4. The Sweep Phase (Persistent Reclamation)
Once marking is complete, every object remaining in the "White" state is considered garbage.
*   **Unlinking:** The object is removed from the VM's internal linked list.
*   **Slot Reuse:** The memory occupied by the object is returned to a **Freelist**. Future allocations in the Mature space will prioritize these "holes" before expanding the `.state` file, ensuring the database remains compact.

## 5. Cross-Generational Barriers
A common problem in generational GC is when a Mature object (Gen 1) points to a Nursery object (Gen 0).
*   **The Problem:** If we only scan the Nursery, we might accidentally delete an object that is still referenced by a Mature global.
*   **The Solution: Write Barriers.** Every time a reference is updated (e.g., `list_add(persistent_list, new_obj)`), Walia triggers the **Card Marking** system. The Nursery collector treats "Dirty" Mature cards as additional roots, ensuring no live data is ever lost.

## 6. GC Safepoints and Dispatch
Walia performs GC cycles at specific **Safepoints** within the instruction dispatch loop.
*   **Instruction Counting:** Every 10,000 instructions, the VM checks if the heap threshold has been reached.
*   **Allocation Triggers:** If `reallocate()` cannot find a 4KB block in the Nursery, a Minor Cycle is triggered immediately.

## 7. Performance Benchmarks
*   **Nursery Collection:** < 0.5ms (Imperceptible to users).
*   **Full Mature Cycle:** ~10ms per 100,000 objects.
*   **Persistence Impact:** By identifying "Dead State" early, the GC ensures that the atomic checkpoints flushes to disk only contain useful logic, keeping the `.state` image optimized for UFO-grade execution speeds.

---
**Status:** Technical Specification 1.0  
**Algorithm:** Generational Mark-and-Sweep  
**Philosophy:** Non-Moving Persistent Reclamation

