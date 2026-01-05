
# Tier VI: The Grand Convergence
## Module 90: Exhaustive Logic and the Sentry

### 1. The Trap of Unhandled States
In traditional software development, a common source of crashes is "unhandled cases." For example, a program might know how to handle "Order Processed" and "Order Failed," but if the system enters a new state like "Order Pending," the code might fail silently or crash. In an immortal, persistent system like Walia, an unhandled state can contaminate your database and create long-term corruption.

Walia prevents this through the **Exhaustive Sentry**. This compiler-level guardian ensures that your `match` blocks handle every possible physical outcome before the program is permitted to run.

### 2. The Exhaustiveness Principle
A `match` block is considered **Exhaustive** only if it covers the entire range of potential values for a given type.

*   **Enforcement:** The Walia compiler analyzes the "Domain" of the target value.
*   **Result:** If even one potential state is missing, the build fails.

### 3. Boolean Exhaustiveness
When matching against a `Bool`, you must handle both `true` and `false`.

```Walia
var is_active = true;

// This will COMPILE
match is_active {
    true  => print "Online",
    false => print "Offline"
}

// This will FAIL to build (Missing false)
// match is_active {
//    true => print "Online"
// }
```

### 4. Layout and Tagged Union Exhaustiveness
The Sentry is most powerful when working with **Unions** and **Layouts** (Phase 11). If you define a status byte with specific bit-patterns, the compiler ensures you have addressed every valid pattern.

```Walia
layout Message {
    var type: u8; // Range 0-255
}

fun handle_msg(m) {
    match m.type {
        0 => print "CONTROL",
        1 => print "DATA",
        2 => print "SIGNAL",
        _ => print "RESERVED" // The wildcard covers all other 253 possibilities
    }
}
```

**Industrial Tip:** Use the `_` (Wildcard) as a safety net. It represents "all other cases" and satisfies the Exhaustive Sentry for any type.

### 5. Neural Exhaustiveness
In a **Neural Match** (Module 89), exhaustiveness is handled by the **Recognition Threshold**.

*   **Logic:** Every neural match is implicitly exhaustive because if the similarity is below the threshold, the VM **must** have a path to follow.
*   **Requirement:** Any `match` block containing a `~` (similarity) pattern **must** include a default `_` branch.

```Walia
match biometric_scan {
    ~ owner_key => unlock(),
    _           => report_unknown() // Mandatory for neural logic
}
```

### 6. Semantic "Did You Mean?" (Phase 10.2 Integration)
The Exhaustive Sentry is integrated with the **Sovereign Cure** system. If you miss a case, the compiler doesn't just error—it helps you fix it.

**Example Error:**
```text
[CRITICAL] Logic Breach in 'auth.wal': Non-Exhaustive Match.
  >> Missing Case: '2' (SIGNAL)
  >> Suggested Cure: Add '2 => ...' or '_' fallback.
```

### 7. Performance: Pruning Unreachable Patterns
While ensuring you cover every case, the Sentry also identifies **Dead Logic**. If you have two patterns that overlap (e.g., matching `10` twice), the engine flags the second one as unreachable.

```Walia
match score {
    10 => print "A",
    10 => print "B", // WARNING: This logic path can never be reached.
    _  => print "C"
}
```

### 8. Practical: Proving the State Machine
Let's build a small state machine and watch the Sentry enforce our logical completeness.

1.  **Define the States:**
    ```Walia
    const STATE_IDLE = 0;
    const STATE_RUN  = 1;
    const STATE_STOP = 2;
    ```
2.  **Attempt an incomplete match:**
    ```Walia
    var current_state = STATE_IDLE;

    // TRY THIS: Omit 'STATE_STOP' and watch the build fail.
    match current_state {
        STATE_IDLE => print "Waiting...",
        STATE_RUN  => print "Executing...",
        _          => print "Recovering..." // This saves the build
    }
    ```

Notice how the Sentry forces you to think about the "Fallback" case. You are building logic that is mathematically guaranteed to have a destination for every possible input.

### 9. Summary
1.  **Exhaustive Sentry:** Ensures all possible states are handled in a `match` block.
2.  **Safety:** Prevents silent failures and database contamination from unhandled values.
3.  **The Wildcard (`_`):** The primary tool for satisfying the Sentry.
4.  **Neural Sentry:** Requires a default branch for all similarity searches.
5.  **Clean Logic:** Identifies and warns about unreachable logic paths.

---
**Next Step:** Logic is fast, and logic is safe. Now we must make it **Physical**. In the final module of the Cognitive Architecture group, we will explore **UFO-Grade Decision Trees**, learning the hardware secret of how Walia compiles complex `match` blocks into high-speed binary jump tables.
