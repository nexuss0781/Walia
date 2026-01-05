
# Tier III: The Data Sovereign
## Module 19: NoSQL Collections and Flexibility

### 1. The Power of Dynamic Storage
While relational tables are excellent for structured data, many modern applications require the ability to store information that changes frequently or doesn't follow a rigid pattern. For this, Walia provides the **NoSQL Engine**. 

Instead of B-Trees, this engine uses **Extendible Hashing**. This ensures that finding any piece of data—no matter how large your collection grows—takes the exact same amount of time: **Constant Time ($O(1)$)**.

### 2. The `@nosql` Decorator
To designate a class for high-velocity, flexible storage, use the `@nosql` decorator.

```Walia
@nosql
class Metadata {
    var key;
    var value;
    var tags;
}
```

### 3. Schema-Less Freedom
Unlike SQL tables, NoSQL collections allow you to store objects with different sets of properties in the same collection. You can even store nested **Lists** and **Maps** directly.

```Walia
var config = Metadata();
config.key = "UI_THEME";
config.value = "Dark";
config.tags = List(); // Storing a list inside a record
config.tags.add("System");
config.tags.add("UserPreference");

// Save to the persistent collection
db_save("Metadata", config.key, config);
```

### 4. Direct Key-Value Lookups
The NoSQL engine is optimized for retrieving a specific object by its key. Because it uses hashing, it skips the "searching" process and jumps directly to the data's physical location.

```Walia
// Instant retrieval
var current_theme = db_find("Metadata", "UI_THEME");
print current_theme.value; // Output: Dark
```

### 5. Slotted Page Efficiency
Under the hood, Walia uses **Slotted Pages** to manage this data.
*   **Automatic Compaction:** If you delete or update an object, Walia automatically cleans up the "holes" in the storage file to keep your database small and fast.
*   **Variable Width:** You can store a tiny 10-byte object and a massive 1MB object in the same collection. Walia handles the resizing and overflow automatically.

### 6. Relational vs. Dynamic: The Decision
How do you choose between `@sql` and `@nosql`?

*   **Choose `@sql`** if your data is structured, needs to be ordered (e.g., sorted by date), or requires strict rules about what can be stored in each field.
*   **Choose `@nosql`** if you need the absolute fastest possible lookups for individual items, or if your objects have unpredictable properties (e.g., user settings or session logs).

### 7. Practical: The Persistent Log
Let's build a flexible event logger that remembers everything.

1.  Define the structure:
    ```Walia
    @nosql
    class Event {
        var id;
        var data;
    }
    ```
2.  Save different types of data:
    ```Walia
    var e1 = Event();
    e1.id = "boot_01";
    e1.data = "System check passed";

    var e2 = Event();
    e2.id = "user_01";
    e2.data = {"login_count": 5, "last_ip": "192.168.1.1"};

    db_save("Event", e1.id, e1);
    db_save("Event", e2.id, e2);
    ```

Even though `e1.data` is a String and `e2.data` is a Map, the NoSQL engine stores both perfectly. You have total freedom to evolve your data as your application grows.

---
**Next Step:** With freedom comes responsibility. In the next module, we will learn how to enforce **Data Integrity and Constraints**, ensuring that your sovereign data remains clean and valid even in a high-velocity environment.
