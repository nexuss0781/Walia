#ifndef WALIA_SYS_PARSER_EXT_H
#define WALIA_SYS_PARSER_EXT_H

#include "../core/parser.h"
#include "../sys/sys_types.h"

// ==========================================
// PARSER EXTENSION API
// ==========================================

/**
 * @brief Parses an optional type annotation.
 * Syntax: ": type_name"
 * 
 * @return WaliaSysType The resolved type, or SYS_TYPE_VALUE if no annotation.
 */
WaliaSysType sys_parse_type_annotation();

/**
 * @brief Hook for variable declaration parsing.
 * Called by parser.c when 'var' is encountered.
 */
Stmt* sys_parse_var_declaration();

#endif // WALIA_SYS_PARSER_EXT_H
