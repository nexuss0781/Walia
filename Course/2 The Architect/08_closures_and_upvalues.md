
# Tier II: The Architect
## Module 08: Closures and Upvalues

### 1. The Memory of a Function
When you write a function inside another function, something special happens. The inner function can "see" the variables of the outer function. But what happens when the outer function finishes and returns? Do those variables die?

In Walia, the answer is **No**. The inner function "captures" the variables it needs. This combination of a function and its captured environment is called a **Closure**.

### 2. Conceptual Example: The Counter
Imagine you want a counter that remembers its count, but you don't want to use a global variable that anyone can mess with.

```Walia
// Example 1: The Counter Factory
fun make_counter() {
    var count = 0; // This variable is local to make_counter
    
    // This inner function uses 'count'
    return fun() {
        count = count + 1;
        return count;
    };
}

var counter_a = make_counter();
var counter_b = make_counter();

print counter_a(); // 1
print counter_a(); // 2
print counter_b(); // 1 (Totally independent!)
```

**The Logic:** Even though `make_counter` finished executing, the variable `count` is kept alive inside `counter_a`. It is now an **Upvalue**.

### 3. What is an Upvalue?
An **Upvalue** is a local variable that has been "lifted" from the stack to the heap because a closure refers to it.

*   **Stack Variable:** Lives only while the function runs. Fast, but transient.
*   **Upvalue:** Lives as long as the closure exists. Persistent and protected.

Walia manages this automatically. You simply write the code, and the compiler detects which variables need to be captured.

### 4. Practical Example: Configurable Logic
Closures are excellent for creating specialized functions from generic ones.

```Walia
// Example 2: A specialized adder
fun make_adder(amount) {
    return fun(n) n + amount;
}

var add_five = make_adder(5);
var add_ten  = make_adder(10);

print add_five(10); // 15
print add_ten(10);  // 20
```
Here, `amount` is the upvalue. `add_five` captures the value `5`, while `add_ten` captures `10`.

### 5. Sovereign Example: The Transaction Wrapper
In enterprise systems, closures are often used to wrap logic in safety or transaction handlers.

```Walia
// Example 3: Automatic Transaction Handling
fun with_transaction(database, logic) {
    print "Starting Transaction...";
    // We call the user's logic, passing the database
    logic(database);
    print "Committing Transaction...";
}

var db = "MyDatabase"; // Placeholder for real DB object

with_transaction(db, fun(d) {
    print "Writing to " + d;
    // Because this is a closure, we could also access
    // variables from outside this block if we wanted!
});
```

### 6. Why This Matters
Closures allow you to bundle **Data** (the upvalues) with **Behavior** (the code). This is the secret to building modular, secure systems without needing complex classes for everything. It is the heart of functional programming in Walia.

---
**Next Step:** We have seen how closures capture data in memory. But in Walia, memory is persistent. In the next module, we will explore the **Persistence of Closures**—what happens to your `counter_a` when you reboot the computer?
