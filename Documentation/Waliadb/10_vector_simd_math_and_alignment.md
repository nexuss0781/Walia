# Vector Wing: SIMD Math and Hardware Alignment

## 1. Hardware-Aligned AI Substrate
To achieve multi-trillion scale neural search, WaliaDB bypasses standard software abstractions and communicates directly with the CPU's mathematical units. The **Vector Wing** is built on the principle of **Hardware Alignment**, ensuring that every neuron in the sovereign heap is positioned for maximum throughput using **SIMD (Single Instruction, Multiple Data)** instructions.

By aligning neural data to 64-byte boundaries, WaliaDB enables the CPU to saturate the system's memory bandwidth, reaching the physical limits of hardware-level AI inference.

## 2. SIMD Memory Alignment (The 512-bit Rule)
Modern enterprise CPUs (Intel Xeon, AMD EPYC) utilize **AVX-512** instructions, which process 512 bits (16 single-precision floats) in a single clock cycle. If data is not "Aligned" to a 64-byte boundary, the CPU must perform extra work to fetch split segments, resulting in a 20-30% performance penalty.

### I. Aligned Allocation Logic
WaliaDB implements a custom persistent allocator (`allocateAligned`) that ensures every `ObjVector` is born at a memory address that is a multiple of 64.
*   **Mechanism:** The allocator requests $N + 64$ bytes from the Pager and uses a bitmask (`& ~63`) to find the nearest hardware-aligned offset.
*   **Persistent Consistency:** Because these alignments are preserved in the `.wld` file, the vectors remain "SIMD-Ready" across system restarts.

## 3. The SIMD Calculation Core
The Vector Wing implements distance metrics (Cosine Similarity, Euclidean Distance) using raw hardware intrinsics rather than standard C loops.

### I. Intel/AMD Path (AVX-512)
*   **Instruction:** `_mm512_fmadd_ps` (Fused Multiply-Add).
*   **Throughput:** Processes 16 dimensions per cycle.
*   **Performance:** Performs `(A * B) + Sum` as a single atomic CPU operation.

### II. ARM Path (NEON)
*   **Instruction:** `vfmaq_f32`.
*   **Throughput:** Processes 4 dimensions per cycle (128-bit).
*   **Sovereignty:** Ensures UFO-grade performance on ARM-based cloud instances (AWS Graviton) and Apple Silicon.

## 4. Dynamic Dispatch (Sovereign Optimization)
WaliaDB features an industry-standard **Dynamic Dispatch** mechanism. The engine does not hard-code a math path; it adapts to its environment.

1.  **Boot Detection:** During `initVM()`, the engine queries the CPU via `cpuid` or feature-bit headers.
2.  **Logic Swapping:** The VM hot-swaps the internal function pointers for `db_vector_dist`. 
3.  **Result:** The exact same Walia script runs with AVX-512 speed on a high-end server and gracefully falls back to NEON or optimized C on edge devices, always extracting the maximum possible speed from the local hardware.

## 5. Zero-Copy Neural Math
Unlike traditional AI databases that fetch vectors from disk into a "processing buffer," WaliaDB math occurs **In-Place**.

*   **Logic:** SIMD instructions operate directly on the memory-mapped pointers provided by the Sovereign Pager.
*   **UFO Speed:** The data streams from the SSD through the OS Page Cache directly into the CPU's ZMM/Neon registers. There is zero data marshaling, zero serialization, and zero intermediate memory allocation.

## 6. Performance Comparative

| Metric | Standard C Math | Walia SIMD (NEON) | Walia SIMD (AVX-512) |
| :--- | :--- | :--- | :--- |
| **Dimensions / Cycle** | 1 | 4 | **16** |
| **Instructions for Dot Product** | ~5 (Load, Mul, Add, Inc, Branch) | 2 (Vector Load, FMA) | **2 (Aligned Load, FMA)** |
| **Throughput (1k Dims)** | ~1,200 cycles | ~250 cycles | **~64 cycles** |

## 7. Security and Integrity
Even at SIMD speeds, the Vector Wing remains protected by the **Walia Sentry**.
*   **Bounds Guard:** The math core verifies the `dimensions` metadata in the `ObjVector` header before firing SIMD instructions, preventing buffer over-read vulnerabilities.
*   **Write Barriers:** Every update to a neuron triggers `markCard()`, ensuring that "Trained" weights are immediately persisted to the `.wld` image.

---
**Status:** Enterprise Specification 1.0 (Implemented)  
**Hardware Support:** AVX-512, AVX2, NEON  
**Alignment:** 64-byte (512-bit) Strict
