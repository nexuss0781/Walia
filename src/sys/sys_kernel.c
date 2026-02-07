#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "sys/sys_kernel.h"
#include "core/object.h"

// ==========================================
// ARCHITECTURE-SPECIFIC SYSCALL LOGIC
// ==========================================

/**
 * @brief Core Syscall Dispatcher.
 * Uses inline assembly to bridge Walia to the Kernel.
 */
Value sys_kernel_call(int id, int argCount, Value* args) {
    long result = 0;

    // UFO GRADE: Direct unboxing into raw registers
    uintptr_t a1 = (argCount > 0) ? (uintptr_t)AS_NUMBER(args[0]) : 0;
    uintptr_t a2 = (argCount > 1) ? (uintptr_t)AS_NUMBER(args[1]) : 0;
    uintptr_t a3 = (argCount > 2) ? (uintptr_t)AS_NUMBER(args[2]) : 0;
    uintptr_t a4 = (argCount > 3) ? (uintptr_t)AS_NUMBER(args[3]) : 0;
    uintptr_t a5 = (argCount > 4) ? (uintptr_t)AS_NUMBER(args[4]) : 0;
    uintptr_t a6 = (argCount > 5) ? (uintptr_t)AS_NUMBER(args[5]) : 0;

#if defined(__x86_64__)
    __asm__ __volatile__ (
        "movq %1, %%rax\n\t"
        "movq %2, %%rdi\n\t"
        "movq %3, %%rsi\n\t"
        "movq %4, %%rdx\n\t"
        "movq %5, %%r10\n\t"
        "movq %6, %%r8\n\t"
        "movq %7, %%r9\n\t"
        "syscall\n\t"
        "movq %%rax, %0\n\t"
        : "=r" (result)
        : "r" ((long)id), "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a6)
        : "rax", "rdi", "rsi", "rdx", "r10", "r8", "r9"
    );
#elif defined(__aarch64__)
    __asm__ __volatile__ (
        "mov x8, %1\n\t"
        "mov x0, %2\n\t"
        "mov x1, %3\n\t"
        "mov x2, %4\n\t"
        "mov x3, %5\n\t"
        "mov x4, %6\n\t"
        "mov x5, %7\n\t"
        "svc #0\n\t"
        "mov %0, x0\n\t"
        : "=r" (result)
        : "r" ((long)id), "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a6)
        : "x8", "x0", "x1", "x2", "x3", "x4", "x5"
    );
#else
    // Silence for non-hardware builds
    (void)id; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6;
#endif

    return NUMBER_VAL((double)result);
}

// ==========================================
// REGISTER ACCESS LOGIC
// ==========================================

Value sys_read_register(const char* regName) {
    uintptr_t val = 0;
#if defined(__x86_64__)
    if (strcmp(regName, "rax") == 0) __asm__ ("movq %%rax, %0" : "=r"(val));
    else if (strcmp(regName, "rbx") == 0) __asm__ ("movq %%rbx, %0" : "=r"(val));
    else if (strcmp(regName, "rcx") == 0) __asm__ ("movq %%rcx, %0" : "=r"(val));
    else if (strcmp(regName, "rdx") == 0) __asm__ ("movq %%rdx, %0" : "=r"(val));
#elif defined(__aarch64__)
    if (strcmp(regName, "x0") == 0) __asm__ ("mov %0, x0" : "=r"(val));
    else if (strcmp(regName, "x1") == 0) __asm__ ("mov %0, x1" : "=r"(val));
#endif
    return NUMBER_VAL((double)val);
}

void sys_write_register(const char* regName, Value val) {
    uintptr_t raw = (uintptr_t)AS_NUMBER(val);
#if defined(__x86_64__)
    if (strcmp(regName, "rax") == 0) __asm__ ("movq %0, %%rax" : : "r"(raw));
#elif defined(__aarch64__)
    if (strcmp(regName, "x0") == 0) __asm__ ("mov x0, %0" : : "r"(raw));
#endif
}

// ==========================================
// SOVEREIGN NATIVE BRIDGE
// ==========================================

extern void defineNative(const char* name, NativeFn function);

static Value walia_sys_call(int argCount, Value* args) {
    if (argCount < 1) return NIL_VAL;
    int id = (int)AS_NUMBER(args[0]);
    return sys_kernel_call(id, argCount - 1, &args[1]);
}

static Value walia_reg_read(int argCount, Value* args) {
    if (argCount != 1 || !IS_STRING(args[0])) return NIL_VAL;
    return sys_read_register(AS_CSTRING(args[0]));
}

void initKernelLibrary() {
    defineNative("syscall",  walia_sys_call);
    defineNative("reg_read", walia_reg_read);
    printf(">> Walia: Kernel System Bridge online.\n");
}
