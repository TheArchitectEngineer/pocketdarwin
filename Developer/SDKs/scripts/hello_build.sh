#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
TOOLCHAIN_BIN="$($ROOT_DIR/Developer/SDKs/scripts/pd_toolchain_path.sh)"

WORK_DIR="${1:-/tmp/pocketdarwin-hello}"
mkdir -p "$WORK_DIR"

cat <<'SRC' > "$WORK_DIR/hello.c"
#include <stdio.h>
int main(void) {
    printf("PocketDarwin hello!\n");
    return 0;
}
SRC

"$TOOLCHAIN_BIN/clang" -o "$WORK_DIR/hello" "$WORK_DIR/hello.c"

echo "Built: $WORK_DIR/hello"
