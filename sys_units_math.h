#ifndef WALIA_SYS_UNITS_MATH_H
#define WALIA_SYS_UNITS_MATH_H

#include "ast.h"

// ==========================================
// DIMENSIONAL ALGEBRA API
// ==========================================

/**
 * @brief Checks if two dimensional vectors are identical.
 * Mandatory for Addition and Subtraction operations.
 */
bool wld_dim_equal(WldDimensionVector a, WldDimensionVector b);

/**
 * @brief Performs dimensional multiplication (Adds exponents).
 * Logic: [L1, M1, T1...] * [L2, M2, T2...] = [L1+L2, M1+M2, T1+T2...]
 */
WldDimensionVector wld_dim_multiply(WldDimensionVector a, WldDimensionVector b);

/**
 * @brief Performs dimensional division (Subtracts exponents).
 * Logic: [L1, M1, T1...] / [L2, M2, T2...] = [L1-L2, M1-M2, T1-T2...]
 */
WldDimensionVector wld_dim_divide(WldDimensionVector a, WldDimensionVector b);

/**
 * @brief Checks if the value is a dimensionless scalar.
 */
bool wld_dim_is_scalar(WldDimensionVector v);

/**
 * @brief Returns a zero-initialized (dimensionless) vector.
 */
WldDimensionVector wld_dim_empty();

#endif // WALIA_SYS_UNITS_MATH_H