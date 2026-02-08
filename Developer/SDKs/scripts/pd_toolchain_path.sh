#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
TC_XCTOOLCHAIN="$ROOT_DIR/Developer/SDKs/Toolchains/PocketDarwin.xctoolchain"
TC_TOOLCHAIN="$ROOT_DIR/Developer/SDKs/Toolchains/PocketDarwin.toolchain"

if [[ -d "$TC_XCTOOLCHAIN/usr/bin" ]]; then
  echo "$TC_XCTOOLCHAIN/usr/bin"
  exit 0
fi

if [[ -d "$TC_TOOLCHAIN/usr/bin" ]]; then
  echo "$TC_TOOLCHAIN/usr/bin"
  exit 0
fi

echo "Toolchain not found." >&2
exit 1
