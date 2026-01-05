
# Tier II: The Architect
## Module 16: The Oracle - Syntax and Contracts

### 1. The Death of Stale Docs
Have you ever read documentation that said a function did one thing, but the code did another? That is "Documentation Rot." Walia eliminates this by turning documentation into **Executable Contracts**.

The **Walia Oracle** is a subsystem that reads your comments, compiles them, and runs them. If the comment fails, your project fails to build.

### 2. The Triple-Slash (`///`)
Standard comments (`//`) are ignored. Oracle comments start with `///`.
This signals to the compiler: "The text that follows is Law."

### 3. Writing a Contract
A contract consists of a description and a code block tagged with `example:`.

```Walia
/// example: Basic Addition
///   var result = add(2, 3);
///   assert(result == 5);
fun add(a, b) {
    return a + b;
}
```

### 4. The Rules of the Oracle
1.  **Placement:** The `///` block must appear immediately before the `fun` or `class` it describes.
2.  **Context:** The example code automatically has access to the function being documented. You don't need to import it.
3.  **Assertions:** You use `assert(condition)` to define the expected behavior. If `assert` returns `false`, the build breaks.

### 5. Advanced Contracts: State
You can write complex examples that set up variables and test state changes.

```Walia
/// example: Counter Logic
///   var c = make_counter();
///   c();
///   var val = c();
///   assert(val == 2);
fun make_counter() { ... }
```

### 6. Why "Contracts"?
This isn't just about testing; it's about **Trust**. When you download a Walia library, you don't need to wonder if it works. The fact that it compiled *proves* that every example in its manual is true.

---
**Next Step:** You know how to write the contracts. But what happens when you break them? In the final module of Tier II, we will look at **Oracle Workflow and Enforcement**.
