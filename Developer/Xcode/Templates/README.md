# PocketDarwin Xcode Templates

## System Bundle Template

This repository includes a project template at:

`Project Templates/macOS/PocketDarwin/System Bundle.xctemplate`

Install it by copying `Project Templates` into one of:

- `~/Library/Developer/Xcode/Templates/`
- `/Library/Developer/Xcode/Templates/`

Then restart Xcode. You should find **System Bundle** under the macOS templates in the **PocketDarwin** group.

### Template defaults

- Product type: `com.apple.product-type.bundle`
- Bundle extension: `.System` (customizable)
- Install path: `$(LOCAL_LIBRARY_DIR)/PocketDarwin/SystemBundles` (customizable)
- No source code scaffolding by default
- Optional starter layout:
  - `SystemBundle.plist`
  - `Executables/README.txt`
  - `Libraries/README.txt`
