#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "sys_units_registry.h"

// ==========================================
// SI BASE & DERIVED TABLE
// ==========================================

static WldUnitDefinition standardTable[] = {
    // Base Units
    { "m",   {{1, 0, 0, 0, 0, 0, 0}} }, // Length
    { "kg",  {{0, 1, 0, 0, 0, 0, 0}} }, // Mass
    { "s",   {{0, 0, 1, 0, 0, 0, 0}} }, // Time
    { "A",   {{0, 0, 0, 1, 0, 0, 0}} }, // Current
    { "K",   {{0, 0, 0, 0, 1, 0, 0}} }, // Temp
    { "mol", {{0, 0, 0, 0, 0, 1, 0}} }, // Amount
    { "cd",  {{0, 0, 0, 0, 0, 0, 1}} }, // Intensity

    // Derived Units (Standard Industry Aliases)
    { "Hz",  {{0, 0, -1, 0, 0, 0, 0}} }, // Frequency (1/s)
    { "N",   {{1, 1, -2, 0, 0, 0, 0}} }, // Force (kg*m/s^2)
    { "Pa",  {{-1, 1, -2, 0, 0, 0, 0}} },// Pressure (N/m^2)
    { "J",   {{2, 1, -2, 0, 0, 0, 0}} }, // Energy (N*m)
    { "W",   {{2, 1, -3, 0, 0, 0, 0}} }, // Power (J/s)
    { "C",   {{0, 0, 1, 1, 0, 0, 0}} },  // Charge (A*s)
    { "V",   {{2, 1, -3, -1, 0, 0, 0}} } // Voltage (W/A)
};

#define STANDARD_COUNT (sizeof(standardTable) / sizeof(WldUnitDefinition))

// ==========================================
// COMPOSITE PARSER (UFO-Grade)
// ==========================================

/**
 * @brief Internal helper to find an atomic unit in the table.
 */
static WldDimensionVector find_atomic(const char* name, int len) {
    for (size_t i = 0; i < STANDARD_COUNT; i++) {
        if (strlen(standardTable[i].name) == (size_t)len && 
            strncmp(standardTable[i].name, name, (size_t)len) == 0) {
            return standardTable[i].vector;
        }
    }
    return wld_dim_empty(); // Unknown units are treated as scalar
}

WldDimensionVector sys_units_resolve(const char* unitStr, int length) {
    WldDimensionVector finalVec = wld_dim_empty();
    
    // We utilize a two-pass tokenizer for composite strings like "m/s^2"
    // Logic: 
    // 1. Identify Numerator (until '/')
    // 2. Identify Denominator (after '/')
    
    char* copy = malloc((size_t)length + 1);
    memcpy(copy, unitStr, (size_t)length);
    copy[length] = '\0';

    char* slash = strchr(copy, '/');
    char* numerator = copy;
    if (slash) *slash = '\0';
    char* denominator = slash ? slash + 1 : NULL;

    // Process Numerator (Multiplicative)
    char* token = strtok(numerator, "*");
    while (token != NULL) {
        // Handle exponents like m^2
        char* hat = strchr(token, '^');
        int exp = 1;
        if (hat) {
            *hat = '\0';
            exp = atoi(hat + 1);
        }
        
        WldDimensionVector atomic = find_atomic(token, (int)strlen(token));
        for(int e=0; e<exp; e++) finalVec = wld_dim_multiply(finalVec, atomic);
        
        token = strtok(NULL, "*");
    }

    // Process Denominator (Divisive)
    if (denominator) {
        token = strtok(denominator, "*");
        while (token != NULL) {
            char* hat = strchr(token, '^');
            int exp = 1;
            if (hat) {
                *hat = '\0';
                exp = atoi(hat + 1);
            }
            
            WldDimensionVector atomic = find_atomic(token, (int)strlen(token));
            for(int e=0; e<exp; e++) finalVec = wld_dim_divide(finalVec, atomic);
            
            token = strtok(NULL, "*");
        }
    }

    free(copy);
    return finalVec;
}

void sys_units_init_registry() {
    printf(">> Walia Physics: SI-7 Registry initialized [%zu Atomic Units]\n", STANDARD_COUNT);
}