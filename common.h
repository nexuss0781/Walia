#ifndef WALIA_COMMON_H
#define WALIA_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// ==========================================
// GLOBAL STATE FLAGS
// ==========================================
// Centralized flag for build-gating (The Truth Gate)
extern bool hadError;

#define NAN_BOXING
#define UNUSED(x) (void)(x)

// Enterprise System Versions
#define WLD_VERSION 120 
#define WLD_PAGE_SIZE 4096

typedef enum {
    EXIT_SUCCESS_CODE = 0,
    EXIT_COMPILE_ERROR = 65,
    EXIT_RUNTIME_ERROR = 70,
    EXIT_IO_ERROR = 74
} ExitCode;

#endif // WALIA_COMMON_H