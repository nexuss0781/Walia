
# Tier III: The Data Sovereign
## Module 20: Data Integrity and Constraints

### 1. The Sovereign Sentry
In a persistent environment, the data you save today may live for decades. If you allow "garbage" data (incorrect types or duplicate IDs) to enter your system, that garbage becomes immortal. Walia prevents this through the **Logic Sentry**—a hardware-level validation layer that enforces the "Source of Truth" before any data is physically committed to the disk.

By using simple attributes, you tell Walia the rules of your universe, and the engine ensures they are never broken.

### 2. Enforcing Uniqueness: The `@unique` Attribute
Often, a property must be unique across the entire table. For example, two users cannot share the same email address, and two assets cannot share the same serial number.

```Walia
@sql
class Account {
    @unique
    var email;
    var balance;
}
```

**How it works:** When you attempt to insert an `Account`, the SQL Agency checks the B-Tree index. If the email already exists, the operation is blocked, and the system returns a **Constraint Violation**. This check happens at the binary level, making it incredibly fast.

### 3. High-Speed Lookups: The `@index` Attribute
By default, searching for an object by its `id` is $O(\log n)$. But what if you want to search for a user by their `username`? Without an index, the computer has to read every single row (a "Full Table Scan"), which is slow.

The `@index` attribute creates a secondary high-speed jump-map for that specific property.

```Walia
@sql
class Player {
    var id;
    @index
    var rank;
    var username;
}
```

**Industrial Benefit:** With `@index`, finding the highest-ranked players is instantaneous. The engine maintains a background B-Tree specifically for the `rank` field.

### 4. Relational Sentry: Type Integrity
Because Walia is integrated with the database, it enforces **Physical Type Safety**. If you define a field that stores Numbers, the Sentry physically prevents you from storing text there.

```Walia
@sql
class Inventory {
    var id;
    var quantity; // Walia expects a Number
}

var item = Inventory();
item.id = 500;
item.quantity = "Ten"; // ERROR: Logic Sentry blocks the write.
```

This prevents the "Dirty Data" problem that plagues traditional web development, where a bug in a script can corrupt an entire database.

### 5. Multi-Property Constraints
You can combine these rules to model complex industrial logic.

```Walia
@sql
class Device {
    @unique
    var serial_number;
    
    @index
    var manufacturer;
    
    var firmware_version;
}
```

### 6. Practical: The Sovereign Asset Registry
Let's build a secure registry for high-value items where data integrity is the primary goal.

1.  **Define the Logic:**
    ```Walia
    @sql
    class Asset {
        var id;
        
        @unique
        @index
        var registration_code;
        
        var owner_name;
        var value;
    }
    ```
2.  **Try to break the rules:**
    ```Walia
    var a1 = Asset();
    a1.id = 1;
    a1.registration_code = "GOLD_001";
    a1.owner_name = "Architect";
    db_insert("Asset", a1);

    var a2 = Asset();
    a2.id = 2;
    a2.registration_code = "GOLD_001"; // DUPLICATE CODE
    db_insert("Asset", a2); 
    // RESULT: Logic Sentry blocks this! a2 is never saved.
    ```

Notice how the language itself acts as the guardian of your data. You don't need to write complex "if-exists" checks; you simply define the **Sovereign Constraint**, and the engine handles the rest.

---
**Next Step:** Data is only as good as your ability to change it. In the next module, we will explore **Schema Evolution**—how to add new fields to your tables without losing your existing data or breaking the system.
