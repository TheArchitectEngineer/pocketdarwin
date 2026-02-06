#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
SDK_ROOT="$ROOT_DIR/Developer/SDKs/PocketDarwin.sdk"

SRC_HEADERS="${1:-}"
SRC_LIBS="${2:-}"

if [[ -z "$SRC_HEADERS" || -z "$SRC_LIBS" ]]; then
  echo "Usage: $0 <headers_dir> <libs_dir>"
  exit 1
fi

if [[ ! -d "$SRC_HEADERS" ]]; then
  echo "Headers dir not found: $SRC_HEADERS"
  exit 1
fi

if [[ ! -d "$SRC_LIBS" ]]; then
  echo "Libs dir not found: $SRC_LIBS"
  exit 1
fi

mkdir -p "$SDK_ROOT/usr/include" "$SDK_ROOT/usr/lib"

rsync -a "$SRC_HEADERS/" "$SDK_ROOT/usr/include/"
rsync -a "$SRC_LIBS/" "$SDK_ROOT/usr/lib/"

echo "Sysroot populated: $SDK_ROOT"
