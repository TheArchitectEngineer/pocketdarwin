#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
SDK_ROOT="$ROOT_DIR/Developer/SDKs/PocketDarwin.sdk"

missing=0

require_file() {
  local path="$1"
  if [[ ! -e "$path" ]]; then
    echo "missing: $path"
    missing=1
  fi
}

require_file "$SDK_ROOT/SDKSettings.json"
require_file "$SDK_ROOT/usr/include/stdio.h"
require_file "$SDK_ROOT/usr/include/stdlib.h"
require_file "$SDK_ROOT/usr/include/string.h"
require_file "$SDK_ROOT/usr/include/stdint.h"
require_file "$SDK_ROOT/usr/include/unistd.h"
require_file "$SDK_ROOT/usr/lib/pkgconfig/libSystem.pc"

if [[ $missing -ne 0 ]]; then
  echo "Sysroot validation failed."
  exit 1
fi

echo "Sysroot looks OK: $SDK_ROOT"
