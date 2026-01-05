# Tier II: The Architect
## Module 10: Recursion and Tail-Call Optimization (TCO)

### 1. The Recursive Concept
Recursion is when a function calls itself to solve a problem. It breaks a big task (like summing a list) into a smaller task (summing the first item + the rest of the list).

In many languages, recursion is dangerous. Every time a function calls itself, it consumes a "Stack Frame" (memory). If you call it too many times (e.g., 10,000), you get a **Stack Overflow Error**, and the program crashes.

Walia is different. It implements **Tail-Call Optimization (TCO)**.

### 2. What is TCO?
If the *very last thing* a function does is call itself (or another function), Walia realizes it doesn't need to keep the old stack frame. It simply reuses the current frame for the next call.

This transforms recursion from a memory-hungry chain into a highly efficient loop.

### 3. Conceptual Example: The Infinite Counter
Let's look at a recursive function that counts forever.

```Walia
// Without TCO, this would crash after ~10,000 calls.
fun count_forever(n) {
    // print n; // Uncomment to see it run
    
    // The recursive call is the LAST action.
    // Walia optimizes this into a 'JUMP' instruction.
    return count_forever(n + 1);
}
```
In Walia, you can run `count_forever(0)` and it will run until the heat death of the universe (or until you stop it), consuming zero extra memory.

### 4. Practical Example: List Processing
TCO is perfect for processing data without using `while` loops.

```Walia
// Summing a list recursively
fun sum_list(list, index, total) {
    // Base Case: We reached the end
    if (index == list.length()) {
        return total;
    }
    
    var item = list.get(index);
    
    // Recursive Step: Tail Call
    return sum_list(list, index + 1, total + item);
}

var numbers = List();
numbers.add(10);
numbers.add(20);
numbers.add(30);

print sum_list(numbers, 0, 0); // Output: 60
```
This style of coding is often cleaner and less error-prone than managing loop counters manually.

### 5. The "Stack Overflow" Trap (How to break TCO)
TCO only works if the call is in the **Tail Position**.

**Correct (TCO works):**
```Walia
fun good(n) {
    return good(n - 1);
}
```

**Incorrect (TCO fails):**
```Walia
fun bad(n) {
    // We have to multiply *after* the function returns.
    // This requires holding the stack frame open.
    return n * bad(n - 1); 
}
```
If you run `bad(100000)`, Walia will eventually throw a Stack Overflow error because it cannot optimize the memory.

### 6. Sovereign Example: The State Machine
State machines (like game logic or protocol parsers) are best written as mutually recursive functions.

```Walia
// State 1: Idle
fun state_idle() {
    print "System Idle. Waiting for input...";
    // Transition to active
    return state_active();
}

// State 2: Active
fun state_active() {
    print "System Active. Processing...";
    // Transition back to idle
    return state_idle();
}

// Start the machine
// state_idle(); 
```
This infinite loop of state transitions is perfectly safe in Walia. It behaves like an infinite `while` loop but is organized into clean, separate functions.

---
**Next Step:** We have mastered the functional side of Walia. Now it is time to structure our data and behavior into blueprints. In the next module, we begin our journey into **Object-Oriented Sovereignty** with **Classes**.
