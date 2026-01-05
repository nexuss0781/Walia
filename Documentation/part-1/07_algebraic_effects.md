# Algebraic Effects and Resumable Continuations

## 1. The Effect-Managed Paradigm
Walia is an **Effect-Managed** language. While traditional languages rely on exceptions (`try/catch`) for non-local control flow, Walia utilizes **Algebraic Effects**. Unlike exceptions, which are destructive and unwind the stack, Algebraic Effects are **resumable**. They allow a program to pause, yield control to a handler, and then jump back exactly where it left off with a result.

## 2. Core Keywords: `perform`, `handle`, and `resume`

### I. `perform` (The Request)
The `perform` keyword is used to signal an effect. It pauses the current function and sends a request up the call stack for the nearest handler that can resolve it.

```javascript
fun fetchData(id) {
    print "Fetching data...";
    // Perform the "NetworkIO" effect and wait for the result
    var result = perform "NetworkIO"; 
    return "Data received: " + result;
}
```

### II. `handle` (The Provider)
The `handle` block defines how a specific effect should be resolved. It acts as a boundary for side effects.

```javascript
handle "NetworkIO" {
    var data = fetchData(101);
    print data;
}
```

### III. `resume` (The Continuation)
When an effect is caught by a `handle` block, the VM passes a **Continuation** object to the handler. The handler uses the `resume` statement to return a value to the point where the effect was performed.

```javascript
// A hypothetical system handler
handle "NetworkIO" {
    // 'cont' is the captured execution state of fetchData()
    var cont = ...; // Captured automatically
    resume(cont, "Status: 200 OK");
}
```

## 3. How it Works: Stack Slicing
Walia implements effects through a high-performance technique called **Stack Slicing**.

1.  **Capture:** When `perform` is called, the VM identifies the "slice" of the stack between the `perform` call and the `handle` block.
2.  **Lifting:** This slice (including call frames and registers) is lifted from the volatile stack and stored as an `ObjContinuation` on the persistent heap.
3.  **Transfer:** Execution jumps to the `handle` block.
4.  **Splice:** When `resume` is called, the slice is "spliced" back onto the live stack, and execution continues.

## 4. Persistent Continuations
Because Walia is a **Data-Sovereign** language, captured continuations are **Persistent**.

*   **State Snapshots:** If a continuation is captured and the system shuts down, the continuation remains in the `walia.state` file.
*   **Time-Independent Execution:** You can resume a continuation hours or days after it was captured. This allows for long-running workflows (e.g., waiting for human approval or an asynchronous external event) to be managed as simple function calls without an external state machine or database.

## 5. Benefits over Traditional Exceptions

| Feature | Exceptions (`try/catch`) | Algebraic Effects (`handle/perform`) |
| :--- | :--- | :--- |
| **Control Flow** | One-way (Destructive) | Two-way (Resumable) |
| **Context** | Lost during unwinding | Preserved in Continuation |
| **State** | Must be re-initialized | Remains exactly where it was |
| **Decoupling** | Hard-coded error paths | Complete separation of logic and effects |

## 6. Practical Use Cases
*   **Dependency Injection:** Instead of passing a "logger" or "database" object through 50 functions, just `perform "Log"`.
*   **Async/Await:** Walia implements asynchronous logic as a library of effects rather than a core language keyword.
*   **Mocking/Testing:** In a `test` block, you can provide a mock handler for an effect, allowing you to test complex business logic without actual network or disk I/O.

---
**Status:** Enterprise Specification 1.0  
**Mechanism:** First-Class Resumable Continuations  

