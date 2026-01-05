v3.0.0 is only for the prime walia os focus solely about an os. Before diving to implementation first let's do analysis this include
- all things one primed stndard OS system should have as archtectural level the  followings
ok propose it but don't forget auto import and unique function feature in walia 

you can't assume specific core and archtect because i have seen 6 cores 4 cores also 2 cores but what if it has been run on single core if we get it should scale 
```
You need these core components. Build in this order.

Boot and startup

Bootloader. Loads your kernel from disk or flash.

Early init. Switch CPU mode, set stack, clear BSS.


Kernel core

CPU abstraction. Context switch, registers, traps.

Memory management. Paging, virtual memory, allocators.

Process and thread scheduler. Preemption, priorities.

System call interface. User to kernel boundary.

Interrupt and exception handling. Timers, faults.


Hardware support

Device drivers. Keyboard, display, storage, network.

Bus support. PCI, USB, MMIO.

Power management. Sleep, wake, CPU states.


Storage

Block device layer. Disks, flash.

File systems. VFS, ext-like or custom FS.

Caching and buffering. Page cache.


User space

Program loader. ELF or custom format.

Standard libraries. libc-like APIs.

Shell. Command execution, job control.

Core utilities. File, process, network tools.


Security and isolation

User and permission model. UID, GID, ACLs.

Memory isolation. User vs kernel space.

Capability or sandbox system.


Networking

Network stack. TCP/IP, UDP, sockets.

Drivers. Ethernet, Wi-Fi.

Firewall and routing.


Build and tooling

Cross compiler toolchain.

Linker scripts.

Build system.

Debugging tools. GDB, serial logs.


Optional but expected

SMP support. Multi-core scheduling.

GUI system. Window manager, compositor.

Package manager.

Update system.

Logging and diagnostics.
```

Next Unique features or unique Goals like we have primarily on CPU and Security.

we will build also unique file system we will change how data are stored to minimize memory and storage. we will have two parts;  How data is stored and data compression  we will talk more about this when we reach this.

also the below pdf also will be guide for our archtecture you will see it. i prepared 1 full version for operating system so v3.0.0 can be extended to comprehensive phase once our all of objectives festures and what we plan is fully finished it will be long milestone from booting uoto lanching our unique 5th generation os. so Analyze all of this 

the main thing is the os will automate many works and minimize overheads it wl many of cpu also utshould be ģ