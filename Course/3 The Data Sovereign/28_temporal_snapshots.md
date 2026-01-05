
# Tier III: The Data Sovereign
## Module 28: Temporal Snapshots (Time-Travel Queries)

### 1. The Immortality of State
In traditional databases, when you update a record, the old data is physically overwritten or moved to a slow backup log. In Walia, data is never truly destroyed; it is **Shadowed**. 

Because Walia utilizes **Shadow Paging** (Module 25) for every transaction, the engine can keep a physical record of every previous state of the entire project. This enables **Temporal Sovereignty**—the ability to query your database as it existed at any specific moment in history.

### 2. The `db.at()` Primitive
To travel through time, you use the `db.at()` function. This returns a "Historical View" of the database.

```Walia
// Get the database as it looked on Jan 1st, 2025 (Unix timestamp)
var history = db.at(1735689600);

// Now, any query you perform on 'history' reads from the past
var old_user = history.User.find(u => u.id == 101);
```

### 3. $O(1)$ Time-Travel: No Backups Needed
Unlike traditional systems that require hours to "Restore from Backup," Walia time-travel is instantaneous.

*   **The Logic:** Walia maintains a **Snapshot Registry** on **Page 2** of the `.wld` file. 
*   **The Action:** When you call `db.at()`, the Virtual Machine simply switches its "Root Pointer" to a historical Shadow Table.
*   **Performance:** Mounting a historical view takes **< 1ms**, regardless of whether the database is 1MB or 100TB. 

### 4. Forensic Auditing: The "Why"
Time-travel is the ultimate tool for **Forensic Integrity**. It allows you to answer questions that are impossible in other languages:
*   "What did the AI know before it made this specific decision?"
*   "What was the balance of this account 5 seconds before the crash?"
*   "How did this relational graph look before the massive data import?"

### 5. Snapshot Isolation
When you are in a historical view via `db.at()`:
1.  **Read-Only:** You can query and analyze history, but you cannot change it. This preserves the "Immutability of the Past."
2.  **Concurrency:** You can have 100 threads querying 100 different points in time simultaneously. Because each thread has its own **Shadow Root**, they never interfere with each other or the "Present" state.

### 6. Managing Snapshot Retention
To prevent the `.wld` file from growing indefinitely, Walia provides **Pruning Policies**.
*   **Automatic:** Keep snapshots for the last 30 days.
*   **Manual:** Delete snapshots older than a specific Transaction ID.
*   **Sovereign:** Mark specific "Golden Snapshots" (e.g., Year-End Close) that are never deleted.

### 7. Practical: The "Undo" Recovery
Let's simulate a catastrophic logic error and its recovery.

1.  **Phase A: The Present (Valid State)**
    ```Walia
    var t1 = time_now(); // Save the current time
    db_save("Logs", "id_01", {"status": "GOOD"});
    ```
2.  **Phase B: The Disaster**
    ```Walia
    // A logic bug accidentally clears the data
    db_remove("Logs", "id_01"); 
    print db_find("Logs", "id_01"); // Output: nil
    ```
3.  **Phase C: The Recovery**
    ```Walia
    // We go back to 't1' using the time machine
    var past = db.at(t1);
    var recovered_data = past.Logs.find(l => l.id == "id_01");
    
    print recovered_data.status; // Output: GOOD
    
    // Now we can restore it to the present!
    db_save("Logs", "id_01", recovered_data);
    ```

You have successfully performed a "Physical Undo." You didn't just reverse a variable; you used the **Sovereign Substrate** to reach back through the timeline and retrieve a lost physical reality.

---
**Next Step:** We have mastered Relational Tables and Document Collections. But the true power of Walia lies in its ability to handle **Intelligence**. In the final module of Tier III, we will look at **Vector Integration**, the bridge that connects your data to the world of AI.
