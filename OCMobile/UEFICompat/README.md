UEFI Compatibility Layer (UEFICompat)
====================================

What this is
------------
This folder provides a tiny userspace "UEFI compatibility" shim intended to
be installed into an Android device's `boot.img` ramdisk. It is NOT a full
UEFI firmware implementation. Instead it provides:

- `compat` — a tiny daemon that watches `/uefi` and will exec a payload if
  present. Useful as an early-launch point for a userspace UEFI-emulation
  helper or other boot-time payloads.
- `install_to_ramdisk.sh` — helper script to copy built artifacts into an
  extracted ramdisk directory.
- `uefi_compat.rc` — an `init.rc` snippet that starts the shim early.

Usage
-----
1. Build the shim for your target device (arm64) from the repo root:

```sh
make uefi-compat CROSS_PREFIX=aarch64-none-elf-
```

2. Extract your `boot.img` ramdisk using your normal workflow. Repack after
   modifications.

3. Run the installer against the extracted ramdisk directory:

```sh
OCMobile/UEFICompat/install_to_ramdisk.sh /path/to/extracted/ramdisk
```

4. Merge the `uefi_compat.rc` snippet into the ramdisk's `init.rc` (or place
   it where your device's init loads additional rc snippets).

5. Repack and flash the `boot.img` (use `mkbootimg`, `abootimg`, or your
   distro-specific tools).

Limitations
-----------
- This shim does not provide EFI runtime services or UEFI protocol layers.
- The intended payload is a userspace binary that understands how to run
  in this environment; an unmodified `.efi` PE file will not be executable
  on Android unless you provide a compatible interpreter or loader.

If you want a lower-level firmware replacement (OEM/bootloader level), this
repository can be extended, but that is device-specific and out of scope for
the simple ramdisk-compatible shim here.

Security
--------
Be careful when modifying `boot.img` and flashing to devices — incorrect
images can brick devices. Test on a spare device or emulator when possible.
