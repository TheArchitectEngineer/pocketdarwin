#!/bin/bash
# Test script for generic ARM configurations

echo "=== PocketDarwin Generic ARM Configuration Test ==="
echo ""

# Test GENERICARM64 configuration
echo "Testing GENERICARM64 configuration..."
cd /Volumes/FileSystemMac/Developer/Projects/PocketDarwin/System/darwinKernel/xnu
make CURRENT_MACHINE_CONFIG=GENERICARM64 \
     SUPPORTED_ARM64_MACHINE_CONFIGS="VMAPPLE GENERICARM64" \
     print_exports 2>/dev/null | grep -E "(CURRENT_MACHINE_CONFIG|SUPPORTED_ARM64_MACHINE_CONFIGS)" | head -5

echo ""
echo "Testing GENERICARM configuration..."
make CURRENT_MACHINE_CONFIG=GENERICARM \
     SUPPORTED_ARM_MACHINE_CONFIGS="GENERICARM" \
     print_exports 2>/dev/null | grep -E "(CURRENT_MACHINE_CONFIG|SUPPORTED_ARM_MACHINE_CONFIGS)" | head -5

echo ""
echo "=== Configuration Files Created ==="
echo "ARM64 Configs:"
ls -1 /Volumes/FileSystemMac/Developer/Projects/PocketDarwin/System/darwinKernel/xnu/config/MASTER.arm64.* | grep -E "(GENERICARM|VMAPPLE)"

echo ""
echo "ARM Configs:"  
ls -1 /Volumes/FileSystemMac/Developer/Projects/PocketDarwin/System/darwinKernel/xnu/config/MASTER.arm.* | grep GENERICARM

echo ""
echo "Export Files:"
ls -1 /Volumes/FileSystemMac/Developer/Projects/PocketDarwin/System/darwinKernel/xnu/config/*.exports | grep -E "(GENERICARM|VMAPPLE)" | head -10

echo ""
echo "=== Makefile Targets ==="
cd /Volumes/FileSystemMac/Developer/Projects/PocketDarwin/System/darwinKernel
make -n genericarm64 2>/dev/null | head -3
make -n genericarm 2>/dev/null | head -3

echo ""
echo "✅ Generic ARM configuration test complete!"
