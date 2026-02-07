#ifndef WALIA_DB_SENTRY_MAPPER_H
#define WALIA_DB_SENTRY_MAPPER_H

#include "db_common.h" 
#include "db_schema.h"
#include "db_pager.h"  
#include "../core/ast.h"
#include "../core/object.h"

// ==========================================
// MAPPING SPECIFICATIONS
// ==========================================

typedef struct {
    char          className[WLD_MAX_NAME_LEN];
    WldColumn     columns[WLD_MAX_COLUMNS]; // Resolved via db_common.h
    int           columnCount;
    bool          hasVector;
    bool          isRelational;
    uint8_t       classHash[32];
} WldMappingContract;

// ==========================================
// SENTRY MAPPING API
// ==========================================

/**
 * @brief Analyzes a Walia Class AST node and produces a Mapping Contract.
 */
WldMappingContract db_sentry_map_class(Stmt* classStmt);

/**
 * @brief Physically applies the mapping to the WaliaDB Catalog.
 */
PageID db_sentry_deploy_contract(WldPager* pager, WldMappingContract* contract);

/**
 * @brief Resolves a property name to its physical byte-offset in O(1).
 */
int db_sentry_resolve_offset(PageID dataPageId, ObjString* name);

/**
 * @brief Logic-aware type inference for persistent fields.
 */
WldColumnType db_sentry_infer_type(Expr* initializer);

#endif // WALIA_DB_SENTRY_MAPPER_H
