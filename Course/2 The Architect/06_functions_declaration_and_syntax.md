
# Tier II: The Architect
## Module 06: Functions - Declaration and Syntax

### 1. The Core of Logic
In Walia, the **Function** is the fundamental unit of reusable logic. Unlike a script which runs top-to-bottom, a function encapsulates behavior that can be invoked on demand.

The keyword to define a function is `fun`.

### 2. The Basic Declaration
A function has a **Name**, a **Parameter List** (inputs), and a **Body** (logic).

```Walia
// Example 1: The simplest function
fun say_hello() {
    print "Hello, Sovereign Architect.";
}

// Invoking (Calling) the function
say_hello(); 
```

**Key Rule:** Function names must be globally unique in your project (Phase 10.1). You cannot have `say_hello` in two different files. This ensures you never have to guess *which* function you are calling.

### 3. Parameters and Arguments
Functions accept inputs called **Parameters**. You declare them inside the parentheses `()`.

```Walia
// Example 2: Accepting inputs
fun greet(name, title) {
    print "Welcome, " + title + " " + name;
}

greet("Jules", "Agent"); // Output: Welcome, Agent Jules
```

**Type Inference:** Notice we didn't specify types (`String`, `Number`). Walia infers them. However, if you are writing Systems code (Phase 11), you *can* enforce them: `fun add(a: i32, b: i32)`. For general logic, dynamic typing is preferred.

### 4. Return Values
Functions return data using the `return` keyword.

```Walia
// Example 3: Returning a value
fun add(a, b) {
    var result = a + b;
    return result;
}

var sum = add(10, 20); // sum becomes 30
```

### 5. Implicit Returns (Expression Bodies)
Walia is an **Expression-Oriented Language**. If a function body is a single expression (not a block with `{}`), it implicitly returns that value. This allows for extremely concise code.

```Walia
// Example 4: The concise syntax
// This function automatically returns a * b
fun multiply(a, b) a * b;

print multiply(5, 5); // 25
```

This is identical to:
```Walia
fun multiply(a, b) {
    return a * b;
}
```
Use the concise syntax for simple math or getters. Use the block syntax `{ ... }` for complex logic requiring variables and loops.

### 6. Summary of Syntax Rules
1.  **Keyword:** `fun` starts the declaration.
2.  **Name:** Must be a valid identifier (letters, numbers, underscores).
3.  **Parameters:** Comma-separated list inside `()`.
4.  **Body:** Either a block `{ ... }` or a single expression.
5.  **Scope:** Variables declared inside the function are **Local**. They die when the function returns.

---
