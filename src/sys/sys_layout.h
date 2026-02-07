#ifndef WALIA_SYS_LAYOUT_H
#define WALIA_SYS_LAYOUT_H

#include "db/db_common.h"
#include "core/ast.h"
#include "sys/sys_types.h"

// ==========================================
// LAYOUT ENGINE API
// ==========================================

/**
 * @brief Computes physical memory offsets for a structured logic block.
 * 
 * Logic:
 * 1. For Layouts: Performs sequential packing with automatic alignment padding.
 * 2. For Unions: Resets all offsets to 0 and calculates the max required size.
 * 3. Handles Bitfields: Accumulates bits and advances byte-offsets only when full.
 * 
 * @param count Number of members in the structure.
 * @param members The array of member metadata to be updated with physical offsets.
 * @param isUnion Boolean flag to switch between struct and union logic.
 * @return int The total calculated size of the structure in bytes.
 */
int sys_layout_calculate(int count, LayoutMember* members, bool isUnion);

#endif // WALIA_SYS_LAYOUT_H
