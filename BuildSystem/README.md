# PocketDarwin Build System (Swift)

This is a standalone Swift script that orchestrates project builds across platforms.

Usage:
```
./BuildSystem/pdbuild.swift <command>
```

Commands:
- `interlude`        Build the interlude loader
- `sdk-validate`     Validate the SDK sysroot
- `sdk-populate`     Populate the SDK sysroot (requires --headers and --libs)
- `installer-build`  Build the macOS installer app (macOS only)
- `installer-dmg`    Build the macOS installer DMG (macOS only)
- `all`              Run interlude + sdk-validate (+ installer on macOS)

Examples:
```
./BuildSystem/pdbuild.swift interlude
./BuildSystem/pdbuild.swift sdk-validate
./BuildSystem/pdbuild.swift sdk-populate --headers /path/to/headers --libs /path/to/libs
./BuildSystem/pdbuild.swift installer-build --config Release --derived /tmp/PDDerivedData
./BuildSystem/pdbuild.swift installer-dmg
```
