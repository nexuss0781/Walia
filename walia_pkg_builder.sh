#!/usr/bin/env bash

# ==============================================================================
# WALIA PACKAGE BUILDER (Enterprise Release v1.0.0)
# (c) 2025. Automated Sovereign Distribution.
# ==============================================================================

set -e 

VERSION="1.0.0"
MAINTAINER="Sovereign Architect <dev@walia.io>"
DESCRIPTION="Sovereign Language & Database Substrate."

# 1. ARCHITECTURE DETECTION
RAW_ARCH=$(uname -m)
case $RAW_ARCH in
    x86_64)          ARCH="amd64" ;;
    aarch64)         ARCH="arm64" ;;
    armv8l|armv7l)   ARCH="armhf" ;; 
    *)               ARCH="foreign" ;;
esac

# 2. PATH CONFIGURATION
PKG_NAME="walia_${VERSION}_${ARCH}"
BUILD_DIR="walia_dist" 

echo ">> Walia: Initiating Enterprise Build for Architecture: ${ARCH}..."

# 3. PRE-FLIGHT CLEANUP & STRUCTURE
rm -rf "${BUILD_DIR}"
rm -f *.deb *.tar.gz *.sha256
mkdir -p "${BUILD_DIR}/DEBIAN"
mkdir -p "${BUILD_DIR}/usr/local/bin"

# 4. COMPILE PRODUCTION SUBSTRATE
echo ">> Walia: Compiling Optimized Substrate..."
make clean > /dev/null
make all > /dev/null

# 5. ASSEMBLE DEBIAN STRUCTURE
echo ">> Walia: Assembling Sovereign Files..."
cp walia "${BUILD_DIR}/usr/local/bin/"

cat <<EOF > "${BUILD_DIR}/DEBIAN/control"
Package: walia
Version: ${VERSION}
Section: devel
Priority: optional
Architecture: ${ARCH}
Maintainer: ${MAINTAINER}
Description: ${DESCRIPTION}
 Walia is a data-sovereign language built for multi-trillion 
 neuron AI and enterprise-grade persistent logic.
EOF

# 6. ENFORCE DEBIAN PERMISSIONS (UFO SAFETY)
# Fixed: Explicitly setting permissions to satisfy dpkg-deb requirements
chmod 755 "${BUILD_DIR}"
chmod 755 "${BUILD_DIR}/DEBIAN"
chmod 644 "${BUILD_DIR}/DEBIAN/control"
chmod 755 "${BUILD_DIR}/usr/local/bin/walia"

# 7. GENERATE LINUX DEBIAN PACKAGE (.deb)
if command -v dpkg-deb &> /dev/null; then
    echo ">> Walia: Building Debian Package..."
    # Using --root-owner-group to ensure standard package ownership
    dpkg-deb --build --root-owner-group "${BUILD_DIR}" "${PKG_NAME}.deb" > /dev/null
    sha256sum "${PKG_NAME}.deb" > "${PKG_NAME}.deb.sha256"
    echo -e ">> Status: \033[0;32m.deb Ready\033[0m"
else
    echo ">> Walia: dpkg-deb not found. Skipping .deb generation."
fi

# 8. GENERATE TERMUX ARCHIVE (.tar.gz)
echo ">> Walia: Generating Termux-Portable Archive..."
TERMUX_PKG="walia_termux_${VERSION}_${ARCH}.tar.gz"
tar -czf "${TERMUX_PKG}" walia
sha256sum "${TERMUX_PKG}" > "${TERMUX_PKG}.sha256"
echo -e ">> Status: \033[0;32m.tar.gz Ready\033[0m"

# 9. FINAL CLEANUP
rm -rf "${BUILD_DIR}"
echo "------------------------------------------------"
echo -e "ARCHITECTURE: ${ARCH}"
echo -e "PACKAGE:      ${TERMUX_PKG}"
echo "------------------------------------------------"
