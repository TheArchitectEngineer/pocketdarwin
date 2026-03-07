# PocketDarwin Build System (Swift)

This is a standalone Swift script that orchestrates project builds across platforms and can now generate portable `.System` bundles.

Usage:
```sh
./BuildSystem/pdbuild.swift <command>
```

Commands:
- `interlude`        Build the interlude loader
- `sdk-validate`     Validate the SDK sysroot
- `sdk-populate`     Populate the SDK sysroot (requires `--headers` and `--libs`)
- `installer-build`  Build the macOS installer app (macOS only)
- `installer-dmg`    Build the macOS installer DMG (macOS only)
- `system-bundle`    Build System components and emit a `.System` bundle with metadata
- `all`              Run interlude + sdk-validate (+ installer on macOS)

## System bundle output naming

`system-bundle` emits a bundle named with:
- current git branch,
- UTC build date/time (`yyyyMMdd-HHmmss`),
- random codename (`Adjective-Noun`)

Example:

```text
PocketDarwin-main-20260307-120501-Aurora-Falcon.System
```

Inside the output bundle:
- `Contents/Info.plist` includes the same metadata
- payload is arranged under `Contents/System/{Applications,Libraries,Services,Programs}`

## iPhoneOS 2.0 cross-compiling

`system-bundle` supports using an iPhoneOS 2.0 sysroot for cross-compiling make-based components.

Examples:

```sh
# Explicit SDK path
./BuildSystem/pdbuild.swift system-bundle --sdk-root /path/to/iPhoneOS2.0.sdk

# Repository shortcut (Developer/SDKs/iPhoneOS2.0.sdk)
./BuildSystem/pdbuild.swift system-bundle --iphoneos2-sdk

# Custom output directory
./BuildSystem/pdbuild.swift system-bundle --iphoneos2-sdk --output-dir build/releases
```
