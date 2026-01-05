
# Tier III: The Data Sovereign
## Module 21: Schema Evolution

### 1. The Challenge of Immortality
In standard programming, when you change a class definition, you simply restart the program and the old objects are gone. But in Walia, your objects are **Persistent**. If you have 1 million "User" objects stored in your system and you decide to add a new `phone_number` field to the class, what happens to the existing data?

**Schema Evolution** is the process by which Walia automatically adapts your persistent data to match your changing logic. It ensures that your system can grow and evolve without losing its history or requiring complex database migration scripts.

### 2. Logical Identity: The Sovereign Fingerprint
Every time you define a class with `@sql` or `@nosql`, Walia generates a unique **Logical Fingerprint** based on the properties and rules of that class.

*   **Recognition:** When Walia starts, it compares the fingerprint of the code you just wrote with the fingerprint stored in the data file (`.wld`).
*   **Drift Detection:** If they are different, Walia recognizes that the schema has "evolved."

### 3. Non-Destructive Expansion
The most common evolution is adding a new property. Walia handles this using **Lazy Hydration**.

```Walia
// Version 1 of your code
@sql
class User {
    var id;
    var name;
}

// Version 2: You add 'email'
@sql
class User {
    var id;
    var name;
    var email; // New Field
}
```

**What happens to old data:** 
When you read an old record that was saved during Version 1, Walia sees that the `email` field is missing in the physical storage. Instead of crashing, it automatically provides the value `nil` (or a default value) for that property. The next time you save that object, it is physically updated to the new format.

### 4. Logic Pruning: Removing Fields
If you remove a property from your class, Walia preserves the "Sovereign Integrity" of the data.

*   **Logic Side:** Your code can no longer see the field. 
*   **Storage Side:** The data remains in the file but is marked as "Shadowed." This ensures that if you accidentally deleted a field, you can add it back and the data will reappear, thanks to the **Temporal Sentry**.

### 5. Automated Migration Rules
Walia follows three industrial rules during evolution:
1.  **Order Preservation:** You can add fields anywhere in the class; Walia maps them based on their names, not their position.
2.  **Type Compatibility:** If you change a field from a Number to a String, Walia attempts a **Safe Cast**. If the cast is impossible, it flags a **Logic Drift Error** to prevent data corruption.
3.  **Index Rebuilding:** If you add an `@index` to an existing field, Walia automatically triggers a background scan to build the new high-speed jump-map for your old data.

### 6. Practical: The Evolving Profile
Let's simulate the evolution of a user profile system.

**Step 1: The Initial Launch**
```Walia
@sql
class Profile {
    var id;
    var bio;
}

var p1 = Profile();
p1.id = 1;
p1.bio = "Sovereign Architect";
db_insert("Profile", p1);
```

**Step 2: The Evolution (Modify your code)**
Change the class definition by adding a `rank` field:
```Walia
@sql
class Profile {
    var id;
    var bio;
    var rank; // Added later
}
```

**Step 3: Accessing Old Data**
```Walia
var old_p = db_find_sql("Profile", 1);
print old_p.bio;  // Output: Sovereign Architect
print old_p.rank; // Output: nil (Walia handled the missing field safely)

// Update the record to the new schema
old_p.rank = 10;
db_update("Profile", old_p);
```

### 7. Industry Benefit: 100% Uptime
In traditional systems, "Migrating the Database" often requires taking the application offline. Because Walia handles evolution **Lazily** (at the moment the data is accessed), your system stays online 100% of the time. The data "Heals" itself as it flows through your logic.

---
**Next Step:** Now that we know how to define our data territories, we must learn how to manipulate them with precision. In the next module, we will explore **High-Velocity CRUD**, mastering the commands to Insert, Update, and Delete data at the physical limit of your hardware.
