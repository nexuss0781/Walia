
# Tier II: The Architect
## Module 09: Persistence of Closures

### 1. The Immortal Function
In standard languages like JavaScript or Python, a closure lives in RAM. If the process dies, the closure and its captured variables are lost.

In Walia, closures obey the law of **Orthogonal Persistence**. If a closure is reachable from a persistent root (like a global variable), both the function logic and its captured Upvalues are automatically saved to the `.wld` heap.

This means you can create a "State Machine" using nothing but functions, and it will survive a system reboot perfectly intact.

### 2. The Persistence Test
Let's revisit our counter example, but this time, we will observe its behavior across a "Cold Boot" (simulated by exiting the REPL).

1.  **Session A (Create the State):**
    ```Walia
    // Define the factory
    fun make_persistent_counter() {
        var count = 0;
        return fun() {
            count = count + 1;
            return count;
        };
    }
    
    // Create the persistent closure
    var my_global_counter = make_persistent_counter();
    
    // Advance the state
    print my_global_counter(); // Output: 1
    print my_global_counter(); // Output: 2
    ```
    
2.  **Action:** Type `.exit` to close Walia. This flushes the memory to disk.

3.  **Session B (Resume the State):**
    Open Walia again.
    ```Walia
    // We do NOT redefine the function. It is already in the heap.
    print my_global_counter(); // Output: 3
    ```

**What happened?** The variable `count` was frozen inside `my_global_counter`. When you restarted, Walia re-mapped the closure, and `count` picked up exactly where it left off.

### 3. Why is this Revolutionary?
This feature eliminates the need for external databases for simple application state.
*   **Web Servers:** A "Session" object can just be a closure. No Redis needed.
*   **Game Logic:** An NPC's AI state can be a closure. No save-files needed.
*   **Workflows:** A multi-step approval process can be a closure that waits for days.

### 4. Technical Detail: How it Works
When Walia saves a closure to disk, it saves:
1.  The **Function Bytecode** (The logic).
2.  The **Upvalue Array** (The captured data).

If the Upvalue points to a Number, the number is saved. If it points to a large Object (like a List or another Closure), that object is also saved recursively.

### 5. Practical: The Long-Running Workflow
Imagine a task that takes days to complete.

```Walia
// Example: A 3-Step Approval Process
fun make_workflow() {
    var step = 0;
    return fun() {
        step = step + 1;
        if (step == 1) return "Manager Approved";
        if (step == 2) return "Director Approved";
        if (step == 3) return "VP Approved - Finalized";
        return "Already Done";
    };
}

var approval_process = make_workflow();
```
You can call `approval_process()` on Monday. The system can crash on Tuesday. You can call it again on Wednesday, and it will correctly output "Director Approved." The state is immortal.

---
**Next Step:** Closures allow us to maintain state. But what if we need to perform a loop that never ends, or process a list with infinite items? In the next module, we will master **Recursion and Tail-Call Optimization (TCO)** to write loops that are both infinite and safe.
