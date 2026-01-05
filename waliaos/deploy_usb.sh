#!/usr/bin/env bash 

# ============================================================================== 
# WALIA USB DEPLOYMENT TOOL 
# WARNING: THIS WILL ERASE DATA ON THE TARGET DRIVE. 
# ============================================================================== 

ISO="walia_sovereign_v3.0.0.iso" 

if [ ! -f "$ISO" ]; then 
    echo "Error: ISO not found. Run ./build_release.sh first." 
    exit 1 
fi 

echo ">> Available Block Devices:" 
lsblk -d -o NAME,MODEL,SIZE,TYPE | grep "disk" 

echo "" 
read -p ">> ENTER TARGET DEVICE (e.g., sdb): " TARGET_DEV 

if [ -z "$TARGET_DEV" ]; then 
    echo "Operation cancelled." 
    exit 1 
fi 

# Safety check: Prevent writing to sda (usually the OS drive) 
if [ "$TARGET_DEV" == "sda" ] || [ "$TARGET_DEV" == "nvme0n1" ]; then 
    echo -e "\n[WARNING] You selected $TARGET_DEV. This looks like a system drive." 
    read -p "Are you ABSOLUTELY sure? (Type 'SOVEREIGN' to confirm): " CONFIRM 
    if [ "$CONFIRM" != "SOVEREIGN" ]; then 
        echo "Aborted." 
        exit 1 
    fi 
fi 

DEVICE_PATH="/dev/$TARGET_DEV" 

echo -e "\n>> WRITING WALIA OS TO $DEVICE_PATH..." 
echo ">> DO NOT REMOVE THE DRIVE." 

# The dd command performs the raw binary transfer 
sudo dd if="$ISO" of="$DEVICE_PATH" bs=4M status=progress oflag=sync 

echo -e "\n>> DEPLOYMENT COMPLETE." 
echo ">> You may now boot the target hardware from this USB drive." 
