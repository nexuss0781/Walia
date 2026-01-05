#ifndef WALIA_SYS_UNITS_CHECK_H
#define WALIA_SYS_UNITS_CHECK_H

#include "ast.h"

// ==========================================
// PHYSICS SENTRY API
// ==========================================

/**
 * @brief Validates a binary operation for dimensional consistency.
 * 
 * Logic:
 * 1. Checks Operator Type (+, -, *, /).
 * 2. Compares dimensions of Left and Right operands.
 * 3. Computes the resulting dimension vector for the parent expression.
 * 
 * @return bool True if the operation is dimensionally valid.
 */
bool sys_units_validate_binary(BinaryExpr* expr);

/**
 * @brief Ensures a variable assignment matches its defined unit constraints.
 */
bool sys_units_validate_assignment(VarStmt* stmt);

#endif // WALIA_SYS_UNITS_CHECK_H