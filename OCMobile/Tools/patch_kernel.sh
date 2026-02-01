#!/bin/bash

# Kernel Patcher for PocketDarwin
# Applies .diff files to the Darwin kernel source for Android compatibility

KERNEL_DIR="../Kernel"
PATCH_DIR="./patches"  # Directory containing .diff files

echo "Applying kernel patches..."

if [ ! -d "$KERNEL_DIR" ]; then
    echo "Error: Kernel directory $KERNEL_DIR not found"
    exit 1
fi

if [ ! -d "$PATCH_DIR" ]; then
    echo "Warning: Patches directory $PATCH_DIR not found, creating..."
    mkdir -p "$PATCH_DIR"
    echo "Place your .diff files in $PATCH_DIR and run this script again"
    exit 0
fi

cd "$KERNEL_DIR"

for patch in "$PATCH_DIR"/*.diff; do
    if [ -f "$patch" ]; then
        echo "Applying $patch..."
        if git apply "$patch"; then
            echo "Applied $patch successfully"
        else
            echo "Failed to apply $patch"
            exit 1
        fi
    fi
done

echo "All patches applied successfully"