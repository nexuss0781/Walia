
# Lesson 04: Control Flow (The River of Thought)

## 1. Guiding the Logic
Control flow structures allow you to determine which parts of your code execute based on specific conditions. In Walia, control flow is **Expression-Oriented**. This means that structures like `if` don't just "do things"—they produce values that can be assigned to variables or passed to other functions.

Because Walia is a persistent environment, the state of your decisions is preserved. If a loop is running when the system restarts, Walia’s architecture is designed to handle the continuity of that logic.

## 2. Decisions: The `if` Expression
The `if` expression evaluates a condition. If the condition is `true`, it executes the first block. If `false`, it executes the optional `else` block.

```Walia
var temperature = 30;

var weather_report = if (temperature > 25) {
    "It is a warm day."
} else {
    "It is a cool day."
};

print weather_report; // Output: It is a warm day.
```

### The Expression Rule
In Walia, you can use `if` directly on the right side of an assignment. This eliminates the need for temporary variables and makes your code much cleaner.

```Walia
var access_level = 10;
var status = if (access_level > 5) "Admin" else "Guest";
```

## 3. Repetition: The `while` Loop
The `while` loop repeats a block of code as long as a condition remains `true`.

```Walia
var energy_level = 5;

while (energy_level > 0) {
    print "System is operational...";
    energy_level = energy_level - 1;
}

print "System entering hibernation.";
```

**Persistence Note:** If you define `energy_level` as a global variable, its value is saved to the persistent heap every time it changes. If the loop is interrupted, the data remains at its last known state.

## 4. The `match` Engine (Pattern Matching)
Walia introduces a high-performance decision engine called `match`. It is a more powerful version of the "switch" statement found in older languages. It allows you to branch logic based on exact values or complex patterns.

```Walia
var command = "start";

match command {
    "start" => print "Engine Initiated",
    "stop"  => print "Engine Halted",
    "reset" => {
        print "Clearing cache...";
        print "System Rebooted";
    },
    _ => print "Unknown Command" // The '_' acts as a default fallback
}
```

### Advanced Matching: Neural Similarity
As you progress to Tier IV, you will learn how `match` can even use the `~` operator to branch logic based on how "similar" a piece of data is to a target pattern—a key feature for building intelligent agents.

## 5. Scope in Control Flow
Variables created inside the curly braces `{ }` of an `if` or `while` block are **Local**. They cannot be accessed from outside that block. This ensures that your system’s memory remains organized and free of accidental interference.

```Walia
if (true) {
    var secret = 123;
}
// print secret; // ERROR: 'secret' does not exist here.
```

## 6. Practical: The Resilient Security Gate
Let's build a simple, persistent logic gate.

1.  Open the Walia terminal.
2.  Create a persistent variable to track attempts:
    ```Walia
    var attempts = 0;
    var max_attempts = 3;
    ```
3.  Write a logic block to check access:
    ```Walia
    var input_pin = 1234;
    var correct_pin = 1234;

    if (input_pin == correct_pin) {
        print "Access Granted";
        attempts = 0; // Reset persistent counter
    } else {
        attempts = attempts + 1;
        print "Access Denied";
    }
    ```
4.  Use `match` to report the system status:
    ```Walia
    match attempts {
        0 => print "System Secure",
        1 => print "Warning: 1 Failure",
        2 => print "Critical: Final Attempt",
        _ => print "System Locked"
    }
    ```

Because `attempts` is persistent, the "System Locked" status will remain even if you restart your computer. You have created a physical record of the security state.

---
**Next Step:** Proceed to **Lesson 05: Scoping and Blocks**, where we will master the rules of variable visibility and how to organize complex logic into "Sovereign Territories."