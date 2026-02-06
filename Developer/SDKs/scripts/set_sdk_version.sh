#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
SDK_SETTINGS="$ROOT_DIR/Developer/SDKs/PocketDarwin.sdk/SDKSettings.json"

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <version>"
  exit 1
fi

VERSION="$1"

python3 - "$SDK_SETTINGS" "$VERSION" <<'PY'
import json
import sys

path = sys.argv[1]
version = sys.argv[2]

with open(path, 'r', encoding='utf-8') as f:
    data = json.load(f)

for key in ("Version", "DefaultProperties"):
    if key not in data:
        data[key] = {}

data["Version"] = version
props = data.get("DefaultProperties", {})
props["SDK_VERSION"] = version
props["SDK_PRODUCT_BUILD_VERSION"] = version
props["SDK_NAME"] = "pocketdarwin"
props["SDKROOT"] = "pocketdarwin"
data["DefaultProperties"] = props

with open(path, 'w', encoding='utf-8') as f:
    json.dump(data, f, indent=2)
    f.write("\n")
PY

echo "Updated SDKSettings.json to version $VERSION"
