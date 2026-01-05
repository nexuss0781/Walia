
# Tier II: The Architect
## Module 11: Classes - Anatomy of a Blueprint

### 1. From Chaos to Structure
Functions are powerful, but as your system grows, you need to group related data and behavior together. In Walia, the **Class** is the sovereign blueprint for creating Objects.

Unlike C++ (where classes are compile-time templates), a Walia Class is a **First-Class Sovereign Object**. It lives in the persistent heap, can be passed around, and can even be modified at runtime.

### 2. Defining a Class
A class is defined using the `class` keyword. It can contain **Fields** (Data) and **Methods** (Behavior).

```Walia
class Robot {
    // 1. Fields (Data)
    var name;
    var battery_level;

    // 2. Methods (Behavior)
    fun charge() {
        print "Charging...";
        // 'this' refers to the specific object instance
        this.battery_level = 100;
    }
}
```

### 3. Fields (Properties)
Fields are variables attached to an object.
*   **Declaration:** Inside the class body using `var`.
*   **Default Value:** If you don't initialize them, they default to `nil`.
*   **Access:** Use the dot operator: `my_robot.name`.

### 4. Methods and `this`
Methods are functions that belong to the class.
*   **The `this` Keyword:** Inside a method, `this` acts as a reference to the specific object (Instance) that is calling the method.
*   **Binding:** When you call `bot.charge()`, Walia automatically binds `this` to `bot`.

### 5. Persistent Identity
Because Classes are persistent objects, they have a unique identity in the `walia.registry`.

```Walia
print Robot; // Output: <class Robot>
```
If you store an instance of `Robot` in the database, Walia saves both the data (`name="RX-78"`) and a pointer to the `Robot` class. This ensures that when you load the data back, it still knows how to behave (it can still call `charge()`).

### 6. The 8-Byte Optimization
Walia optimizes class instances using **Hidden Classes (Shapes)**.
*   **Memory:** An instance does not store a hash map of property names.
*   **Speed:** It stores a flat array of values.
*   **Lookup:** The VM uses the Class/Shape to know that "battery_level" is at index 1. This makes property access nearly as fast as raw array access ($O(1)$).

---
**Next Step:** A blueprint is useless until you build something with it. In the next module, we will learn about **Initializers and Instantiation**—the birth of an object.
