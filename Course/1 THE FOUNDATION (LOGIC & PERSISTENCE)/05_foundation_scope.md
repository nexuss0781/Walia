
# Lesson 05: Scoping and Blocks (Sovereign Territories)

## 1. Defining Logical Territories
In Walia, **Scope** refers to the visibility and lifetime of your variables. Understanding scope is the difference between a disorganized script and a professional, industrial-grade application. Because Walia is a persistent language, its scoping rules are unique: they don't just determine where a variable can be seen, but where it is physically stored in the computer's persistent memory.

## 2. Global Scope: The Sovereign Registry
Any variable declared at the very top level of a Walia file is in the **Global Scope**. 

*   **Identity:** These variables are registered in the project's **Global Registry**.
*   **Persistence:** They are physically mapped to the persistent heap. If you define a global variable, it is essentially a "record" in your system's long-term memory.
*   **Visibility:** They can be accessed from any function or block within your project without needing an `import` statement.

```Walia
var global_config = "Default"; // Persistent, Global, Immortal.

fun update_system() {
    print global_config; // Perfectly valid access.
}
```

## 3. Local Scope: The Transient Block
A **Local Scope** is created whenever you use curly braces `{ }`. Variables created inside these braces are **Transient**.

*   **Lifetime:** They are born when the code enters the block and die when the code hits the closing `}`.
*   **Efficiency:** Walia optimizes local variables to live directly in the CPU's registers, ensuring they are processed at the absolute physical speed of the processor.
*   **Isolation:** A local variable cannot be seen or modified by logic outside of its braces.

```Walia
var x = 100; // Global

{
    var y = 200; // Local to this block
    print x + y; // 300
}

// print y; // ERROR: 'y' does not exist in this scope.
```

## 4. The Expression-Oriented Block
One of Walia's most powerful features is that a block `{ }` is itself an expression. It returns the value of the very last line of code inside the braces.

```Walia
var result = {
    var a = 5;
    var b = 10;
    a * b; // This is the final value of the block
};

print result; // Output: 50
```

## 5. Shadowing: Safe Redefinition
Walia allows you to declare a variable in an inner scope that has the same name as one in an outer scope. This is called **Shadowing**. This is useful for temporary transformations without changing the original value.

```Walia
var data = "ORIGINAL";

{
    var data = "TRANSFORMED"; // Shadows the outer 'data'
    print data; // Output: TRANSFORMED
}

print data; // Output: ORIGINAL (The outer variable is unchanged)
```

## 6. Visualizing Scope: The Command Nexus HUD
When you run your code with the `--nexus` flag, you can actually see the difference between Scopes in the **Visual HUD**:

*   **Global Writes:** When you modify a global variable, you will see a flicker in the **PageMap** widget. This is the persistent heap being updated.
*   **Local Logic:** When you execute local variables, you will see activity in the **Execution Lanes**. This shows the CPU registers handling the transient data without involving the storage disk.

## 7. Practical: The Secure Session Logic
Let's build a logic structure that demonstrates the interaction between persistent global state and transient local processing.

1.  Open the Walia terminal.
2.  Define the global system state:
    ```Walia
    var system_uptime = 0;
    var active_session = false;
    ```
3.  Create a block to simulate a user session:
    ```Walia
    {
        // Transient local variables for the session
        var session_key = "SECURE_TOKEN_99";
        var local_timer = 0;

        active_session = true; // Modifying global state
        print "Session started with key: " + session_key;

        while (local_timer < 5) {
            local_timer = local_timer + 1;
            system_uptime = system_uptime + 1; // Updating global persistent clock
        }
        
        // Block ends here; session_key and local_timer are physically deleted from RAM
    }
    ```
4.  Verify the Global Persistence:
    ```Walia
    print system_uptime;   // 5
    print active_session;   // true
    // print session_key;    // Error: Cannot access local transient
    ```

You have successfully managed a multi-level scope where logic was processed at high speed in local registers while the important results were preserved forever in the global registry.

---
**Next Step:** This concludes **Tier I: The Foundation**. You have mastered the mindset of persistence, the atoms of data, and the river of control flow. 

Proceed to **Tier II: The Architect**, where we will learn to build reusable blueprints using Functions, Classes, and the "Truth-or-Death" documentation contract.