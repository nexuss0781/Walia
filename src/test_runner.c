#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "test_runner.h"
#include "core/vm.h"
#include "core/object.h"
#include "persistence.h"

// ==========================================
// SOVEREIGN SANDBOXING
// ==========================================

static void beginTestSandbox() {
    vm.isTestMode = true;
    vm.isTraceActive = false; 
    printf(">> Walia: Entering Test Sandbox [Persistence Suspended]\n");
}

static void endTestSandbox() {
    vm.isTestMode = false;
    vm.isTraceActive = true;
}

// ==========================================
// ORCHESTRATION LOGIC
// ==========================================

void walia_run_tests(ObjFunction* script) {
    beginTestSandbox();
    
    clock_t startTotal = clock();
    int testCount = 0;
    int failCount = 0;

    printf("\n--- WALIA INTEGRATED TEST SUITE ---\n");

    for (int i = 0; i < script->chunk.constants.count; i++) {
        Value constant = script->chunk.constants.values[i];
        if (IS_FUNCTION(constant)) {
            ObjFunction* testFunc = AS_FUNCTION(constant);
            if (testFunc->name != NULL) {
                testCount++;
                printf("RUN  | [%s]\n", testFunc->name->chars);
                clock_t startTest = clock();
                InterpretResult result = interpret(testFunc);
                clock_t endTest = clock();
                double elapsed = (double)(endTest - startTest) / CLOCKS_PER_SEC;

                if (result == INTERPRET_OK) {
                    printf("PASS | [%s] (%.3f ms)\n", testFunc->name->chars, elapsed * 1000);
                } else {
                    printf("FAIL | [%s] - Logic Error\n", testFunc->name->chars);
                    failCount++;
                }
            }
        }
    }

    clock_t endTotal = clock();
    printf("-----------------------------------\n");
    printf("RESULT: %d/%d passed (%.3f seconds)\n\n", 
           testCount - failCount, testCount, (double)(endTotal - startTotal) / CLOCKS_PER_SEC);

    endTestSandbox();
    if (failCount > 0) exit(EXIT_COMPILE_ERROR);
}
