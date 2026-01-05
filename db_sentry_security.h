#ifndef WALIA_DB_SENTRY_SECURITY_H
#define WALIA_DB_SENTRY_SECURITY_H

#include "db_common.h"
#include "db_pager.h"

// ==========================================
// SECURITY SPECIFICATIONS
// ==========================================

typedef enum {
    WLD_PERM_NONE  = 0,
    WLD_PERM_READ  = 1,
    WLD_PERM_WRITE = 2,
    WLD_PERM_ADMIN = 4
} WldPermission;

/**
 * @brief Entry in the Sovereign Security Registry.
 */
typedef struct {
    char          roleName[32];
    PageID        targetPageId; // Table or Collection Root
    uint32_t      permissions;  // Bitmask
    bool          isInheritable;
} WldSecurityEntry;

// ==========================================
// SOVEREIGN SECURITY API
// ==========================================

/**
 * @brief Enforces access rules before a memory pointer is returned.
 * Returns true if the current context has the requested permission.
 */
bool db_security_check_access(WldPager* pager, PageID targetRoot, WldPermission requested);

/**
 * @brief Authenticates the VM session as a specific role.
 */
void db_security_assume_role(const char* roleName);

/**
 * @brief Persistently grants permissions on a table to a role.
 */
void db_security_grant(WldPager* pager, const char* role, PageID tableRoot, uint32_t perms);

#endif // WALIA_DB_SENTRY_SECURITY_H
