# CLI Usage and Command Flags

## 1. The Walia Sovereign Runner
The `walia` executable is the singular interface for the Walia ecosystem. It serves as the compiler, the virtual machine runner, the test orchestrator, and the persistence manager. Designed for high-reliability environments, the CLI provides deterministic control over the sovereign heap and execution parameters.

## 2. Basic Command Syntax
The primary method of execution involves passing a `.wal` source file to the binary.

```bash
./walia [options] <script.wal>
```

*   **File Extension:** The runner strictly enforces the `.wal` extension to differentiate between sovereign source code and generic text files.
*   **Automatic Mounting:** Upon execution, the CLI automatically attempts to mount the `walia.state` file in the current working directory.

## 3. Operational Modes

### I. Production Mode (Default)
Executes the script normally, honoring Orthogonal Persistence. `test` blocks are ignored by the compiler, and debug diagnostics are suppressed to maximize throughput.
```bash
./walia app.wal
```

### II. Integrated Test Mode
Activates the internal test runner. The compiler generates bytecode for all `test` blocks, and the VM executes them in isolation.
```bash
./walia --test app.wal
```

## 4. Primary Flags

| Flag | Short | Description |
| :--- | :--- | :--- |
| `--test` | `-t` | Runs the internal test suite defined in `test` blocks. |
| `--telemetry` | `-m` | Dumps Prometheus-formatted metrics to `stdout` on exit. |
| `--verbose` | `-v` | Prints the disassembled bytecode (ISA) before execution. |
| `--cold` | `-c` | Forces a "Cold Boot" by ignoring the existing `walia.state`. |
| `--help` | `-h` | Displays the help manual and version information. |

## 5. Persistence Management via CLI
Because Walia owns its data in a `.state` image, the CLI provides mechanisms to manage this environment without external tools.

*   **Heap Reset:** To clear all persistent globals and closures, simply remove the state file:
    ```bash
    rm walia.state
    ```
*   **State Portability:** The `walia.state` file is a portable binary image. It can be moved between compatible hardware architectures (e.g., from one x64 Linux server to another) and resumed instantly using the CLI.

## 6. Environment Variables (Advanced Tuning)
For enterprise-scale deployments, Walia supports environment variables to tune the internal C-substrate without recompilation.

*   **`WALIA_HEAP_SIZE`**: Sets the size of the memory-mapped heap in bytes (Default: 128MB).
*   **`WALIA_GC_THRESHOLD`**: Sets the initial memory consumption point that triggers a GC cycle.
*   **`WALIA_SYNC_INTERVAL`**: Defines the frequency (in instructions) of automatic background disk flushes.

## 7. Exit Codes (CI/CD Integration)
Walia adheres to industrial exit code standards to facilitate integration into automated pipelines (e.g., Kubernetes probes or GitHub Actions).

*   **`0`**: Successful execution.
*   **`65`**: Compile-time error (Syntax or Semantic analysis failed).
*   **`70`**: Runtime error (Exception occurred during VM execution).
*   **`74`**: I/O error (Source file missing or `.state` file corrupted).

## 8. Deployment Best Practices
In a production cloud environment, it is recommended to run Walia with the `--telemetry` flag enabled and pipe the output to a log aggregator. This provides real-time visibility into the health of the sovereign heap and the frequency of algebraic effect resumptions.

---
**Status:** Enterprise Specification 1.0  
**Interface:** POSIX-Compliant CLI  
**Binary:** `walia` v1.0.0  

