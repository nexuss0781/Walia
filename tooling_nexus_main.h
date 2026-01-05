#ifndef WALIA_TOOLING_NEXUS_MAIN_H
#define WALIA_TOOLING_NEXUS_MAIN_H

#include "common.h"

// ==========================================
// NEXUS COMMAND API
// ==========================================

/**
 * @brief Launches the Command Nexus in a background thread.
 * Initializes the canvas and starts the 60fps refresh loop.
 */
void walia_nexus_start();

/**
 * @brief Stops the Nexus and restores the terminal state.
 */
void walia_nexus_stop();

/**
 * @brief Background thread entry point.
 */
void* walia_nexus_thread_loop(void* arg);

#endif // WALIA_TOOLING_NEXUS_MAIN_H
