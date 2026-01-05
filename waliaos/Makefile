# ============================================================================== 
# WALIA OS: SOVEREIGN BUILD ENGINE 
# Structure: Flat Root (boot.S/linker.ld in root) 
# ============================================================================== 
 
# 1. TOOLCHAIN 
CC      := gcc 
LD      := ld 
OBJCOPY := objcopy 
 
# 2. DIRECTORIES 
BUILD_DIR := build 
ISO_DIR   := iso_root 
# Location of the Walia Language Source (VM, Parser, Compiler) 
# Change this to wherever you saved the Phase 1-19 code 
VM_DIR    := ../src 
 
# 3. FLAGS (UFO-Grade / Freestanding) 
CFLAGS := -m64 -ffreestanding -O3 -flto -Wall -Wextra \ 
           -Iinclude -I$(VM_DIR) \ 
           -fno-stack-protector -fno-pic -mno-red-zone -mcmodel=kernel \ 
           -D_WALIA_KERNEL_MODE 
 
# 4. SOURCE DISCOVERY 
# OS Sources 
OS_C_SRC  := $(wildcard core/*.c) \ 
              $(wildcard drivers/*.c) \ 
              $(wildcard lib/*.c) 
 OS_S_SRC  := boot.S 
 
 # VM Sources (The Language Substrate) 
 # We exclude main.c because the OS has its own entry point (kernel_init.c) 
 VM_SRC    := $(wildcard $(VM_DIR)/*.c) 
 VM_SRC    := $(filter-out $(VM_DIR)/main.c $(VM_DIR)/walia_repl.c, $(VM_SRC)) 
 
 # Object Lists 
 OBJECTS   := $(OS_C_SRC:.c=.o) $(OS_S_SRC:.S=.o) $(VM_SRC:.c=.o) 
 
 # 5. TARGETS 
 .PHONY: all clean kernel iso run 
 
 all: kernel 
 
 kernel: $(BUILD_DIR)/walia_os.bin 
 
 $(BUILD_DIR)/walia_os.bin: $(OBJECTS) 
 	 @mkdir -p $(BUILD_DIR) 
 	 @echo ">> Linking Sovereign Kernel..." 
 	 @$(LD) -n -T linker.ld -o $@ $(OBJECTS) 
 	 @echo ">> Walia OS: Binary Created." 
 
 %.o: %.c 
 	 @echo ">> Compiling: $<" 
 	 @$(CC) $(CFLAGS) -c $< -o $@ 
 
 %.o: %.S 
 	 @echo ">> Assembling: $<" 
 	 @$(CC) $(CFLAGS) -c $< -o $@ 
 
 # 6. ISO GENERATION 
 iso: kernel 
 	 @mkdir -p $(ISO_DIR)/boot/grub 
 	 @cp $(BUILD_DIR)/walia_os.bin $(ISO_DIR)/boot/kernel.bin 
 	 @echo 'set timeout=0' > $(ISO_DIR)/boot/grub/grub.cfg 
 	 @echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg 
 	 @echo 'menuentry "Walia OS" { multiboot2 /boot/kernel.bin; boot; }' >> $(ISO_DIR)/boot/grub/grub.cfg 
 	 @grub-mkrescue -o walia_os.iso $(ISO_DIR) --quiet 
 	 @echo ">> Walia OS: ISO Image Ready (walia_os.iso)" 
 
 run: iso 
 	 @qemu-system-x86_64 -cdrom walia_os.iso -m 2G -serial stdio 
 
 clean: 
 	 @rm -rf $(BUILD_DIR) $(ISO_DIR) walia_os.iso 
 	 @rm -f core/*.o drivers/*.o lib/*.o *.o 
 	 @rm -f $(VM_DIR)/*.o 
 	 @echo ">> Cleaned." 
