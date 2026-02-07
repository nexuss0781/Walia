# Functions and Closures

## 1. First-Class Functional Logic
In Walia, functions are first-class citizens. They are not merely subroutines; they are persistent objects that can be assigned to variables, passed as arguments to other functions, and returned as results. This functional foundation is essential for implementing Walia’s **Effect-Managed** architecture.

## 2. Function Declaration
Functions are defined using the `fun` keyword.

```javascript
fun add(a, b) {
    return a + b;
}

var sum = add(10, 20);
```

*   **Arity:** Functions have a fixed arity (number of parameters). The Walia VM enforces this at the call site to ensure runtime stability.
*   **Implicit Returns:** While the `return` keyword is available for early exits, a function will implicitly return the value of the last expression evaluated in its body (adhering to Walia’s expression-oriented philosophy).

## 3. Register Windowing (The Call Stack)
Walia utilizes a high-performance **Register-Based Virtual Machine**. Unlike stack-based VMs that push and pop values for every operation, Walia uses a "Sliding Window" on a continuous stack.

*   **Zero-Copy Argument Passing:** When a function is called, the VM slides its "window" so that the arguments are already sitting in the first registers of the new frame. No data is moved or copied.
*   **Call Frames:** Each function invocation creates a `CallFrame` that tracks the Instruction Pointer (`ip`) and the base register for that specific scope.

## 4. Closures and Upvalues
A closure is a function that "closes over" variables from its parent scope. In Walia, closures are persistent and survive beyond the execution of their parent function.

### I. Upvalues
When an inner function references a variable from an outer scope, Walia creates an **Upvalue**.
*   **Open Upvalues:** While the parent function is still running, the upvalue points directly to the variable's slot on the VM stack.
*   **Closed Upvalues:** When the parent function returns, the VM "hoists" the variable from the stack into the heap-resident closure object. 

### II. Persistent Environments
Because closures are heap objects, they are **Sovereign** and **Persistent**.
```javascript
fun createCounter() {
    var count = 0;
    return fun() {
        count = count + 1;
        return count;
    };
}

var myCounter = createCounter();
print myCounter(); // Output: 1
```
If the Walia process is terminated after the first call, `myCounter` and its captured `count` variable remain in the `walia.state` file. Upon restart, the next call to `myCounter()` will correctly return `2`.

## 5. Guaranteed Tail-Call Optimization (TCO)
Functional programming often relies on recursion. To prevent stack overflows, Walia provides **Guaranteed Tail-Call Optimization**.

*   **Stack Frame Reuse:** If the last action of a function is to return the result of another function call, the VM does not create a new stack frame. Instead, it reuses the current frame.
*   **Infinite Recursion:** This allows for deep, infinite recursion—essentially turning recursive calls into high-speed loops. This is a critical requirement for functional patterns that Python and standard JavaScript cannot support safely.

## 6. Native C-Bridge (FFI)
Walia supports the registration of Native C functions as first-class Walia objects.
*   **Zero-Overhead Calls:** Calling a C function from Walia uses the same register-window logic as internal functions.
*   **Standard Library:** All high-performance "Batteries" (like `clock`, `List`, and `Map`) are implemented as native C functions but behave exactly like standard Walia functions to the developer.

---
**Status:** Enterprise Specification 1.0  
**Architecture:** Register-Based / Sliding Window  

