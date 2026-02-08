#include "walia_kernel_base.h"
#include <stdbool.h>
#include <stdarg.h>
#include "driver_vga.h"

bool hadError = false;

// Forward declare Walia's allocator
void* reallocate(void* pointer, size_t oldSize, size_t newSize);

void* k_memcpy(void* dest, const void* src, size_t num) {
    uint64_t* d64 = (uint64_t*)dest;
    const uint64_t* s64 = (const uint64_t*)src;
    while (num >= 8) {
        *d64++ = *s64++;
        num -= 8;
    }
    uint8_t* d8 = (uint8_t*)d64;
    const uint8_t* s8 = (const uint8_t*)s64;
    while (num--) *d8++ = *s8++;
    return dest;
}

void* k_memset(void* ptr, int value, size_t num) {
    uint8_t* p = (uint8_t*)ptr;
    uint8_t v = (uint8_t)value;
    if (v == 0 && num >= 8) {
        uint64_t* p64 = (uint64_t*)ptr;
        while (num >= 8) {
            *p64++ = 0;
            num -= 8;
        }
        p = (uint8_t*)p64;
    }
    while (num--) *p++ = v;
    return ptr;
}

size_t k_strlen(const char* str) {
    const char* start = str;
    while (*str) str++;
    return (size_t)(str - start);
}

int k_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int k_strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int k_memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;
    while (n--) {
        if (*p1 != *p2) return (int)*p1 - (int)*p2;
        p1++;
        p2++;
    }
    return 0;
}

char* k_strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

char* k_strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c) return (char*)s;
        s++;
    }
    if (c == 0) return (char*)s;
    return (void*)0;
}

static char* last_strtok = (void*)0;
char* k_strtok(char* str, const char* delim) {
    if (str) last_strtok = str;
    if (!last_strtok) return (void*)0;
    while (*last_strtok) {
        bool is_delim = false;
        for (const char* d = delim; *d; d++) if (*last_strtok == *d) { is_delim = true; break; }
        if (!is_delim) break;
        last_strtok++;
    }
    if (!*last_strtok) return (void*)0;
    char* token = last_strtok;
    while (*last_strtok) {
        bool is_delim = false;
        for (const char* d = delim; *d; d++) if (*last_strtok == *d) { is_delim = true; break; }
        if (is_delim) {
            *last_strtok = '\0';
            last_strtok++;
            return token;
        }
        last_strtok++;
    }
    last_strtok = (void*)0;
    return token;
}

int k_atoi(const char* str) {
    int res = 0;
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }
    return res;
}

char* k_strdup(const char* s) {
    size_t len = k_strlen(s) + 1;
    void* d = reallocate((void*)0, 0, len);
    if (!d) return (void*)0;
    return k_memcpy(d, s, len);
}

char* k_itoa(int value, char* str, int base) {
    char* rc; char* ptr; char* low;
    if (base < 2 || base > 36) { *str = '\0'; return str; }
    rc = ptr = str;
    if (value < 0 && base == 10) *ptr++ = '-';
    low = ptr;
    do {
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[value % base];
        value /= base;
    } while (value);
    *ptr-- = '\0';
    while (low < ptr) { char tmp = *low; *low++ = *ptr; *ptr-- = tmp; }
    return rc;
}

void* mmap(void* addr, size_t length, int prot, int flags, int fd, size_t offset) { (void)addr; (void)length; (void)prot; (void)flags; (void)fd; (void)offset; return (void*)-1; }
int msync(void* addr, size_t length, int flags) { (void)addr; (void)length; (void)flags; return 0; }
int open(const char* pathname, int flags, ...) { (void)pathname; (void)flags; return -1; }
int ftruncate(int fd, size_t length) { (void)fd; (void)length; return -1; }
int close(int fd) { (void)fd; return 0; }
int mprotect(void *addr, size_t len, int prot) { (void)addr; (void)len; (void)prot; return 0; }
long sysconf(int name) { (void)name; return 4096; }

double strtod(const char* str, char** endptr) { (void)str; if (endptr) *endptr = (char*)str; return 0.0; }
int sprintf(char* str, const char* format, ...) { return k_strcpy(str, format) ? k_strlen(format) : 0; }
int vfprintf(void* stream, const char* format, va_list args) { (void)stream; (void)format; (void)args; return 0; }
int fputs(const char* s, void* stream) { (void)s; (void)stream; return 0; }
void* realloc(void* ptr, size_t size) { return reallocate(ptr, 0, size); }

