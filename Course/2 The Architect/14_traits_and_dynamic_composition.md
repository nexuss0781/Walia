
# Tier II: The Architect
## Module 14: Traits and Dynamic Composition

### 1. Beyond Inheritance
Inheritance is rigid. A `Car` cannot inherit from both `Vehicle` and `Logger`. This limitation often leads to complex, brittle class hierarchies.

Walia solves this with **Traits**. A Trait is a collection of methods that can be "mixed in" to any class or object. It represents a capability, not an identity.

### 2. Defining a Trait
Use the `trait` keyword. It looks like a class but cannot be instantiated directly.

```Walia
trait Logger {
    fun log(msg) {
        print "[LOG]: " + msg;
    }
}
```

### 3. Mixing into a Class
You can apply a trait to a class definition. Every instance of that class will have the trait's methods.

```Walia
class Server with Logger { // Syntax: 'with' keyword (Conceptual - Walia v1.2+)
    fun start() {
        this.log("Server Started"); // Using the trait!
    }
}
```

### 4. The Sovereign Power: Runtime Composition
Walia's unique feature is the ability to add traits to **Specific Objects** at runtime using the `walia_trait_apply` (often aliased as `mixin`) function.

```Walia
class User {
    var name;
    fun init(n) { this.name = n; }
}

var admin = User("Root");
var guest = User("Guest");

// Give the 'admin' object the ability to log, but not 'guest'
mixin(admin, Logger);

admin.log("I have power."); // Works!
// guest.log("Hello?");     // Error: Method 'log' not found.
```

### 5. Why is this useful?
This allows for **Entity-Component-System (ECS)** patterns without the boilerplate.
*   In a game, you can mix `Flyable` into a specific character when they pick up a jetpack.
*   In a server, you can mix `Debuggable` into a problematic request object to trace it, without slowing down the rest of the system.

### 6. The Flattening Magic
You might worry that adding methods at runtime makes the object slow.
*   **The Secret:** Walia uses **Shape Flattening**.
*   **The Result:** When you mix `Logger` into `admin`, the engine creates a new internal Shape that includes `log` directly in the method table. Calling `admin.log()` is just as fast ($O(1)$) as calling a normal method. There is no "list searching" overhead.

---
**Next Step:** To understand *why* this is so fast, we need to peek under the hood. In the next module, we will explore **Advanced Composition Flattening**—the architectural secret of Walia's speed.
