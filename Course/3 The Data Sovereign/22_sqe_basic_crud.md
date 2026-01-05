
# Tier III: The Data Sovereign
## Module 22: High-Velocity CRUD (Create, Read, Update, Delete)

### 1. The Heartbeat of Data
In the Walia ecosystem, **CRUD** operations are the fundamental actions you perform on your persistent territories. Unlike standard languages that send text commands to an external database, Walia performs these actions as **Direct Binary Imaging**. 

When you "Insert" a record, you are physically moving 8-byte bit patterns into a memory-mapped page. This is why Walia achieves the physical bandwidth limit of your hardware—there is no translation, no parsing, and no waiting.

### 2. CREATE: Initializing Sovereignty
Depending on which engine you chose in Module 18 & 19, you use a specific command to create data.

#### I. SQL Insertion (`db_insert`)
Use this for classes decorated with `@sql`. It places the object into a **Clustered B+ Tree**.

```Walia
var user = User();
user.id = 101;
user.name = "Architect";

// Physically commit to the B-Tree
db_insert("User", user);
```

#### II. NoSQL Saving (`db_save`)
Use this for classes decorated with `@nosql`. It utilizes **Extendible Hashing** for immediate $O(1)$ placement.

```Walia
var config = Metadata();
db_save("Metadata", "theme_key", config);
```

### 3. READ: Instant Retrieval
Reading data in Walia is a **Direct Memory Jump**. The Virtual Machine uses the address provided by the database index to "Lift" the record into its registers.

```Walia
// SQL: Search by ID (Logarithmic speed)
var record = db_find_sql("User", 101);

// NoSQL: Search by Key (Constant speed)
var settings = db_find("Metadata", "theme_key");

print record.name;
```

### 4. UPDATE: The Shadow-Page Secret
In many databases, updating a record is slow because the system has to lock the file and overwrite bytes. Walia uses **Shadow Paging**.

*   **The Logic:** When you call an update, Walia creates a "Shadow Copy" of the data page. 
*   **The Benefit:** Readers can keep reading the old version while the writer is working. There is **Zero Lock Contention**. The update only becomes "Real" when the atomic pointer swap occurs in the Superblock.

```Walia
var user = db_find_sql("User", 101);
user.score = user.score + 500;

// Commits the new shadow page to the sovereign state
db_update("User", user);
```

### 5. DELETE: Sovereign Removal
Deleting data in Walia is more than just removing a reference. The engine physically reclaims the space in the `.wld` file so it can be reused for new records.

```Walia
// SQL Deletion
db_delete_sql("User", 101);

// NoSQL Deletion
db_remove("Metadata", "theme_key");
```

### 6. The "Zero-Serialization" Advantage
Why is this CRUD so much faster than other languages?
1.  **Traditional C#/Python:** Object -> JSON -> String -> Socket -> SQL Parser -> Disk.
2.  **Walia:** Object -> **Direct Memory Mapping** -> Disk.

Walia eliminates **5 out of 6 steps** in the data lifecycle. Your logic speaks the native language of the SSD.

### 7. Practical: The Persistent Inventory Cycle
Let's perform a full life-cycle of a high-value data asset.

1.  **Define:**
    ```Walia
    @sql
    class Product { var id; var price; }
    ```
2.  **Create:**
    ```Walia
    var p = Product();
    p.id = 50; p.price = 1000;
    db_insert("Product", p);
    ```
3.  **Read & Update:**
    ```Walia
    var item = db_find_sql("Product", 50);
    item.price = item.price * 0.9; // Apply 10% discount
    db_update("Product", item);
    ```
4.  **Verify & Delete:**
    ```Walia
    print db_find_sql("Product", 50).price; // 900.0
    db_delete_sql("Product", 50);
    ```

You have successfully manipulated the physical state of the computer. Every operation was atomic, persistent, and performed at hardware limits.

---
**Next Step:** CRUD is for individual records. But what if you need to find all users with a score over 1000? In the next module, we will explore **Fluent Queries** and see how Walia closures become high-speed machine-code filters.
