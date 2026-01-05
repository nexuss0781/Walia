
# Tier III: The Data Sovereign
## Module 18: SQL Tables and Types

### 1. The Death of the "CREATE TABLE" Statement
In traditional programming, you have your code (classes), and you have your database (SQL scripts). You have to manually keep them in sync. If you change a class, you have to run an `ALTER TABLE` script.

Walia eliminates this separation. In Walia, **Your Class IS The Table.**

### 2. The `@sql` Decorator
To tell Walia that a class should be stored in the high-performance Relational Engine (B-Trees), you simply add the `@sql` decorator.

```Walia
@sql
class User {
    var id;
    var username;
    var email;
    var score;
}
```

**What happens:** When you compile this, Walia automatically allocates a Clustered B+ Tree in the `.wld` file. It maps the class properties to database columns.

### 3. Automatic Type Mapping
Walia infers the physical storage type from your variable usage or explicit type hints (Systems Mode).

| Walia Type | Physical Storage |
| :--- | :--- |
| `Number` (Integer) | `INT64` (8 bytes) |
| `Number` (Decimal) | `FLOAT64` (8 bytes) |
| `String` | `VARCHAR` (Variable width) |
| `Bool` | `BYTE` (1 byte) |
| `Vector` | `BLOB` (Binary Large Object) |

### 4. The Implicit Primary Key
Every Relational Table needs a Primary Key to index data efficiently.
*   **Rule:** Walia looks for a field named `id`. If found, it becomes the Primary Key.
*   **Performance:** Queries using `id` are instantaneous ($O(\log n)$) because they use the B-Tree index directly.

```Walia
@sql
class Product {
    var id; // PRIMARY KEY
    var price;
}
```

### 5. Creating Records
You don't write `INSERT INTO...`. You just create instances of the class.

```Walia
// This logic creates a record in the B-Tree
var new_user = User();
new_user.id = 101;
new_user.username = "SovereignCoder";
new_user.score = 5000;

// To persist it permanently to the table:
db_insert("User", new_user);
```

### 6. Strict Schema Enforcement
Because `@sql` tables are relational, they enforce structure.
*   If you try to assign a String to a Number field (`score`), the runtime will throw an error.
*   This ensures data integrity at the lowest level, critical for financial or enterprise applications.

---
**Next Step:** Not all data fits into neat tables. In the next module, we will explore **NoSQL Collections**—the flexible, schema-less storage engine for dynamic objects.
