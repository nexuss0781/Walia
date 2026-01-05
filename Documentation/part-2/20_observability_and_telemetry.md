# Observability and Telemetry

## 1. Observability as a Language Primitive
In the Walia ecosystem, observability is not an external plugin or a secondary consideration; it is integrated into the core Virtual Machine substrate. To meet the demands of enterprise distributed systems, Walia provides a **Zero-Allocation Telemetry System**. This ensures that monitoring the health and performance of a sovereign script never consumes the resources required for the logic itself, nor does it trigger unintended Garbage Collection cycles.

## 2. Integrated Metric Categories
The Walia VM tracks performance metrics across three high-impact categories. Each metric is exported in an industry-standard format compatible with **Prometheus** and **OpenTelemetry**.

### I. Execution Metrics (Counters)
*   `walia_instructions_total`: The total count of 32-bit instructions processed.
*   `walia_effects_performed_total`: The number of algebraic effects triggered by `perform`.
*   `walia_calls_total`: Total function and native calls executed.

### II. Memory & GC Metrics (Gauges & Counters)
*   `walia_heap_bytes_current`: The current size of the mapped `walia.state` file.
*   `walia_gc_cycles_total`: Total number of generational GC events.
*   `walia_gc_reclaimed_bytes_total`: Cumulative memory returned to the persistent allocator.

### III. System Health (Gauges)
*   `walia_stack_depth_current`: Current number of active `CallFrame` windows.
*   `walia_uptime_seconds`: Time elapsed since the Walia VM boot sequence.

## 3. The Zero-Allocation Storage Engine
Traditional telemetry systems often rely on dynamic memory (JSON builders or linked lists). In Walia, all metrics are stored in a **Fixed-Size Primitive Array** within the C runtime.

*   **Static Buffering:** Counters are 64-bit unsigned integers stored at fixed memory offsets.
*   **OOM Resiliency:** Because metrics storage is pre-allocated at VM startup, the system can accurately report data even when the persistent heap is 100% full.
*   **Deterministic Latency:** Incrementing a metric cost exactly one CPU `INC` instruction, resulting in zero measurable impact on script execution speed.

## 4. Exporting Data
Walia supports two primary modes for exporting telemetry data to the cloud ecosystem.

### I. CLI Integration
When running a script, the `--telemetry` flag instructs the CLI to dump a Prometheus-formatted report to `stdout` upon process termination or at specific intervals.

```text
# HELP walia_heap_bytes_current Current size of the persistent heap file
# TYPE walia_heap_bytes_current gauge
walia_heap_bytes_current 134217728
```

### II. Native API
The Standard Library provides a `walia_metrics()` function, allowing a script to introspect its own performance or expose an HTTP endpoint for real-time scraping by monitoring agents.

```javascript
// A self-monitoring loop
while (true) {
    var stats = walia_metrics();
    if (stats.gc_count > 100) {
        print "High GC churn detected.";
    }
    sleep(60);
}
```

## 5. Persistent Telemetry Logs
Because Walia utilizes **Orthogonal Persistence**, critical telemetry data can be stored within the `.state` file itself. This allows for **Historical Analysis**: when a Walia VM resumes from a warm boot, it can compare its current performance against its state from hours or days prior, facilitating deep diagnostics of long-running autonomous processes.

## 6. Enterprise Utility
By embedding telemetry directly into the ISA and the persistence layer, Walia eliminates the "Observer Effect," where the act of monitoring a program changes its behavior. Developers gain a high-fidelity view of the sovereign heap, effect frequency, and instruction throughput with zero configuration.

---
**Status:** Technical Specification 1.0  
**Format:** Prometheus / OpenTelemetry (OTEL)  
**Storage:** Pre-allocated 64-bit Ring Buffer  

