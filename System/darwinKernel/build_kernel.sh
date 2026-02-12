#!/bin/bash
# Direct build script for PocketDarwin XNU configurations

set -e

echo "=== PocketDarwin XNU Direct Build ==="
echo ""

# Configuration
CONFIG=${1:-VMAPPLE}
BUILDTYPE=${2:-DEVELOPMENT}
ARCH=${3:-ARM64}

echo "Configuration: $CONFIG"
echo "Build Type: $BUILDTYPE"
echo "Architecture: $ARCH"
echo ""

# Paths
XNU_SRC="/Volumes/FileSystemMac/Developer/Projects/PocketDarwin/System/darwinKernel/xnu"
BUILD_DIR="/Volumes/FileSystemMac/Developer/Projects/PocketDarwin/System/darwinKernel/build"

# Clean build
echo "Cleaning previous build..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Set environment variables
export SRCROOT="$XNU_SRC"
export OBJROOT="$BUILD_DIR/obj"
export SYMROOT="$BUILD_DIR/sym"
export DSTROOT="$BUILD_DIR/dst"
export SDKROOT="$(xcrun --show-sdk-path)"
export CURRENT_MACHINE_CONFIG="$CONFIG"
export KERNEL_CONFIGS="$BUILDTYPE"
export ARCH_CONFIGS="$ARCH"
export RC_DARWIN_KERNEL_VERSION="0.0.0"

# Set supported configs based on architecture
if [ "$ARCH" = "ARM64" ]; then
    export SUPPORTED_ARM64_MACHINE_CONFIGS="VMAPPLE GENERICARM64"
elif [ "$ARCH" = "ARM" ]; then
    export SUPPORTED_ARM_MACHINE_CONFIGS="GENERICARM"
fi

echo "Building XNU kernel..."
echo "Source: $XNU_SRC"
echo "Build: $BUILD_DIR"
echo ""

# Build the kernel
cd "$XNU_SRC"
make -j$(sysctl -n hw.ncpu) \
    LOGCOLORS=n \
    PLATFORM=MacOSX \
    BUILD_LTO=0 \
    BUILD_WERROR=0 \
    DO_CTFMERGE=0 \
    BUILD_STATIC_LINK=1 \
    VERBOSE=YES \
    USE_WERROR=0 \
    all

echo ""
echo "✅ XNU kernel build completed!"
echo "Configuration: $CONFIG"
echo "Build artifacts in: $BUILD_DIR"

# Check for kernel binary
if [ -f "$BUILD_DIR/dst/mach_kernel" ]; then
    echo "Kernel binary: $BUILD_DIR/dst/mach_kernel"
    ls -lh "$BUILD_DIR/dst/mach_kernel"
else
    echo "⚠️  Kernel binary not found. Manual linking may be required."
    echo "Run: ./link_kernel.sh"
fi
