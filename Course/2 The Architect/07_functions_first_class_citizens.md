
# Tier II: The Architect
## Module 07: Functions as First-Class Citizens

### 1. What does "First-Class" mean?
In many older languages (like C or Java), functions are rigid structures. They sit in memory and wait to be called. In Walia, a function is a **Value**, just like a Number or a String.

*   You can store a function in a variable.
*   You can pass a function as an argument to another function.
*   You can return a function from another function.

This capability is what allows Walia to support powerful patterns like **Callbacks**, **Event Handlers**, and **Functional Pipelines**.

### 2. Storing Functions in Variables
Because functions are values, you can assign them to variables. This allows you to rename or alias logic dynamically.

```Walia
// Example 1: Assigning a function
fun bark() {
    print "Woof!";
}

var sound = bark; // Note: No parentheses. We are not calling it; we are moving it.

sound(); // Output: Woof!
```

**Sovereign Note:** If you assign a function to a global variable (e.g., `var my_logic = bark`), that assignment is **Persistent**. The link between the variable and the function survives a system reboot.

### 3. Passing Functions as Arguments (Callbacks)
You can write functions that accept *other* functions as inputs. This is essential for writing generic logic (like "Do this action 5 times").

```Walia
// Example 2: A generic repeater
fun repeat_action(times, action) {
    var i = 0;
    while (i < times) {
        action(); // Call the passed function
        i = i + 1;
    }
}

fun say_hi() {
    print "Hi!";
}

// Pass 'say_hi' into 'repeat_action'
repeat_action(3, say_hi);
```

### 4. Anonymous Functions (Lambdas)
Sometimes you don't want to name a function. You just want to define logic "in-place." Walia supports **Anonymous Functions** (often called Lambdas).

```Walia
// Example 3: Using a Lambda
repeat_action(2, fun() {
    print "I am anonymous!";
});
```

This is incredibly common in data processing.
```Walia
// Example 4: Lambda in a Pipeline
var numbers = List();
// ... add numbers ...
numbers |> map(fun(n) n * 2); // Multiplies every item by 2
```

### 5. Returning Functions
A function can create and return *another* function. This is the foundation of **Closures**, which we will explore deeply in the next module.

```Walia
// Example 5: A Function Factory
fun make_greeter() {
    return fun() {
        print "Greetings from the factory!";
    };
}

var my_greeter = make_greeter();
my_greeter();
```

---
**Next Step:** Now that we know functions can be moved around like data, we must understand what happens to the variables *inside* them when they move. In the next module, we will master **Closures and Upvalues**.
