#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <arm64|armv7>"
  exit 1
fi

case "$1" in
  arm64)
    export PD_TARGET_TRIPLE=arm64-apple-darwin
    ;;
  armv7)
    export PD_TARGET_TRIPLE=armv7-apple-darwin
    ;;
  *)
    echo "Unsupported target: $1"
    exit 1
    ;;
esac

echo "PD_TARGET_TRIPLE=$PD_TARGET_TRIPLE"
