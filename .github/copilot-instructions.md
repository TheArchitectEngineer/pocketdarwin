# PocketDarwin AI Coding Guidelines

## Project Overview
PocketDarwin is a project to run Darwin (macOS/iOS kernel and runtime) on Android devices. It consists of:
- **OCMobile**: Mobile bootloader (OpenCore-inspired) in freestanding C, handles booting from disk images, config files, and kext loading.
- **Kernel**: Darwin kernel source.
- **Kexts**: Kernel extensions, e.g., AndroidPlatformBridge for Android hardware abstraction.
- **objc4**: Apple's Objective-C runtime.
- **Userland**: Core utilities and managers.

Architecture: Bootloader → Kernel → Kexts → Runtime → Userland. Data flows via boot params, IOKit services, and ObjC messaging.

## Key Patterns
- **Freestanding Code**: Low-level components (OCMobile, kernel) use no stdlib; include `<stdint.h>`, `<stddef.h>`. Example: [OCMobile/loader.c](OCMobile/loader.c) checks boot magic and panics.
- **IOKit Kexts**: Extensions inherit `IOService`, use `IOLog()` for logging, `setProperty()` for config. Example: [Kexts/AndroidPlatformBridge/AndroidPlatformBrigde.cpp](Kexts/AndroidPlatformBridge/AndroidPlatformBrigde.cpp) publishes platform properties.
- **Build System**: Makefiles with `clang`/`clang++`, freestanding flags (`-ffreestanding`). Run `bash droidBuildEnv.sh` to set Termux environment on Android.
- **No Stdlib in Low-Level**: Avoid `printf`, use custom `ocm_console_putc` in [OCMobile/loader.c](OCMobile/loader.c).
- **Config Files**: OCMobile uses plist-like config (future), kexts load via `Kextld.c`.

## Workflows
- **Build**: `make` in component dirs (e.g., `OCMobile/Makefile`). objc4 uses Xcode project.
- **Debug**: IOLog output in kexts; OCMobile prints to console.
- **Test**: No formal tests; prototype panics on success in loader.

## Conventions
- Headers: `.hpp` for C++, `.h` for C.
- Naming: `ocm_` prefix for OCMobile functions.
- Comments: Descriptive, e.g., "opaque for now" in structs.

Reference: [TODO](TODO) for current tasks; [README.md](README.md) for vision.</content>
<parameter name="filePath">/workspaces/PocketDarwin/.github/copilot-instructions.md