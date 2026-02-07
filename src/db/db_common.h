#ifndef WALIA_DB_COMMON_H
#define WALIA_DB_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include "../core/common.h" 

// ==========================================
// PHYSICAL STORAGE SPECIFICATIONS
// ==========================================

#define WLD_MAGIC         0x574C4442  
#define WLD_PAGE_SIZE     4096        
#define WLD_MAX_NAME_LEN  64          
#define WLD_MAX_COLUMNS   32          
#define WLD_INVALID_PAGE  0xFFFFFFFF  

// ==========================================
// CORE TYPE DEFINITIONS
// ==========================================

typedef uint32_t PageID;

typedef enum {
    WLD_OK = 0,
    WLD_ERR_IO,
    WLD_ERR_CORRUPT,
    WLD_ERR_FULL,
    WLD_ERR_PERMISSION,
    WLD_ERR_VERSION_MISMATCH
} WldResult;

typedef enum {
    PAGE_TYPE_SUPERBLOCK = 0,
    PAGE_TYPE_CATALOG    = 1,
    PAGE_TYPE_FREE_LIST  = 2,
    PAGE_TYPE_BTREE_NODE = 3,
    PAGE_TYPE_HASH_DIR   = 4,
    PAGE_TYPE_OVERFLOW   = 5,
    PAGE_TYPE_VECTOR_INDEX = 6,
    PAGE_TYPE_SCHEMA     = 7
} PageType;

// ==========================================
// RELATIONAL SPECIFICATIONS
// ==========================================

typedef enum {
    WLD_TYPE_INT,
    WLD_TYPE_FLOAT,
    WLD_TYPE_STRING,
    WLD_TYPE_BOOL,
    WLD_TYPE_VECTOR,
    WLD_TYPE_POINTER
} WldColumnType;

typedef struct {
    char     name[WLD_MAX_NAME_LEN];
    uint32_t type;         // WldColumnType
    uint32_t offset;       // Physical byte offset
    bool     isPrimaryKey;
    bool     isNullable;   // Fixed: Added for constraint enforcement
} WldColumn;

// ==========================================
// PERSISTENT HEADERS
// ==========================================

typedef struct {
    uint32_t type;            
    uint32_t lsn;             
    uint32_t checksum;        
    uint32_t ownerId;         
} WldPageHeader;

typedef struct {
    WldPageHeader header;
    char          tableName[WLD_MAX_NAME_LEN];
    uint32_t      btreeRoot;
    uint32_t      columnCount;
    uint32_t      rowSize;       // Fixed: Added for physical offset calculation
    uint8_t       schemaHash[32]; // Fixed: Added for content-identity verification
    WldColumn     columns[WLD_MAX_COLUMNS]; 
} WldTableDescriptor;

typedef struct {
    uint32_t magic;           
    uint32_t version;         
    uint64_t timestamp;       
    uint32_t pageCount;       
    PageID catalogRoot;       
    PageID freeListHead;      
    PageID shadowTableRoot;   
    uint64_t checksum;        
} WldSuperblock;

#endif // WALIA_DB_COMMON_H
