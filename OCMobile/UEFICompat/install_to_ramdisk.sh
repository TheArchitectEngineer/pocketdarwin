#!/bin/sh
set -e

usage() {
  cat <<EOF
Usage: $0 <ramdisk-dir> [compat-binary] [efi-payload]

Copies the built uefi-compat shim and an optional EFI payload into an
extracted boot ramdisk directory.

Arguments:
  ramdisk-dir     Path to extracted ramdisk directory (cpio root)
  compat-binary   (optional) path to built compat binary (default: build/uefi_compat/compat)
  efi-payload     (optional) path to EFI payload to place at /uefi/OCMobile.efi (default: build/uefi/OCMobile.efi)

This script does not repack the boot image. After running it, repack the
ramdisk with your normal workflow (mkbootimg, abootimg, etc.).
EOF
}

if [ $# -lt 1 ]; then usage; exit 1; fi

RAMDIR=$1
COMPAT=${2:-build/uefi_compat/compat}
EFI=${3:-build/uefi/OCMobile.efi}

if [ ! -d "$RAMDIR" ]; then echo "ramdisk directory not found: $RAMDIR"; exit 1; fi

mkdir -p "$RAMDIR/uefi"

if [ -f "$COMPAT" ]; then
  cp "$COMPAT" "$RAMDIR/uefi/compat"
  chmod 0755 "$RAMDIR/uefi/compat"
  echo "Installed compat -> $RAMDIR/uefi/compat"
else
  echo "Warning: compat binary not found at $COMPAT (run 'make uefi-compat')"
fi

if [ -f "$EFI" ]; then
  cp "$EFI" "$RAMDIR/uefi/OCMobile.efi"
  chmod 0644 "$RAMDIR/uefi/OCMobile.efi"
  echo "Installed EFI payload -> $RAMDIR/uefi/OCMobile.efi"
else
  echo "No EFI payload found at $EFI (optional)"
fi

echo "Note: add the provided init snippet (uefi_compat.rc) into your ramdisk's init sequence or merge manually. Repack ramdisk afterwards."
