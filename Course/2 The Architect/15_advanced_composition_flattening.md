
# Tier II: The Architect
## Module 15: Advanced Composition Flattening

### 1. The Dynamic Performance Problem
In standard dynamic languages (like JavaScript or Python), modifying an object at runtime (e.g., adding a method) is disastrous for performance. The engine has to switch the object into "Hash Map Mode" (slow dictionary lookup) because it can no longer predict the memory layout.

Walia refuses to compromise speed for flexibility.

### 2. The Shape Transition Tree
Walia tracks the structure of every object using a **Hidden Class** (called a Shape).
1.  **Shape A:** `User { name }`
2.  **Mixin:** `mixin(user, Logger)`
3.  **Transition:** Walia checks if a Shape exists for `User + Logger`.
    *   If yes: The object's pointer is updated to point to **Shape B**.
    *   If no: A new **Shape B** is created.

### 3. Method Inlining (Flattening)
When **Shape B** is created, the engine copies the method pointers from `User` and the method pointers from `Logger` into a single, flat array.

*   **Virtual Dispatch:** The VM sees `CALL Method #4`.
*   **Result:** It jumps directly to the function address. It does not scan a list of traits. It does not walk an inheritance chain.

### 4. Zero-Cost Abstraction
This means that a highly dynamic object with 10 traits mixed in runs at the **exact same speed** as a rigid C++ object.
*   **Cost of Mixin:** Paid once (at the moment of `mixin()`).
*   **Cost of Call:** Zero overhead.

### 5. Architectural Implication
This allows you to build systems that evolve. You can start with a simple `DataPoint` class and upgrade it into a `NeuralNode` with `Trainable` and `Serializable` traits as your application runs, without rewriting your core logic or sacrificing performance.

### 6. Persistence of Shapes
These specialized Shapes are stored in the `.state` file. If you create a unique `User + Logger + Admin` object shape, that shape is preserved. On reboot, the engine doesn't need to re-calculate the flattening; it simply loads the optimized structure from disk.

---
**Next Step:** We have mastered logic and structure. Now we must master **Truth**. In the next module, we enter the world of **Living Documentation** and the Walia Oracle.
