#include <stdio.h>
#include <string.h>

#include "../sys/sys_units_check.h"
#include "../sys/sys_units_math.h"
#include "../tooling/tooling_ledger.h" // For error reporting

// ==========================================
// REALITY VALIDATION ENGINE
// ==========================================

bool sys_units_validate_binary(BinaryExpr* expr) {
    WldDimensionVector leftDim  = expr->left->dimensions;
    WldDimensionVector rightDim = expr->right->dimensions;
    WldDimensionVector resultDim;

    switch (expr->operator.type) {
        // 1. ADDITIVE LAWS: Dimensions MUST match exactly
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            if (!wld_dim_equal(leftDim, rightDim)) {
                char reason[256];
                sprintf(reason, "Sovereign Reality Error: Cannot add/subtract incompatible units.");
                walia_ledger_log(FAULT_CRITICAL, reason, "Physics Sentry", 
                                 "geometry", expr->obj.line, NULL, CURE_NONE);
                return false;
            }
            resultDim = leftDim; // Identity preserved
            break;

        // 2. MULTIPLICATIVE LAWS: Dimensions are combined
        case TOKEN_STAR:
            resultDim = wld_dim_multiply(leftDim, rightDim);
            break;

        case TOKEN_SLASH:
            resultDim = wld_dim_divide(leftDim, rightDim);
            break;

        // 3. LOGICAL LAWS: Usually require scalar inputs
        case TOKEN_GREATER:
        case TOKEN_LESS:
        case TOKEN_EQUAL_EQUAL:
            if (!wld_dim_equal(leftDim, rightDim)) {
                walia_ledger_log(FAULT_CRITICAL, "Cannot compare incompatible physical quantities.", 
                                 "Physics Sentry", "logic", expr->obj.line, NULL, CURE_NONE);
                return false;
            }
            resultDim = wld_dim_empty(); // Comparison results in a dimensionless bool
            break;

        default:
            resultDim = leftDim;
            break;
    }

    // Assign the derived dimension to the parent expression for further chaining
    expr->obj.dimensions = resultDim;
    return true;
}

bool sys_units_validate_assignment(VarStmt* stmt) {
    // If the variable has a defined unit constraint (e.g., var x: <m>)
    // ensure the initializer's derived units match.
    if (!wld_dim_is_scalar(stmt->dimConstraint)) {
        WldDimensionVector initDim = stmt->initializer->dimensions;
        
        if (!wld_dim_equal(stmt->dimConstraint, initDim)) {
            char reason[256];
            sprintf(reason, "Assignment Violation: Data does not match variable's physical constraint.");
            walia_ledger_log(FAULT_CRITICAL, reason, stmt->name.start, 
                             "persistence", stmt->obj.line, NULL, CURE_NONE);
            return false;
        }
    }
    return true;
}
