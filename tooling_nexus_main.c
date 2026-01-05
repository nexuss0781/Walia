#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include "tooling_nexus_main.h"
#include "tooling_nexus_bus.h"
#include "tooling_nexus_canvas.h"
#include "tooling_nexus_widgets.h"

// ==========================================
// NEXUS INTERNAL STATE
// ==========================================

static pthread_t nexusThread;
static bool nexusRunning = false;
static WldCanvas masterCanvas;

// ==========================================
// TELEMETRY AGGREGATION
// ==========================================

typedef struct {
    int totalPagesRead;
    int totalPagesWritten;
    int currentNeuralLevel;
    float currentSimilarity;
    size_t reclaimedMemory;
} NexusStats;

static NexusStats currentStats = {0, 0, 0, 0.0f, 0};

static void process_pulses() {
    WldPulse pulses[1024];
    int count = walia_pulse_collect(pulses, 1024);

    for (int i = 0; i < count; i++) {
        switch (pulses[i].type) {
            case PULSE_DB_PAGE_READ:  currentStats.totalPagesRead++; break;
            case PULSE_DB_PAGE_WRITE: currentStats.totalPagesWritten++; break;
            case PULSE_NEURAL_HOP:    currentStats.currentNeuralLevel = pulses[i].payload; break;
            case PULSE_NEURAL_MATH:   currentStats.currentSimilarity = (float)pulses[i].payload / 100.0f; break;
            case PULSE_GC_SWEEP:      currentStats.reclaimedMemory += pulses[i].payload; break;
            default: break;
        }
    }
}

// ==========================================
// MASTER REFRESH LOOP
// ==========================================

void* walia_nexus_thread_loop(void* arg) {
    (void)arg;
    db_canvas_init();

    while (nexusRunning) {
        // 1. COLLECT DATA: Drain the ring buffer
        process_pulses();

        // 2. PREPARE FRAME: Clear virtual buffer
        db_canvas_clear(&masterCanvas);

        // 3. DRAW WIDGETS: Layout the Sovereign HUD
        // Page Map (Left Segment)
        db_widget_draw_pagemap(&masterCanvas, 2, 2, 40, 20);

        // Neural Gauge (Center Segment)
        db_widget_draw_neural_gauge(&masterCanvas, 44, 2, 20, 
                                   currentStats.currentNeuralLevel, 
                                   currentStats.currentSimilarity);

        // Heap Tanks (Right Segment)
        db_widget_draw_heaptank(&masterCanvas, 58, 2, 512, 256); // Example levels

        // Status Bar (Bottom)
        char status[128];
        snprintf(status, 128, "Walia Sovereign Substrate | Reads: %d | Writes: %d | GC: %zu bytes",
                 currentStats.totalPagesRead, currentStats.totalPagesWritten, currentStats.reclaimedMemory);
        db_canvas_draw_text(&masterCanvas, 2, masterCanvas.height - 2, status, COLOR_CYAN);

        // 4. RENDER: Atomic flush to terminal
        db_canvas_render(&masterCanvas);

        // 5. THROTTLE: 60 FPS (approx 16ms sleep)
        usleep(16666);
    }

    db_canvas_shutdown();
    return NULL;
}

// ==========================================
// LIFECYCLE MANAGEMENT
// ==========================================

void walia_nexus_start() {
    if (nexusRunning) return;
    
    walia_pulse_init();
    nexusRunning = true;
    
    // Launch the TUI in a dedicated background thread
    if (pthread_create(&nexusThread, NULL, walia_nexus_thread_loop, NULL) != 0) {
        perror("Walia Nexus: Failed to spawn HUD thread");
        nexusRunning = false;
    }
}

void walia_nexus_stop() {
    if (!nexusRunning) return;
    
    nexusRunning = false;
    pthread_join(nexusThread, NULL);
    printf(">> Walia Command Nexus: Sovereign Cockpit deactivated.\n");
}
