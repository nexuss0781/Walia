#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "walia_env.h"

static char exec_path[PATH_MAX];
static char lib_path[PATH_MAX];
static char state_file_path[PATH_MAX];

void walia_env_init(const char* argv0) {
    // Check return value to satisfy compiler safety requirements
    if (argv0 == NULL || realpath(argv0, exec_path) == NULL) {
        // Fallback if realpath fails
        strncpy(exec_path, "walia", PATH_MAX);
    }

    const char* env_path = getenv(WALIA_PATH_ENV);
    if (env_path != NULL) {
        strncpy(lib_path, env_path, PATH_MAX);
    } else {
        strncpy(lib_path, WALIA_DEFAULT_LIB, PATH_MAX);
    }

    if (getcwd(state_file_path, PATH_MAX) == NULL) {
        strncpy(state_file_path, ".", PATH_MAX);
    }
    strncat(state_file_path, "/walia.state", PATH_MAX - strlen(state_file_path) - 1);
}

bool walia_resolve_module(const char* moduleName, char* outPath) {
    char attempt[PATH_MAX];

    snprintf(attempt, PATH_MAX, "./%s.wal", moduleName);
    // Added safety check for realpath result
    if (access(attempt, F_OK) == 0) {
        if (realpath(attempt, outPath) != NULL) return true;
    }

    snprintf(attempt, PATH_MAX, "%s/%s.wal", lib_path, moduleName);
    if (access(attempt, F_OK) == 0) {
        strncpy(outPath, attempt, PATH_MAX);
        return true;
    }

    return false;
}

const char* walia_get_state_path() {
    return state_file_path;
}
