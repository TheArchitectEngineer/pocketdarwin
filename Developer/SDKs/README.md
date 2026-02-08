# PocketDarwin SDKs & Toolchains

This folder is the canonical home for PocketDarwin SDKs and toolchains.

Goals:
- Xcode-compatible SDK/toolchain on macOS
- Cross-platform builds (Linux/Windows/BSD) using the same sysroot layout

Reality check:
- Xcode only runs on macOS; for Linux/Windows/BSD we mirror the SDK layout and use clang/ld directly.
- We keep the same sysroot path layout to reduce conditional logic in build scripts.

## Layout
```
Developer/SDKs/
  PocketDarwin.sdk/
    SDKSettings.json
    usr/
      include/
      lib/
      libexec/
      share/
  Toolchains/
    PocketDarwin.xctoolchain/
      ToolchainInfo.plist
      usr/
        bin/
        lib/
        share/
```

## SDK Settings (macOS/Xcode)
`PocketDarwin.sdk/SDKSettings.json` is used by Xcode to recognize the SDK.

## Toolchain Info (macOS/Xcode)
`Toolchains/PocketDarwin.xctoolchain/ToolchainInfo.plist` declares the toolchain for Xcode.

## Cross-platform builds
On Linux/Windows/BSD, point clang to the sysroot:
```
clang --sysroot=/path/to/PocketDarwin/Developer/SDKs/PocketDarwin.sdk \
  --target=arm64-apple-darwin \
  -isystem /path/to/PocketDarwin/Developer/SDKs/PocketDarwin.sdk/usr/include
```

## Wrapper scripts
Wrapper binaries live under:
`Developer/SDKs/Toolchains/PocketDarwin.xctoolchain/usr/bin`

They set `--target` and `--sysroot` automatically.

## Cross-platform helpers
These helpers pick the right toolchain on any host OS:
```
Developer/SDKs/scripts/pd_cc.sh
Developer/SDKs/scripts/pd_cxx.sh
Developer/SDKs/scripts/pd_ld.sh
```

## Populate sysroot
Use the helper script to sync headers/libs from a build output:
```
Developer/SDKs/scripts/populate_sysroot.sh /path/to/headers /path/to/libs
```

## pkg-config stubs
Placeholder `.pc` files live in:
`Developer/SDKs/PocketDarwin.sdk/usr/lib/pkgconfig`

## pkg-config wrapper
Use the toolchain `pkg-config` wrapper to force the PocketDarwin sysroot:
```
Developer/SDKs/Toolchains/PocketDarwin.xctoolchain/usr/bin/pkg-config --libs pocketdarwin
```

## Validate sysroot
```
Developer/SDKs/scripts/validate_sysroot.sh
```
## Sysroot manifest
```
Developer/SDKs/scripts/sysroot_manifest.sh
Developer/SDKs/scripts/verify_sysroot.sh
```

## Hello world build
```
Developer/SDKs/scripts/hello_build.sh /tmp/pd-hello
```
## Toolchain smoke test
```
Developer/SDKs/scripts/toolchain_smoke_test.sh /tmp/pd-smoke
```

## Toolchain env helper
```
source Developer/SDKs/scripts/pd_env.sh
```
## Toolchain path helper
```
Developer/SDKs/scripts/pd_toolchain_path.sh
```

## pkg-config env helper
```
source Developer/SDKs/scripts/pd_pkgconfig_env.sh
```

## Set SDK version
```
Developer/SDKs/scripts/set_sdk_version.sh 0.1
```

## Target presets
```
source Developer/SDKs/scripts/pd_target_env.sh arm64
```

## CMake toolchain file
Use:
```
cmake -DCMAKE_TOOLCHAIN_FILE=Developer/SDKs/Toolchains/pocketdarwin.toolchain.cmake ..
```

## Next steps
- Populate `usr/include` and `usr/lib` with headers/libs from the PocketDarwin build.
- Add wrapper scripts in `Toolchains/PocketDarwin.xctoolchain/usr/bin`.
- Decide on target triples and minimum OS versions.
