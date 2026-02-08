#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
SDK_ROOT="$ROOT_DIR/Developer/SDKs/PocketDarwin.sdk"
MANIFEST="$SDK_ROOT/manifest.sha256"

python3 - "$SDK_ROOT" "$MANIFEST" <<'PY'
import hashlib
import os
import sys

root = sys.argv[1]
manifest = sys.argv[2]

entries = []
for dirpath, _, filenames in os.walk(root):
    for name in filenames:
        if name == os.path.basename(manifest):
            continue
        path = os.path.join(dirpath, name)
        rel = os.path.relpath(path, root)
        with open(path, 'rb') as f:
            data = f.read()
        h = hashlib.sha256(data).hexdigest()
        entries.append((rel, h))

entries.sort()
with open(manifest, 'w', encoding='utf-8') as f:
    for rel, h in entries:
        f.write(f"{h}  {rel}\n")

print(f"Wrote manifest: {manifest}")
PY
