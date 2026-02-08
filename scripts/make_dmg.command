#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DERIVED_DATA="${DERIVED_DATA:-/var/folders/ln/16b2srns5354wpjrk4xq09180000gn/T/PocketDarwinDerivedData}"
CONFIGURATION="${CONFIGURATION:-Release}"

PROJECT_PATH="$ROOT_DIR/Platform/macOS/Install PocketDarwin/Install PocketDarwin.xcodeproj"
APP_NAME="Install PocketDarwin"
APP_PATH="$DERIVED_DATA/Build/Products/$CONFIGURATION/$APP_NAME.app"
DMG_OUTPUT="${DMG_OUTPUT:-$ROOT_DIR/Install-PocketDarwin.dmg}"
STAGING_DIR="${STAGING_DIR:-/tmp/PocketDarwinDMG}"

xcodebuild -project "$PROJECT_PATH" \
  -scheme "$APP_NAME" \
  -configuration "$CONFIGURATION" \
  -sdk macosx \
  -derivedDataPath "$DERIVED_DATA" \
  build

if [[ ! -d "$APP_PATH" ]]; then
  echo "App not found: $APP_PATH"
  exit 1
fi

rm -rf "$STAGING_DIR"
mkdir -p "$STAGING_DIR"
cp -R "$APP_PATH" "$STAGING_DIR/"

hdiutil create -volname "$APP_NAME" \
  -srcfolder "$STAGING_DIR" \
  -ov -format UDZO \
  "$DMG_OUTPUT"

echo "DMG created: $DMG_OUTPUT"
