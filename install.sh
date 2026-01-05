#!/usr/bin/env bash

# ==============================================================================
# WALIA SOVEREIGN SDK INSTALLER
# (c) 2025. Enterprise-Grade Persistent Runtime.
# ==============================================================================

set -e # Exit on any error

# 1. VISUAL BRANDING
BLUE='\033[0;34m'
GREEN='\033[0;32m'
BOLD='\033[1m'
NC='\033[0m' # No Color

echo -e "${BLUE}${BOLD}"
echo "  _      __      _ _      "
echo " | | /| / /___ _(_) /__ _ "
echo " | |/ |/ / _ \`/ / / / _ \` |"
echo " |__/|__/\_,_/_/_/_/\_,_|"
echo -e "      SOVEREIGN ENGINE${NC}"
echo -e "------------------------------------------------"

# 2. ENVIRONMENT DETECTION
OS_TYPE=$(uname -s)
ARCH_TYPE=$(uname -m)
IS_TERMUX=0

if [ -d "/data/data/com.termux" ]; then
    IS_TERMUX=1
    echo -e ">> Status: ${GREEN}Termux Environment Detected${NC}"
else
    echo -e ">> Status: ${GREEN}Linux Environment Detected (${ARCH_TYPE})${NC}"
fi

# 3. DEPENDENCY RESOLUTION (The C-Substrate)
echo ">> Resolving Sovereign dependencies..."

install_deps() {
    if [ $IS_TERMUX -eq 1 ]; then
        pkg update -y && pkg install -y clang make binutils
    else
        # Detect Linux Package Manager
        if command -v apt-get &> /dev/null; then
            sudo apt-get update -y && sudo apt-get install -y build-essential gcc
        elif command -v dnf &> /dev/null; then
            sudo dnf groupinstall -y "Development Tools"
        elif command -v pacman &> /dev/null; then
            sudo pacman -S --noconfirm base-devel gcc
        fi
    fi
}

install_deps > /dev/null 2>&1
echo -e ">> Status: ${GREEN}Dependencies Resolved${NC}"

# 4. SOVEREIGN COMPILATION (UFO SPEED)
# In a real release, this would git-clone the repo. 
# Here we assume the source is present in the current dir.
echo ">> Compiling Walia Binary (Optimizing for ${ARCH_TYPE})..."

if [ ! -f "Makefile" ]; then
    echo "Error: Source code not found. Ensure you are in the Walia root."
    exit 1
fi

make clean > /dev/null 2>&1
make all > /dev/null 2>&1

# 5. GLOBAL DEPLOYMENT
echo ">> Deploying Walia to system path..."

if [ $IS_TERMUX -eq 1 ]; then
    make install > /dev/null 2>&1
else
    # Linux requires sudo for /usr/local/bin
    sudo make install > /dev/null 2>&1
fi

# 6. INITIALIZATION & VERIFICATION
echo -e ">> Status: ${GREEN}Installation Successful${NC}"
echo "------------------------------------------------"

# Run Walia to show version and confirm build
VERSION=$(./walia --help | head -n 1)

echo -e "${BOLD}Walia is now Sovereign on your system.${NC}"
echo -e "SDK Path: $(which walia)"
echo -e "Version:  ${GREEN}$VERSION${NC}"
echo ""
echo -e "${BOLD}NEXT STEPS:${NC}"
echo -e "  1. Create a logic file: ${BLUE}nano hello.wal${NC}"
echo -e "  2. Run your code:       ${BLUE}walia run hello.wal${NC}"
echo -e "  3. Persistence Check:   ${BLUE}ls -lh walia.state${NC}"
echo ""
echo -e "Visit ${BLUE}https://docs.walia.io${NC} for the Technical Manual."
echo "------------------------------------------------"
