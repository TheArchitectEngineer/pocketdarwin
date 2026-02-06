#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
TC_BIN="$($ROOT_DIR/Developer/SDKs/scripts/pd_toolchain_path.sh)"

exec "$TC_BIN/ld.lld" "$@"