void walia_ledger_log(int sev, const char* reason, const char* context, const char* path, int line, uint8_t* hash, int cure) { (void)sev; (void)reason; (void)context; (void)path; (void)line; (void)hash; (void)cure; }
void walia_ledger_init() {}
void walia_ledger_clear() {}
void walia_history_init() {}
void walia_registry_init() {}
void walia_oracle_init() {}
void walia_entangle_init() {}
void walia_scan_project(const char* path) { (void)path; }
void walia_mppa_assess_project() {}
void walia_oracle_run_suite() {}
void walia_report_flight_deck() {}
void walia_ledger_get_master() {}
void walia_boot() {}
void walia_shutdown() {}
void vm_init_magic_strings() {}
void initNatives() {}
void initWaliaCollections() {}
void initSqlLibrary() {}
void initNoSqlLibrary() {}
void initVectorLibrary() {}
void initSqeLibrary() {}
void initSentryLibrary() {}
void initEvolutionLibrary() {}
void initIteratorLibrary() {}
void initTranscoderLibrary() {}
void walia_run_tests(void* func) { (void)func; }
void walia_repl_start() {}
void walia_nexus_start() {}
void walia_nexus_stop() {}
void walia_oracle_generate_manual() {}
void sys_hardware_probe_init() {}
void walia_ledger_calculate_ripple() {}
void* walia_jit_tier2_compile(void* func) { (void)func; return (void*)0; }
void* walia_jit_tier2_execute(void* thunk, int argCount, void* args) { (void)thunk; (void)argCount; (void)args; return (void*)0; }
void sys_allocator_init_kernel() {}
void walia_vm_init() {}
void sys_get_logic_identity() {}
void* global_fs_tx = (void*)0;
void* global_pool = (void*)0;
void db_btree_find() {}
void db_btree_insert() {}
bool IS_PTR(void* v) { (void)v; return false; }
void* AS_PTR(void* v) { return v; }
void* PTR_VAL(void* v) { return v; }
void walia_pulse_collect() {}
void* lapic_base = (void*)0;
void walia_pulse_emit() {}
void* sys_vm_bit_op(void* a, void* b, int op) { (void)a; (void)b; (void)op; return (void*)0; }
void* globalEntangleRegistry = (void*)0;
void* sys_vm_cast(void* v, int type) { (void)v; (void)type; return (void*)0; }
void sys_vm_write_u8(void* a, int b, int c) { (void)a; (void)b; (void)c; }
void* walia_fusion_get_property(void* a, void* b) { (void)a; (void)b; return (void*)0; }
void resumeContinuation(void* a, void* b) { (void)a; (void)b; }
void* captureContinuation(void* a, int b) { (void)a; (void)b; return (void*)0; }
void sys_release(void* a) { (void)a; }
void* sys_alloc(size_t a) { (void)a; return (void*)0; }
void* sys_kernel_call(int a, int b, void* c) { (void)a; (void)b; (void)c; return (void*)0; }
void ptr_write(uintptr_t a, void* b) { (void)a; (void)b; }
void* walia_evolve_breed(void* a, void* b, float c) { (void)a; (void)b; (void)c; return (void*)0; }
void* walia_gene_new(void* a) { (void)a; return (void*)0; }
void* ptr_read(uintptr_t a) { (void)a; return (void*)0; }
int db_vector_match_best(void* a, void* b, int c) { (void)a; (void)b; (void)c; return -1; }
bool walia_stream_next(void* a, void* b) { (void)a; (void)b; return false; }
void walia_entangle_register(int a, void* b, int* c, int d) { (void)a; (void)b; (void)c; (void)d; }
void* walia_stream_new(void* a, int b) { (void)a; (void)b; return (void*)0; }
void walia_ripple_trigger(int a) { (void)a; }
bool vm_try_binary_overload(void* a, void* b, void* c) { (void)a; (void)b; (void)c; return false; }
void* vm_trap_get_property(void* a, void* b) { (void)a; (void)b; return (void*)0; }
bool vm_trap_set_property(void* a, void* b, void* c) { (void)a; (void)b; (void)c; return false; }
void markObject(void* a) { (void)a; }
void markValue(void* a) { (void)a; }
