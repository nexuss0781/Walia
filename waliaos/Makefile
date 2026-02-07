# ==============================================================================
# WALIA SOVEREIGN OS - MASTER BUILD ENGINE (v3.0.0 GOLD)
# (c) 2025. Freestanding Multi-Model Architecture.
# ==============================================================================

# 1. TOOLCHAIN (Standard x86_64 Freestanding)
CC      := gcc
AS      := as
LD      := ld
OBJCOPY := objcopy

# 2. PATHS & DIRECTORIES
BUILD_DIR := build
ISO_DIR   := iso
ARCH_DIR  := arch/x86_64
CORE_DIR  := core
LIB_DIR   := lib
DRV_DIR   := drivers

# 3. COMPILATION FLAGS (UFO-GRADE OPTIMIZATION)
# -ffreestanding: No assumptions about standard libraries
# -mno-red-zone: Essential for kernel stack safety
# -O3 -flto: Maximum cross-module optimization
CFLAGS := -m64 -ffreestanding -O3 -flto -Wall -Wextra -Iinclude -I../ -fno-stack-protector -fno-pic -mno-red-zone -mcmodel=kernel

# 4. SOURCE DISCOVERY
C_SOURCES := $(wildcard $(CORE_DIR)/*.c) \
             $(wildcard $(LIB_DIR)/*.c)  \
             $(wildcard $(DRV_DIR)/*.c)  \
             ../src/core/vm.c ../src/core/object.c ../src/core/memory.c ../src/core/table.c ../src/core/scanner.c ../src/core/parser.c ../src/core/compiler.c ../src/core/hash.c

S_SOURCES := boot.S

OBJECTS := $(C_SOURCES:.c=.o) $(S_SOURCES:.S=.o)

# 5. MASTER TARGETS
.PHONY: all clean kernel iso run

all: kernel

kernel: $(BUILD_DIR)/walia_os.bin

$(BUILD_DIR)/walia_os.bin: $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	@$(LD) -T linker.ld -o $@ $(OBJECTS) -nostdlib
	@echo ">> Walia OS: Sovereign Gold Binary Assembled [8-byte aligned]."

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	@$(CC) $(CFLAGS) -c $< -o $@

# 6. DISTRIBUTION (ISO Generation)
iso: kernel
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(BUILD_DIR)/walia_os.bin $(ISO_DIR)/boot/walia_os.bin
	@echo 'set timeout=0' > $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'menuentry "Walia OS v3.0.0 Gold" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '  multiboot2 /boot/walia_os.bin' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '  boot' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg
	@grub-mkrescue -o walia_sovereign_gold.iso $(ISO_DIR)
	@echo ">> Walia OS: Bootable ISO Generated [walia_sovereign_gold.iso]."

# 7. EXECUTION (UFO SIMULATION)
run: iso
	@qemu-system-x86_64 -cdrom walia_sovereign_gold.iso -m 2G -serial stdio

clean:
	@rm -rf $(BUILD_DIR) $(ISO_DIR) *.o $(ARCH_DIR)/*.o $(CORE_DIR)/*.o $(LIB_DIR)/*.o $(DRV_DIR)/*.o
	@echo ">> Walia OS: Build artifacts purged."
