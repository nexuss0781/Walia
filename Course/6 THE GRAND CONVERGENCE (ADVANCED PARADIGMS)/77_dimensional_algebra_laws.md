
# Tier VI: The Grand Convergence
## Module 77: Dimensional Algebra Laws

### 1. Mathematical Logic of the Universe
In standard math, the magnitude of a number is the only thing that changes. In **Sovereign Dimensional Algebra**, the "Identity" of the data changes as well. When you multiply a length by a length, you are not just getting a larger number; you are physically transforming **Space** into **Area**.

Walia’s compiler implements the three fundamental laws of dimensional calculus. These rules ensure that your logic always produces physically plausible results, preventing the "Dimension Drift" that causes standard simulations to fail.

### 2. The Law of Additive Equality
This is the most critical rule for system safety.
*   **The Rule:** You can only **Add** or **Subtract** two values if their SI-7 Dimensional Vectors are **Identical**.
*   **Inference:** The resulting value has the same units as the inputs.

```Walia
// Valid: [1,0,0...] + [1,0,0...]
var total_length = 5 <m> + 10 <m>; 

// INVALID: [1,0,0...] + [0,0,1...]
// var error = 5 <m> + 2 <s>; 
// RESULT: Sentry halt.
```

### 3. The Law of Multiplicative Combination
Multiplication and Division are the "Unit Engines" of Walia. They combine the SI-7 vectors to create new physical meanings.

#### I. Multiplication (Vector Addition)
When you multiply, the engine **Adds** the exponents of the SI-7 vectors.
*   `[L:1] * [M:1] = [L:1, M:1]` (Meter-Kilogram)
*   `[L:1] * [L:1] = [L:2]` (Meter-Squared / Area)

```Walia
var width: <m> = 10;
var height: <m> = 5;
var area = width * height; // Inferred as <m^2>
```

#### II. Division (Vector Subtraction)
When you divide, the engine **Subtracts** the exponents.
*   `[L:1] / [T:1] = [L:1, T:-1]` (Meter per Second)
*   `[L:2] / [L:1] = [L:1]` (Area / Width = Length)

```Walia
var distance: <m> = 100;
var time: <s> = 10;
var speed = distance / time; // Inferred as <m/s>
```

### 4. The Law of Scalar Invariance
A "Scalar" is a number with no units (a dimensionless value like `10`, `pi`, or `count`).
*   **Multiplication:** Multiplying a physical quantity by a scalar changes its size but **Preserves its Identity**.
*   **Addition:** Adding a scalar to a physical quantity is **Forbidden** (unless the physical quantity is also a scalar).

```Walia
var mass: <kg> = 50;

// Valid: Preserves <kg>
var double_mass = mass * 2; 

// INVALID: Blocks the logic
// var error = mass + 10; 
```

### 5. Advanced Rule: Transcendental Functions
In a 5th Generation system, transcendental functions (like `sin`, `cos`, `log`, `exp`) have a strict physical requirement.
*   **The Rule:** The argument to a transcendental function **must be dimensionless (a scalar)**.
*   **Why:** You cannot calculate the "Sine of 5 Meters." It has no physical meaning.

```Walia
var theta: <rad> = 3.14159; // Radians are technically dimensionless [0,0,0...]
var result = sin(theta); // Valid

// var error = sin(10 <m>); // RESULT: Sentry halt.
```

### 6. The "UFO-Grade" Advantage: JIT Pruning
Because these laws are enforced during the **MPPA Pass (Phase 10.2)**, the resulting bytecode is perfectly optimized. 
*   **Zero Branches:** The VM doesn't need to check units at runtime. 
*   **Pipelining:** The CPU can execute the math in a straight line, as the "Dimensional Safety" was already proven by the Sentry.

### 7. Practical: The Kinetic Energy Proof
Let's use the algebra laws to verify a standard physical formula: $E = 0.5 \cdot m \cdot v^2$.

1.  **Define Inputs:**
    ```Walia
    var m: <kg> = 100;
    var v: <m/s> = 10;
    ```
2.  **Calculate Step-by-Step:**
    *   `v * v` -> Result: `<m^2/s^2>`
    *   `m * (v * v)` -> Result: `<kg*m^2/s^2>` (The SI definition of a Joule)
    *   `0.5 * ...` -> Result: `<J>` (Scalar multiplication preserves identity)
3.  **The Result:**
    ```Walia
    var energy: <J> = 0.5 * m * (v * v);
    print energy; // Output: 5000 <J>
    ```

### 8. Summary
1.  **Addition/Subtraction:** Requires exact unit identity.
2.  **Multiplication:** Adds dimensional exponents.
3.  **Division:** Subtracts dimensional exponents.
4.  **Scalars:** Numbers with no units. preserve identity but cannot be added to physical data.
5.  **Scientific Integrity:** These laws ensure that your code is a mathematically accurate reflection of the physical universe.

---
**Next Step:** Rules are powerful, but mistakes still happen. In the final module of the Dimensional Sovereignty group, we will explore **Reality Validation and Errors**, learning how the Walia compiler identifies, reports, and helps you fix "Physical Impossibilities" in your code.