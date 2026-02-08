# ==============================================================================
# WALIA SOVEREIGN OS - MASTER BUILD ENGINE (v3.0.0 GOLD)
# (c) 2025. Freestanding Multi-Model Architecture.
# ==============================================================================

# 1. TOOLCHAIN (Standard x86_64 Freestanding)
CC      := gcc
AS      := as
LD      := gcc
OBJCOPY := objcopy

# 2. PATHS & DIRECTORIES
BUILD_DIR := build
ISO_DIR   := iso
ARCH_DIR  := arch/x86_64
CORE_DIR  := core
LIB_DIR   := lib
DRV_DIR   := drivers

# 3. COMPILATION FLAGS (UFO-GRADE OPTIMIZATION)
CFLAGS := -m32 -ffreestanding -O3 -Wall -Wextra -Iinclude/libc -Iinclude -I../ -fno-stack-protector -fno-pic -mno-red-zone -DWALIA_KERNEL
LDFLAGS := -m32 -nostdlib -T linker.ld

# 4. SOURCE DISCOVERY & OBJECT MAPPING
CORE_SOURCES := $(wildcard $(CORE_DIR)/*.c)
LIB_SOURCES  := $(wildcard $(LIB_DIR)/*.c)
DRV_SOURCES  := $(wildcard $(DRV_DIR)/*.c)
SHARED_SOURCES := ../src/core/vm.c ../src/core/object.c ../src/core/memory.c ../src/core/table.c ../src/core/scanner.c ../src/core/parser.c ../src/core/compiler.c ../src/core/hash.c \
                 ../src/core/ast.c ../src/core/chunk.c ../src/core/value.c \
                 ../src/sys/sys_layout.c ../src/sys/sys_types.c ../src/sys/sys_units_registry.c \
                 ../src/sys/sys_units_check.c ../src/sys/sys_units_math.c ../src/sys/sys_assembler.c \
                 ../src/compiler_pipe.c ../src/db/db_sentry_parser.c
S_SOURCES := boot.S

# Map sources to the local build directory to avoid clashing with host build
OBJECTS := $(addprefix $(BUILD_DIR)/obj/, $(notdir $(CORE_SOURCES:.c=.o))) \
           $(addprefix $(BUILD_DIR)/obj/, $(notdir $(LIB_SOURCES:.c=.o))) \
           $(addprefix $(BUILD_DIR)/obj/, $(notdir $(DRV_SOURCES:.c=.o))) \
           $(addprefix $(BUILD_DIR)/obj/, $(notdir $(SHARED_SOURCES:.c=.o))) \
           $(addprefix $(BUILD_DIR)/obj/, $(S_SOURCES:.S=.o))

# 5. MASTER TARGETS
.PHONY: all clean kernel iso run

all: kernel

kernel: $(BUILD_DIR)/walia_os.bin

$(BUILD_DIR)/walia_os.bin: $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	@$(LD) $(LDFLAGS) -o $@ $(OBJECTS)
	@echo ">> Walia OS: Sovereign Gold Binary Assembled [8-byte aligned]."

# Rules for local sources
$(BUILD_DIR)/obj/%.o: $(CORE_DIR)/%.c
	@mkdir -p $(BUILD_DIR)/obj
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/obj/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(BUILD_DIR)/obj
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/obj/%.o: $(DRV_DIR)/%.c
	@mkdir -p $(BUILD_DIR)/obj
	@$(CC) $(CFLAGS) -c $< -o $@

# Rules for shared sources in ../src/
$(BUILD_DIR)/obj/%.o: ../src/core/%.c
	@mkdir -p $(BUILD_DIR)/obj
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/obj/%.o: ../src/sys/%.c
	@mkdir -p $(BUILD_DIR)/obj
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/obj/%.o: ../src/db/%.c
	@mkdir -p $(BUILD_DIR)/obj
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/obj/%.o: ../src/%.c
	@mkdir -p $(BUILD_DIR)/obj
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/obj/%.o: %.S
	@mkdir -p $(BUILD_DIR)/obj
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
