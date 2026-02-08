<div align="center">
  <img src="logo.png" alt="PocketDarwin Logo" width="240" />
  <h1>PocketDarwin</h1>
  <p>I'm the crazy guy who's gonna clone iOS! — OakyMacintosh</p>
</div>

## What This Repo Is
PocketDarwin is an experiment to boot Darwin on Android hardware.

## WIP Warnings
- This project is highly experimental and incomplete.
- Expect breakage, missing features, and frequent changes.
- Do not use on devices you rely on; it may soft‑brick or require full reflashing.

## SDKs & Toolchains
SDKs and toolchains live under `Developer/SDKs/`. The layout is Xcode‑compatible on macOS and mirrored for cross‑platform builds.

Quick view:
```
Developer/SDKs/
  PocketDarwin.sdk/
  Toolchains/PocketDarwin.toolchain/
```

Details: see `Developer/SDKs/README.md`.
Quick build with CMake:
```sh
cmake -DCMAKE_TOOLCHAIN_FILE=Developer/SDKs/Toolchains/pocketdarwin.toolchain.cmake ..
```
Populate the sysroot (headers/libs):
```sh
Developer/SDKs/scripts/populate_sysroot.sh /path/to/headers /path/to/libs
```
Validate the sysroot:
```sh
Developer/SDKs/scripts/validate_sysroot.sh
```
Quick hello-world build:
```sh
Developer/SDKs/scripts/hello_build.sh /tmp/pd-hello
```
Cross-platform helpers:
```sh
Developer/SDKs/scripts/pd_cc.sh
Developer/SDKs/scripts/pd_cxx.sh
Developer/SDKs/scripts/pd_ld.sh
```
Toolchain env helper:
```sh
source Developer/SDKs/scripts/pd_env.sh
```
Toolchain path helper:
```sh
Developer/SDKs/scripts/pd_toolchain_path.sh
```
pkg-config env helper:
```sh
source Developer/SDKs/scripts/pd_pkgconfig_env.sh
```
Set SDK version:
```sh
Developer/SDKs/scripts/set_sdk_version.sh 0.1
```
Target preset:
```sh
source Developer/SDKs/scripts/pd_target_env.sh arm64
```
Sysroot manifest:
```sh
Developer/SDKs/scripts/sysroot_manifest.sh
Developer/SDKs/scripts/verify_sysroot.sh
```
Toolchain smoke test:
```sh
Developer/SDKs/scripts/toolchain_smoke_test.sh /tmp/pd-smoke
```

## macOS installer DMG
```sh
scripts/make_dmg.sh
```

## Build system (Swift)
```sh
./BuildSystem/pdbuild.swift help
```

## Interlude (Linux Boot Loader)
Interlude runs on the device to boot the Darwin kernel. It is a minimal Android Linux kernel build, just enough to bring up Darwin, and it is started automatically by `init`.

### Build (with DTB support via libfdt)
```sh
make -C System/Boot/interlude \
  L2D_WITH_LIBFDT=1 \
  LIBFDT_CFLAGS="-I/path/to/libfdt/include" \
  LIBFDT_LDFLAGS="-L/path/to/libfdt/lib"
```

### Inspect DTB (UART + framebuffer)
```sh
System/Boot/interlude/l2d_dump path/to/your.dtb
```

### Run interlude on device (via init)
Interlude is started automatically by Android `init` on the device. A typical `init.rc` entry looks like this:
```rc
service darwin_interlude /sbin/darwin_interlude --dtb /path/to/your.dtb --exec
    class main
    user root
    group root
    oneshot

on boot
    start darwin_interlude
```
The Darwin kernel path is read from `OCMobileConfig.json` on the SD card.

### OCMobileConfig.json example
```json
{
  "darwinKernelPath": "/sdcard/PocketDarwin/mach_kernel",
  "Kexts": [
    "/sdcard/PocketDarwin/Kexts/UARTDriver.kext",
    "/sdcard/PocketDarwin/Kexts/FramebufferDriver.kext"
  ],
  "Platform": {
    "ExternalStorageRoot": "/sdcard",
    "MobileEnvironmentRoot": "/System/Mobile Enviroment",
    "InternalStorageAllowed": false
  },
  "Boot": {
    "Kernel": "/sdcard/PocketDarwin/mach_kernel"
  }
}
```

### OCMobileConfig.json writing guide
- Place the file at `/sdcard/OCMobileConfig.json` (default path used by interlude).
- Provide **one** of these keys (top level or under `Boot`):
  - `darwinKernelPath`
  - `darwin_kernel_path`
  - `darwinKernel`
  - `darwin_kernel`
  - `Kernel` (under `Boot`)
- Use an absolute path that exists on the device (example: `/sdcard/PocketDarwin/mach_kernel`).
 
### Platform overrides
- `Platform.ExternalStorageRoot`: override the SD card mount path passed to Darwin.
- `Platform.MobileEnvironmentRoot`: override the mobile framework root.
- `Platform.InternalStorageAllowed`: keep `false` to block internal storage by default.

### Kexts (layout guidance)
- Place kext bundles under a dedicated folder on the SD card, for example:
  - `/sdcard/PocketDarwin/Kexts/`
- List each kext bundle path in `OCMobileConfig.json` under `Kexts`.
- This is a documentation-only convention for now; actual loading will be wired later.

### AndroidDeviceGeneralController helper (user space)
You can inspect the published properties from user space:
```sh
make -C System/Drivers/AndroidDeviceGeneralController/Tools
System/Drivers/AndroidDeviceGeneralController/Tools/adgc_dump
```
For JSON output:
```sh
System/Drivers/AndroidDeviceGeneralController/Tools/adgc_dump --json
```

### Mobile Controller framework (user space)
The `Mobile Controller` framework exposes the bridge info to apps:
```objc
NSString *sdRoot = MCExternalStorageRoot();
NSString *mobileRoot = MCMobileEnvironmentRoot();
BOOL allowInternal = MCInternalStorageAllowed();
uint64_t fbBase = MCFbBase();
```
Bulk fetch:
```objc
MCMobileInfo info;
if (MCGetMobileInfo(&info)) {
    NSString *sdRoot = info.externalStorageRoot;
}
```
Swift helper:
```swift
if let info = getMobileInfo() {
    print(info.externalStorageRoot)
}
```
Objective-C helper:
```objc
MCMobileEnvironmentInfo *info = [MCMobileEnvironmentInfo currentInfo];
```
