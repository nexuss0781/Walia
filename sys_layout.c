#include <stdio.h>
#include <stdlib.h>

#include "sys_layout.h"

// ==========================================
// PHYSICAL MAPPING ENGINE
// ==========================================

int sys_layout_calculate(int count, LayoutMember* members, bool isUnion) {
    // CASE 1: UNION LOGIC (Shared Memory Space)
    if (isUnion) {
        int maxSize = 0;
        for (int i = 0; i < count; i++) {
            // In a union, all members reside at the same starting address
            members[i].offset = 0;
            const SysTypeInfo* info = sys_get_type_info((WaliaSysType)members[i].type);
            if (info->size > maxSize) maxSize = info->size;
        }
        // Total size is the size of the largest member
        return maxSize;
    }

    // CASE 2: LAYOUT LOGIC (Sequential Packing)
    int currentOffset = 0;
    int currentBitOffset = 0;
    int maxAlignment = 1;

    for (int i = 0; i < count; i++) {
        const SysTypeInfo* info = sys_get_type_info((WaliaSysType)members[i].type);
        
        // Track the largest alignment requirement for structure-level padding
        int alignment = info->size;
        if (alignment > maxAlignment) maxAlignment = alignment;

        if (members[i].bitWidth == 0) {
            // STANDARD MEMBER
            // 1. If we were packing bits, close the current byte
            if (currentBitOffset > 0) {
                currentOffset += 1;
                currentBitOffset = 0;
            }

            // 2. SOVEREIGN ALIGNMENT: Round up currentOffset to match type alignment
            // e.g., if offset is 1 but type is u32, move offset to 4.
            if (alignment > 1) {
                currentOffset = (currentOffset + alignment - 1) & ~(alignment - 1);
            }

            members[i].offset = currentOffset;
            currentOffset += alignment;
        } else {
            // BITFIELD MEMBER
            // Logical packing within the current byte segment
            members[i].offset = currentOffset;
            
            currentBitOffset += members[i].bitWidth;
            
            // If accumulated bits exceed 8, advance the physical byte offset
            if (currentBitOffset >= 8) {
                currentOffset += currentBitOffset / 8;
                currentBitOffset %= 8;
            }
        }
    }

    // FINAL PASS: Ensure the total structure size is aligned to the maxAlignment
    // This allows layouts to be used in arrays safely.
    if (currentBitOffset > 0) currentOffset += 1;
    currentOffset = (currentOffset + maxAlignment - 1) & ~(maxAlignment - 1);
    
    return currentOffset;
}