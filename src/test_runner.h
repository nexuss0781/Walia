#ifndef WALIA_TEST_RUNNER_H
#define WALIA_TEST_RUNNER_H

#include "core/common.h"
#include "core/object.h"

/**
 * @brief The Integrated Sovereign Test Orchestrator.
 * 
 * Logic:
 * 1. Identifies all 'test' blocks within the compiled script.
 * 2. Executes them in a non-persistent sandbox heap.
 * 3. Provides a professional status report for passes/failures.
 * 
 * @param script The top-level function containing the compiled logic and tests.
 */
void walia_run_tests(ObjFunction* script);

#endif // WALIA_TEST_RUNNER_H
