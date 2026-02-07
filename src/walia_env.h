#ifndef WALIA_ENV_H
#define WALIA_ENV_H

#include "core/common.h"

// ==========================================
// SOVEREIGN PATH CONFIGURATION
// ==========================================

#define WALIA_VERSION_STR "1.0.0"

// Standard paths for Linux/Termux distribution
#ifdef __ANDROID__
    #define WALIA_DEFAULT_LIB "/data/data/com.termux/files/usr/lib/walia"
#else
    #define WALIA_DEFAULT_LIB "/usr/local/lib/walia"
#endif

// The environment variable users can use to override library paths
#define WALIA_PATH_ENV "WALIA_PATH"

// ==========================================
// ENVIRONMENT API
// ==========================================

/**
 * @brief Bootstraps the Walia environment.
 * Identifies the location of the Walia binary and the Standard Library.
 */
void walia_env_init(const char* argv0);

/**
 * @brief Resolves an import string to a physical file path.
 * 
 * Logic:
 * 1. Checks current working directory.
 * 2. Checks WALIA_PATH environment variable.
 * 3. Checks global system library (WALIA_DEFAULT_LIB).
 * 
 * @param moduleName The name from 'import "name"'
 * @param outPath Buffer to store the resolved absolute path.
 * @return bool True if the module was found.
 */
bool walia_resolve_module(const char* moduleName, char* outPath);

/**
 * @brief Retrieves the path for the persistent state file.
 * Defaults to the current directory unless WALIA_STATE_DIR is set.
 */
const char* walia_get_state_path();

#endif // WALIA_ENV_H
