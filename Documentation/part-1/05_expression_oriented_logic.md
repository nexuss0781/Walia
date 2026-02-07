# Expression-Oriented Logic

## 1. Everything is a Value
Walia is a strictly **Expression-Oriented** language. In traditional imperative languages, there is a hard distinction between "Statements" (which perform actions) and "Expressions" (which return values). In Walia, this distinction is collapsed. Almost every construct—including conditionals, loops, and code blocks—is an expression that yields a value.

## 2. Conditional Expressions
In Walia, `if` is not just a control flow tool; it is a value producer. This eliminates the need for a separate "ternary operator" (`a ? b : c`) and results in more concise, immutable logic.

```javascript
// Variable assignment directly from a conditional
var status = if (user_count > 100) "Heavy Load" else "Normal";

print status;
```

*   **Branch Result:** The value of the `if` expression is the value of the last expression in whichever branch was executed.
*   **Type Safety:** Both branches should ideally return compatible types to satisfy the Walia Structural Type checker.

## 3. Block Expressions
A block of code delimited by curly braces `{}` is itself an expression. The value of the block is the value of the **final expression** evaluated within it.

```javascript
var result = {
    var x = 10;
    var y = 20;
    x + y; // This is the 'result' of the block
};

print result; // Output: 30
```

*   **Scope Isolation:** Variables defined inside the block (`x`, `y`) are reclaimed at the end of the block, but the final value is "lifted" into the outer register.
*   **Immutability Helper:** This allows complex computations to be encapsulated into a single assignment without leaking temporary variables into the parent scope.

## 4. Loop Expressions
While loops primarily perform side effects, they also return a value in Walia. By default, a loop that completes returns the value of the last expression evaluated in the final iteration. If the loop never runs, it returns `nil`.

```javascript
var final_value = while (i < 5) {
    i = i + 1;
    i; // final_value becomes 5
};
```

## 5. Logical Short-Circuiting
Walia’s logical operators `and` and `or` are also expression-oriented. They do not just return a boolean `true` or `false`; they return the **actual value** of the operand that determined the result.

*   **OR (`or`):** Returns the first truthy operand.
    ```javascript
    var name = input_name or "Anonymous"; 
    ```
*   **AND (`and`):** Returns the second operand if the first is truthy, otherwise returns the first (falsy) operand.

## 6. Functional Composition
Because everything returns a value, Walia code can be composed in a highly functional manner. Deeply nested logic remains readable because the "flow of data" is always explicit.

*   **Zero-Waste Returns:** Since the Virtual Machine is register-based, "returning" a value from a block or an if-statement costs 0 CPU cycles. It is simply a matter of the VM looking at the result in the last used register.

---
**Status:** Enterprise Specification 1.0  
**Paradigm:** Expression-Oriented / Functional-Lite  

