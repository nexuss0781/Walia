#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "db_pager.h"

// ==========================================
// INTERNAL HELPERS
// ==========================================

static bool grow_file(WldPager* pager, uint64_t newSize) {
    if (ftruncate(pager->fd, newSize) == -1) {
        perror("WaliaDB: Failed to grow database file");
        return false;
    }
    
    // In a production x64 environment, we use remap or unmap/map
    // to ensure the address space is contiguous.
    munmap(pager->fileMapping, pager->fileSize);
    
    pager->fileMapping = mmap(NULL, newSize, PROT_READ | PROT_WRITE, 
                              MAP_SHARED, pager->fd, 0);
    
    if (pager->fileMapping == MAP_FAILED) {
        perror("WaliaDB: Failed to re-map database file");
        exit(WLD_ERR_IO);
    }
    
    pager->fileSize = newSize;
    pager->header = (WldSuperblock*)pager->fileMapping;
    return true;
}

// ==========================================
// PUBLIC IMPLEMENTATION
// ==========================================

WldPager* db_pager_open(const char* path) {
    int fd = open(path, O_RDWR | O_CREAT, 0600);
    if (fd == -1) return NULL;

    struct stat st;
    fstat(fd, &st);
    
    WldPager* pager = malloc(sizeof(WldPager));
    pager->fd = fd;
    pager->fileSize = st.st_size;

    // Handle Fresh Database Creation
    if (pager->fileSize < WLD_PAGE_SIZE) {
        pager->fileSize = WLD_PAGE_SIZE * 10; // Initial 10-page pre-allocation
        ftruncate(fd, pager->fileSize);
    }

    pager->fileMapping = mmap(NULL, pager->fileSize, PROT_READ | PROT_WRITE, 
                              MAP_SHARED, fd, 0);
    
    if (pager->fileMapping == MAP_FAILED) {
        free(pager);
        close(fd);
        return NULL;
    }

    pager->header = (WldSuperblock*)pager->fileMapping;

    // Initialize Superblock if new
    if (pager->header->magic != WLD_MAGIC) {
        memset(pager->header, 0, WLD_PAGE_SIZE);
        pager->header->magic = WLD_MAGIC;
        pager->header->version = WLD_VERSION;
        pager->header->pageCount = 1; // Superblock is Page 0
        pager->header->catalogRoot = WLD_INVALID_PAGE;
        pager->header->freeListHead = WLD_INVALID_PAGE;
    }

    pager->totalPages = (uint32_t)(pager->fileSize / WLD_PAGE_SIZE);
    return pager;
}

void* db_pager_get_page(WldPager* pager, PageID pageId) {
    if (pageId >= pager->totalPages) {
        fprintf(stderr, "WaliaDB: Out of bounds page access (%u).\n", pageId);
        return NULL;
    }
    
    // UFO-Grade Speed: Offset arithmetic only. No IO syscall.
    return (void*)(pager->fileMapping + (pageId * WLD_PAGE_SIZE));
}

PageID db_pager_allocate(WldPager* pager) {
    // Check if we need to grow the physical file
    if (pager->header->pageCount >= pager->totalPages) {
        uint64_t newSize = pager->fileSize * 2; // Geometric growth
        if (!grow_file(pager, newSize)) return WLD_INVALID_PAGE;
        pager->totalPages = (uint32_t)(newSize / WLD_PAGE_SIZE);
    }

    PageID newId = pager->header->pageCount++;
    
    // Initialize the new page header
    WldPageHeader* head = (WldPageHeader*)db_pager_get_page(pager, newId);
    head->type = PAGE_TYPE_OVERFLOW; // Default to raw until assigned
    head->lsn = 0;
    head->checksum = 0;
    
    return newId;
}

void db_pager_flush(WldPager* pager, PageID pageId) {
    void* addr = db_pager_get_page(pager, pageId);
    // Flush specifically the 4KB segment to disk
    msync(addr, WLD_PAGE_SIZE, MS_SYNC);
}

void db_pager_close(WldPager* pager) {
    if (pager == NULL) return;
    
    // Atomic Final Flush of Superblock
    msync(pager->fileMapping, WLD_PAGE_SIZE, MS_SYNC);
    
    munmap(pager->fileMapping, pager->fileSize);
    close(pager->fd);
    free(pager);
}
