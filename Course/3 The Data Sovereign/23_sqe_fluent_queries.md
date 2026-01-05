
# Tier III: The Data Sovereign
## Module 23: Fluent Queries (Lambda Unrolling)

### 1. The Death of the Query String
In traditional databases, you search for data by writing a text string (SQL): `"SELECT * FROM users WHERE age > 18"`. This is slow because the computer must parse the text, understand your intent, and then search. 

Walia introduces the **Sovereign Query Engine (SQE)**. Instead of strings, you use **Native Closures** (Functions) to filter your data. Your code doesn't "describe" the query; your code **IS** the query.

### 2. The Syntax: `find()` and the Arrow (`=>`)
To search a table, you call the `.find()` method on the database object and pass a small function called a **Predicate**.

```Walia
// Syntax: db.Table.find( variable => condition )
var adults = db.User.find(u => u.age >= 18);
```

**How it works:** 
1.  `u` represents a single record in the table.
2.  `=>` separates the record from the logic.
3.  `u.age >= 18` is the rule. 

### 3. The "Unrolling" Magic (UFO-Grade Performance)
You might think: "Is looping through every user with a function slow?" 
**In Walia, it is the fastest way to search.**

When the compiler sees `u => u.age >= 18`, it doesn't run that function one by one. It **Unrolls** the logic. It looks at the physical layout of the `User` class, finds the exact byte-offset of `age`, and uses a single hardware instruction to compare 16 rows at once using **SIMD**. 

This allows Walia to scan millions of records per millisecond—approaching the theoretical limit of your CPU's speed.

### 4. Example 1: Basic Numerical Filter
Imagine a gaming system where we need to find high-ranking players.

```Walia
@sql
class Player {
    var id;
    var rank;
}

// Finding all players with a rank higher than 500
var elite_players = db.Player.find(p => p.rank > 500);

print "Found " + elite_players.length() + " elite players.";
```

### 5. Example 2: String and Identity Matching
You can use standard comparison operators to find specific identities or categories.

```Walia
@sql
class Document {
    var id;
    var category;
    var is_archived;
}

// Finding all active reports
var active_reports = db.Document.find(d => d.category == "report" and d.is_archived == false);
```

### 6. Example 3: Complex Multi-Criteria Logic
Walia's query engine supports full logical expressions inside the closure, including `and`, `or`, and `!`.

```Walia
@sql
class Transaction {
    var id;
    var amount;
    var status;
}

// Find high-value pending or suspicious transactions
var alerts = db.Transaction.find(t => 
    (t.amount > 10000 and t.status == "pending") or 
    (t.status == "flagged")
);
```

### 7. Performance Rule: The Sentry Guard
When you write a query closure, the **Sovereign Sentry** validates it at compile-time.
*   If you try to query a field that doesn't exist (`u.height`), the compiler stops you.
*   This is "Type-Safe Querying." You can never have a "SQL Syntax Error" at runtime because the query is part of the language itself.

### 8. Practical: The Population Analyzer
Let's build a real-time query tool for a population dataset.

1.  **Initialize the Data:**
    ```Walia
    @sql
    class Citizen { var id; var age; var city; }
    
    // Assume 100,000 citizens are already in the .wld file
    ```
2.  **Execute the Parallel Scan:**
    ```Walia
    // This executes across all CPU cores simultaneously
    var target_group = db.Citizen.find(c => c.city == "Addis Ababa" and c.age > 21);

    print "Target demographic count: " + target_group.length();
    ```

Because Walia stores your data in its native binary format, the `target_group` list doesn't contain "copies" of the data. It contains **Direct Pointers** to the persistent memory, making the result set nearly instantaneous to process.

---
**Next Step:** Finding data in one table is powerful, but what if tables are connected? In the next module, we will master **Sovereign Pointer Joins**—how to navigate relationships between objects at the speed of a memory jump.
