#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
TC_BIN="$($ROOT_DIR/Developer/SDKs/scripts/pd_toolchain_path.sh)"
SDK_ROOT="$ROOT_DIR/Developer/SDKs/PocketDarwin.sdk"

WORK_DIR="${1:-/tmp/pd-smoke}"
mkdir -p "$WORK_DIR"

cat <<'SRC' > "$WORK_DIR/smoke.c"
#include <stdio.h>
int main(void) {
    puts("pd-smoke");
    return 0;
}
SRC

"$TC_BIN/clang" --sysroot="$SDK_ROOT" -o "$WORK_DIR/smoke" "$WORK_DIR/smoke.c"

if [[ ! -f "$WORK_DIR/smoke" ]]; then
  echo "Smoke test failed: binary not produced"
  exit 1
fi

echo "Smoke test OK: $WORK_DIR/smoke"
