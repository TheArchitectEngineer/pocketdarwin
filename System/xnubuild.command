#!/bin/bash
set -euo pipefail

# ---- configuration ----
XNU_DIR="$(pwd)/xnu"
BUILD_DIR="$(pwd)/build-xnu"
ARCH="ARM64"
KERNEL_CONFIG="DEVELOPMENT"

SDKROOT="$(xcrun --sdk macosx --show-sdk-path)"
TOOLCHAIN="$(xcode-select -p)/Toolchains/XcodeDefault.xctoolchain"

# ---- sanity checks ----
if [ ! -d "$XNU_DIR" ]; then
  echo "❌ xnu directory not found at ./xnu"
  exit 1
fi

if [ ! -d "$SDKROOT" ]; then
  echo "❌ macOS SDK not found"
  exit 1
fi

echo "🧠 SDKROOT = $SDKROOT"
echo "🛠 Toolchain = $TOOLCHAIN"

# ---- clean build dirs ----
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR/OBJROOT"
mkdir -p "$BUILD_DIR/SYMROOT"
mkdir -p "$BUILD_DIR/DSTROOT"

# ---- critical flags ----
export SDKROOT
export TOOLCHAIN_DIR="$TOOLCHAIN"
export DEVELOPER_DIR="$(xcode-select -p)"

export ARCH_CONFIGS="$ARCH"
export KERNEL_CONFIGS="$KERNEL_CONFIG"

# absolutely required to avoid simulator / iboot garbage
export OTHER_CFLAGS="-DTARGET_OS_SIMULATOR=0"
export OTHER_LDFLAGS=""

# prevent accidental embedded paths
unset CC_IBOOT
unset IBOOT
unset EMBEDDED
unset CONFIG_EMBEDDED

echo "🔥 Building XNU (no iBoot, no simulator)"

cd "$XNU_DIR"

make \
  SDKROOT="$SDKROOT" \
  ARCH_CONFIGS="$ARCH_CONFIGS" \
  KERNEL_CONFIGS="$KERNEL_CONFIGS" \
  OBJROOT="$BUILD_DIR/OBJROOT" \
  SYMROOT="$BUILD_DIR/SYMROOT" \
  DSTROOT="$BUILD_DIR/DSTROOT" \
  OTHER_CFLAGS="$OTHER_CFLAGS" \
  OTHER_LDFLAGS="$OTHER_LDFLAGS"

echo
echo "✅ Build complete"
echo "📦 Kernel output location:"

find "$BUILD_DIR" -name "kernel" -o -name "mach_kernel" || true
