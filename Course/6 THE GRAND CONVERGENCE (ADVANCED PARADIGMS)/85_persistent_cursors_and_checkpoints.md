
# Tier VI: The Grand Convergence
## Module 85: Persistent Cursors and Checkpoints

### 1. The Immortality of the Journey
In traditional data processing, if a stream processing 100 billion records is interrupted by a power failure, you usually have to start over from record #1. This is a massive waste of time and energy. 

Walia eliminates this waste through **Persistent Cursors**. In the Walia paradigm, a data pipeline is not just a transient event; it is a **Sovereign Journey**. Every Hyper-Pipe carries a hidden **Logical Cursor** that is etched into the persistent heap, ensuring that the system always remembers exactly how far it has traveled.

### 2. The Sovereign Cursor
A **Cursor** is a persistent coordinate consisting of a `PageID` and a `SlotIndex`. 

*   **Logic:** As data flows through a pipe (`|>`), the engine updates the cursor.
*   **Storage:** The cursor is stored in a dedicated metadata segment of the `.wld` file.

```Walia
// Example 1: Creating a resumable stream
var log_stream = db.Logs |> filter(l => l.level == "ERROR");

// This stream 'remembers' its place in the Logs table.
```

### 3. Atomic Checkpointing
To balance performance and safety, Walia uses **Checkpointing**. You can define how often the engine physically commits the cursor state to the disk.

*   **Syntax:** `stream.checkpoint_at(1000);`
*   **The Mechanism:** Every 1,000 records, the engine pauses for a microsecond and performs an atomic pointer swap in the **Sovereign Superblock**. 

```Walia
// Ensure progress is saved every 5,000 records
var secure_pipe = db.History 
               |> process() 
               |> checkpoint_at(5000);
```

### 4. Resumption Logic (The Warm Boot)
If the computer crashes at record #500,500:
1.  **Restart:** You relaunch Walia.
2.  **Recovery:** The engine looks at the **Persistent Cursor** in the `.wld` file.
3.  **Resume:** It sees that the last successful commit was at #500,000. 
4.  **Action:** The pipe automatically restarts from record #500,001.

**The Benefit:** You have 100% data integrity. Zero records are skipped, and zero records are processed twice (De-duplication by Architecture).

### 5. Managing Infinite Streams
Persistent cursors are the primary tool for managing **Real-Time Data Ingress** (e.g., a stock ticker or a sensor network).

```Walia
// A pipe that waits for new data and never stops
var live_feed = net_socket(8080) 
             |> json_parse() 
             |> db_insert("RawEvents");

// This pipe will resume automatically every time the server boots.
```

### 6. Visualizing the Cursor: The PageMap
To track your pipeline's progress, watch the **Command Nexus HUD**:
*   **PageMap Widget:** You will see a "Highlighter" cursor moving across the grid of data blocks. This is the physical visualization of your logic walking through the persistent heap.
*   **Telemetry:** The HUD displays the `REMAINING_ESTIMATE`, calculated by comparing the current cursor position to the total table size.

### 7. Practical: The Reliable Bulk-Updater
Let's build a script that updates 1 million records and proves its resilience.

1.  **Define the Pipe:**
    ```Walia
    var updater = db.Users 
               |> map(u => u.rank = u.rank + 1)
               |> checkpoint_at(10000);
    ```
2.  **Simulate Interruption:**
    Run the pipe. Force-close the terminal mid-process.
3.  **Verify Resumption:**
    Relaunch Walia and call `updater.run()`. 
    Notice that the **Command Nexus** instantly shows the cursor jumping to the last checkpoint, rather than starting from the beginning.

### 8. Summary
1.  **Persistent Cursor:** An immortal pointer that tracks stream progress.
2.  **Checkpoints:** Atomic commits that save the cursor state to disk.
3.  **Crash Recovery:** Automatic resumption from the last known-good state.
4.  **Zero-Redundancy:** Ensures every record is processed exactly once.

---
**Next Step:** Persistence is the safety; fusion is the speed. In the next module, we will explore **Kernel Fusion Optimization**, learning how the Walia compiler merges multiple pipe stages into a single machine-code loop to achieve 100% hardware efficiency.
