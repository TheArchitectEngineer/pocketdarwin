#!/bin/bash
# Manual kernel linker for PocketDarwin XNU

set -e

echo "=== PocketDarwin XNU Kernel Linker ==="
echo ""

# Paths
BUILD_DIR="/Volumes/FileSystemMac/Developer/Projects/PocketDarwin/System/darwinKernel/build"
OBJ_DIR="$BUILD_DIR/obj/DEVELOPMENT_ARM64_VMAPPLE"
OUTPUT_DIR="$BUILD_DIR/dst"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Kernel output path
KERNEL_OUTPUT="$OUTPUT_DIR/mach_kernel"

echo "Output directory: $OUTPUT_DIR"
echo "Kernel output: $KERNEL_OUTPUT"
echo ""

# Collect all object files
echo "Collecting object files..."
OSFMK_OBJECTS=$(find "$OBJ_DIR/osfmk/DEVELOPMENT" -name "*.o" | tr '\n' ' ')
BSD_OBJECTS=$(find "$OBJ_DIR/bsd/DEVELOPMENT" -name "*.o" | tr '\n' ' ')
IOKIT_OBJECTS=$(find "$OBJ_DIR/iokit/DEVELOPMENT" -name "*.o" | tr '\n' ' ')
LIBKERN_OBJECTS=$(find "$OBJ_DIR/libkern/DEVELOPMENT" -name "*.o" 2>/dev/null | tr '\n' ' ')
SECURITY_OBJECTS=$(find "$OBJ_DIR/security/DEVELOPMENT" -name "*.o" 2>/dev/null | tr '\n' ' ')

# Combine all objects
ALL_OBJECTS="$OSFMK_OBJECTS $BSD_OBJECTS $IOKIT_OBJECTS $LIBKERN_OBJECTS $SECURITY_OBJECTS"

echo "Found object files:"
echo "  osfmk: $(echo $OSFMK_OBJECTS | wc -w)"
echo "  bsd: $(echo $BSD_OBJECTS | wc -w)"  
echo "  iokit: $(echo $IOKIT_OBJECTS | wc -w)"
echo "  libkern: $(echo $LIBKERN_OBJECTS | wc -w)"
echo "  security: $(echo $SECURITY_OBJECTS | wc -w)"
echo "  Total: $(echo $ALL_OBJECTS | wc -w)"
echo ""

# Link the kernel
echo "Linking kernel..."
clang \
    -arch arm64e \
    -mkernel \
    -static \
    -nostdlib \
    -r \
    -o "$KERNEL_OUTPUT" \
    $ALL_OBJECTS

echo ""
echo "✅ Kernel linking completed!"
echo "Kernel binary: $KERNEL_OUTPUT"
echo "Kernel size: $(ls -lh "$KERNEL_OUTPUT" | awk '{print $5}')"
echo ""

# Verify the kernel
echo "Verifying kernel binary..."
file "$KERNEL_OUTPUT"
echo ""

echo "🎉 PocketDarwin XNU kernel build complete!"
