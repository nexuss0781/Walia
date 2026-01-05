
# Tier II: The Architect
## Module 12: Initializers and Instantiation

### 1. The Birth of an Object
To turn a Class (Blueprint) into an Instance (Object), you "call" the class like a function.

```Walia
var my_bot = Robot();
```

However, a new object starts empty. Fields like `name` will be `nil`. We need a way to set up the object immediately upon creation. This is the job of the **Initializer**.

### 2. The `init` Method
Walia looks for a special method named `init`. If it exists, it is called automatically when the object is created.

```Walia
class Robot {
    var name;
    var battery;

    // The Constructor
    fun init(name, starting_battery) {
        this.name = name;
        this.battery = starting_battery;
        print "Robot " + this.name + " online.";
        
        // Implicitly returns 'this'
    }
}
```

### 3. Instantiation Syntax
Now, when we create the object, we pass arguments to the class call. These arguments are forwarded directly to `init`.

```Walia
var bot1 = Robot("Alpha", 100); // Output: Robot Alpha online.
var bot2 = Robot("Beta", 50);   // Output: Robot Beta online.
```

### 4. Safety Rule: Initialization
Walia guarantees that `init` always returns the new instance. Even if you try to `return` something else inside `init`, the VM ignores it and returns `this`. This prevents you from accidentally breaking the object creation chain.

### 5. Practical Example: A Database Record
Classes are often used to model data.

```Walia
class User {
    var id;
    var email;
    var is_active;

    fun init(id, email) {
        this.id = id;
        this.email = email;
        this.is_active = true; // Default value
    }
}

// Creating a new record
var new_user = User(101, "architect@walia.io");
```

### 6. Sovereign Persistence Note
When you instantiate `User(101, ...)`, the object is allocated in the heap. If `new_user` is a global variable, that object is instantly persistent. You have effectively "inserted a record" into the runtime memory database just by creating an instance.

---
**Next Step:** Classes can share behavior. In the next module, we will explore **Inheritance and Polymorphism**—how to create specialized versions of your blueprints.