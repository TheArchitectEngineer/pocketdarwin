#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
SDK_ROOT="$ROOT_DIR/Developer/SDKs/PocketDarwin.sdk"
MANIFEST="$SDK_ROOT/manifest.sha256"

if [[ ! -f "$MANIFEST" ]]; then
  echo "Manifest not found: $MANIFEST"
  exit 1
fi

python3 - "$SDK_ROOT" "$MANIFEST" <<'PY'
import hashlib
import os
import sys

root = sys.argv[1]
manifest = sys.argv[2]

expected = {}
with open(manifest, 'r', encoding='utf-8') as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        h, rel = line.split(None, 1)
        expected[rel.strip()] = h

failed = False
for rel, h in expected.items():
    path = os.path.join(root, rel)
    if not os.path.exists(path):
        print(f"missing: {rel}")
        failed = True
        continue
    with open(path, 'rb') as f:
        data = f.read()
    cur = hashlib.sha256(data).hexdigest()
    if cur != h:
        print(f"mismatch: {rel}")
        failed = True

if failed:
    sys.exit(1)
print("Sysroot manifest OK")
PY
