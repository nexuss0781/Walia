
# Tier III: The Data Sovereign
## Module 25: Bulk Operations and Transactions (The Shield of Atomicity)

### 1. The Necessity of All-or-Nothing
In any enterprise system, logic often requires multiple changes to happen simultaneously. If you are moving money from Account A to Account B, you cannot have the system crash after subtracting from A but before adding to B. 

Walia provides **Atomic Transactions** to ensure that your data territories remain consistent. A transaction guarantees that either every action in a block is physically committed to the disk, or nothing is.

### 2. Transactional Syntax
Walia utilizes three primary commands to manage the lifecycle of a data mutation:

*   **`db_transaction_begin()`**: Creates a "Sovereign Snapshot." From this point on, all your database changes are invisible to the rest of the world.
*   **`db_transaction_commit()`**: Atomically swaps the system pointers. All changes made since "Begin" become part of the immortal record instantly.
*   **`db_transaction_rollback()`**: Discards all changes made during the transaction, reverting the system to the exact state it was in at the "Begin" call.

### 3. The Shadow Paging Secret
How does Walia handle these massive updates without slowing down?
Instead of locking the database and making everyone wait, Walia uses **Shadow Paging**.

When you modify a record inside a transaction:
1.  Walia creates a "Ghost Copy" (Shadow Page) of the data in the persistent heap.
2.  Your logic works on the ghost copy.
3.  Other parts of your application continue reading the original "Sovereign" data.
4.  **The Commit:** Walia performs a single, atomic hardware operation to point the system to the new pages.

### 4. Practical Example: The Financial Transfer
This is the classic industrial test for transactional integrity.

```Walia
fun transfer_funds(from_id, to_id, amount) {
    // 1. Enter the Protected State
    db_transaction_begin();

    var sender = db_find_sql("Account", from_id);
    var receiver = db_find_sql("Account", to_id);

    if (sender.balance < amount) {
        print "Insufficient funds. Aborting.";
        // 2. Revert the universe if logic fails
        db_transaction_rollback();
        return false;
    }

    // Modify the data (These are currently 'Shadow' writes)
    sender.balance = sender.balance - amount;
    receiver.balance = receiver.balance + amount;

    db_update("Account", sender);
    db_update("Account", receiver);

    // 3. Make the changes permanent
    db_transaction_commit();
    print "Transfer successful and persistent.";
    return true;
}
```

### 5. High-Velocity Bulk Ingress
When you need to load millions of records (e.g., importing a historical log or an AI training set), standard one-by-one insertion is too slow. Walia provides the **Bulk Ingress Engine**.

By wrapping your loop in a transaction, you tell the **Sovereign Pager** to buffer the physical writes.

```Walia
db_transaction_begin();

var i = 0;
while (i < 10000) {
    var log = LogEntry();
    log.id = i;
    log.data = "Sensor Reading " + i;
    db_insert("LogEntry", log);
    i = i + 1;
}

db_transaction_commit(); 
// All 10,000 records are written in one hardware-aligned burst.
```

### 6. Massively Parallel Dispatch (MPP)
During a bulk commit, Walia utilizes its **Work-Stealing Dispatcher**. It partitions the 10,000 records into smaller groups and assigns them to different CPU cores. This allows the computer to saturate the total bandwidth of your storage hardware, achieving speeds that were previously only possible in specialized data centers.

### 7. Crash Recovery Guarantee
If the power is cut exactly in the middle of a `db_transaction_commit()`, what happens?
*   **The Sovereign Rule:** Because the "Commit" is a single atomic pointer-swap in the Superblock, the database will reboot into either the **Old State** or the **New State**. 
*   **Zero Corruption:** You will never find a "half-finished" transaction. Your data is physically protected by the architecture of the language.

---
**Next Step:** Protecting your data is not just about atomicity; it is about access. In the next module, we will explore **Security and RBAC**, learning how to define roles and permissions that are enforced at the hardware level.
