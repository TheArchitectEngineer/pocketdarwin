#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
SDK_ROOT="$ROOT_DIR/Developer/SDKs/PocketDarwin.sdk"

export PKG_CONFIG_DIR=""
export PKG_CONFIG_LIBDIR="$SDK_ROOT/usr/lib/pkgconfig"
export PKG_CONFIG_SYSROOT_DIR="$SDK_ROOT"

echo "PKG_CONFIG_LIBDIR=$PKG_CONFIG_LIBDIR"
echo "PKG_CONFIG_SYSROOT_DIR=$PKG_CONFIG_SYSROOT_DIR"
