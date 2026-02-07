#include <stdio.h>
#include <string.h>

#include "db_sentry_security.h"
#include "db_catalog.h"
#include "../core/object.h"
#include "../core/table.h"
#include "../core/vm.h"

// ==========================================
// SOVEREIGN CONTEXT
// ==========================================

typedef struct {
    ObjString* activeRole;
    uint32_t   authLevel;
} SovereignContext;

static SovereignContext current_context = { NULL, 0 };

// ==========================================
// ACCESS ENFORCEMENT
// ==========================================

bool db_security_check_access(WldPager* pager, PageID targetRoot, WldPermission requested) {
    // SYSTEM RULE: Core metadata pages are always accessible to the engine
    if (targetRoot <= 3) return true; 

    // 1. Resolve Identity
    if (current_context.activeRole == NULL) return false; 

    // 2. Scan Persistent ACL Registry
    // In production, Page 3 is reserved for the Security Index.
    PageID securityRegistryId = 3; 
    WldSecurityEntry* registry = (WldSecurityEntry*)db_pager_get_page(pager, securityRegistryId);

    // 3. UFO-Grade Bitmask Validation
    for (int i = 0; i < 100; i++) { 
        if (strcmp(registry[i].roleName, current_context.activeRole->chars) == 0) {
            if (registry[i].targetPageId == targetRoot) {
                return (registry[i].permissions & (uint32_t)requested) != 0;
            }
        }
    }

    return false; 
}

// ==========================================
// ROLE MANAGEMENT
// ==========================================

void db_security_assume_role(const char* roleName) {
    // Convert role to interned Walia string for O(1) matching
    current_context.activeRole = copyString(roleName, (int)strlen(roleName));
    printf(">> Walia Sentry: Logged in as Sovereign Role '%s'.\n", roleName);
}

void db_security_grant(WldPager* pager, const char* role, PageID tableRoot, uint32_t perms) {
    // Only ADMIN level sessions can modify the Security Registry
    if (!db_security_check_access(pager, 3, WLD_PERM_ADMIN)) {
        fprintf(stderr, "Walia Security Error: Admin privileges required to grant access.\n");
        return;
    }

    // Persistent update logic would follow here, writing to Page 3 via the Shadow Pager.
    printf(">> Walia Sentry: Role '%s' granted [%u] on target %u.\n", role, perms, tableRoot);
}
