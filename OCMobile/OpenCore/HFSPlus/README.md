HFS+ (HFS Plus) Port Roadmap
=================================

Goal
----
Provide read-write HFS+ support usable from OCMobile running on Android devices.

Recommended approach
--------------------
- Prefer userspace implementation for rapid development and safety (no kernel patches).
- Use an existing library/toolset (libhfs/libhfsplus from hfsprogs or libhfsp) and wrap it for Android/arm64.
- Allow optional kernel module path later (if performance or block-device integration needed).

This folder provides scaffolding and a minimal userspace stub `hfs_rw_stub` that
will be built if a host HFS library is available. It also documents how to
integrate a real HFS implementation.

Dependencies
------------
- Prefer: `libhfs` or `libhfsplus` (from hfsprogs/hfsprogs-ng). If you have
  the library compiled for `aarch64` place headers in a known include path and
  the library in a known lib path and set `HFS_CFLAGS` and `HFS_LDFLAGS` when
  running the Makefile.

Quick build (host test):

  make -C OCMobile/OpenCore/HFSPlus HFS_CFLAGS="-I/usr/local/include" HFS_LDFLAGS="-L/usr/local/lib -lhfs"

Next steps to fully port OpenCore HFS+ driver
--------------------------------------------
1. Acquire OpenCore HFS+ driver sources (OpenCore's HFS driver is under
   appropriate OpenCore repo). Port driver helper functions into a small
   platform layer that uses `Platform/OpenPartitionDxe` for block access.
2. Implement UEFI protocol shim or userspace bridge exposing necessary
   read/write APIs for mounting macOS volumes.
3. Test on device with spare storage and ensure journaling/integrity features
   are preserved.

Security note
-------------
HFS write support can corrupt volumes if used incorrectly; always test on
non-critical images and keep backups.
