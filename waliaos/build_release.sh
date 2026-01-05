#!/usr/bin/env bash 

# ============================================================================== 
# WALIA OS: GOLDEN IMAGE GENERATOR 
# (c) 2025. The Sovereign Release Pipeline. 
# ============================================================================== 

set -e # Halt on error 

# CONFIGURATION 
KERNEL_BIN="build/walia_os.bin" 
ISO_NAME="walia_sovereign_v3.0.0.iso" 
ISO_DIR="iso_root" 

echo -e "\n>> [STEP 1] COMPILING KERNEL (SOVEREIGN MODE)..." 
# Force a clean rebuild to ensure no stale objects 
make clean 
make kernel 

echo -e "\n>> [STEP 2] VALIDATING MULTIBOOT2 HEADER..." 
if grub-file --is-x86-multiboot2 "$KERNEL_BIN"; then 
    echo ">> Success: Kernel is Multiboot2 Compliant." 
else 
    echo ">> FATAL: Kernel is not Multiboot2 compliant. Check boot.S." 
    exit 1 
fi 

echo -e "\n>> [STEP 3] PREPARING ISO STRUCTURE..." 
mkdir -p "$ISO_DIR/boot/grub" 
cp "$KERNEL_BIN" "$ISO_DIR/boot/kernel.bin" 
cp "arch/x86_64/grub.cfg" "$ISO_DIR/boot/grub/grub.cfg" 

echo -e "\n>> [STEP 4] GENERATING BOOTABLE ISO..." 
# grub-mkrescue creates a hybrid image bootable via CD-ROM or USB 
grub-mkrescue -o "$ISO_NAME" "$ISO_DIR" --quiet 

echo -e "\n>> [STEP 5] FINAL VERIFICATION..." 
ls -lh "$ISO_NAME" 

echo -e "\n--------------------------------------------------" 
echo -e ">> SOVEREIGN GOLD IMAGE READY: $ISO_NAME" 
echo -e ">> To boot in QEMU: make run" 
echo -e ">> To flash to USB: ./deploy_usb.sh" 
echo -e "--------------------------------------------------" 
