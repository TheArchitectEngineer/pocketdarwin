#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
SDK_ROOT="$ROOT_DIR/Developer/SDKs/PocketDarwin.sdk"
TOOLCHAIN_BIN="$($ROOT_DIR/Developer/SDKs/scripts/pd_toolchain_path.sh)"

export SDKROOT="$SDK_ROOT"
export PATH="$TOOLCHAIN_BIN:$PATH"
export PD_TARGET_TRIPLE="${PD_TARGET_TRIPLE:-arm64-apple-darwin}"

echo "SDKROOT=$SDKROOT"
echo "PATH=$PATH"
echo "PD_TARGET_TRIPLE=$PD_TARGET_TRIPLE"
