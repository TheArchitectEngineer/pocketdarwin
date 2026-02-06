#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
INTERLUDE_DIR="$ROOT_DIR/Boot/interlude"

say() {
  printf "%s\n" "$*"
}

prompt() {
  local message="$1"
  local default_value="${2:-}"
  local input
  if [[ -n "$default_value" ]]; then
    printf "%s [%s]: " "$message" "$default_value" >&2
  else
    printf "%s: " "$message" >&2
  fi
  IFS= read -r input || true
  if [[ -z "$input" ]]; then
    input="$default_value"
  fi
  # Normalize drag-n-drop paths from macOS Terminal (quotes and backslash escapes)
  input="${input#\"}"
  input="${input%\"}"
  if [[ "$input" == *\\* ]]; then
    input="$(printf '%b' "$input")"
  fi
  printf "%s" "$input"
}

confirm() {
  local message="$1"
  local default_value="${2:-y}"
  local input
  printf "%s [%s]: " "$message" "$default_value" >&2
  IFS= read -r input || true
  if [[ -z "$input" ]]; then
    input="$default_value"
  fi
  case "$input" in
    y|Y|yes|YES) return 0;;
    *) return 1;;
  esac
}

pick_first_existing() {
  local path
  for path in "$@"; do
    if [[ -f "$path" ]]; then
      printf "%s" "$path"
      return 0
    fi
  done
  return 1
}

add_interlude_to_init() {
  local init_file="$1"
  if rg -q "darwin_interlude" "$init_file"; then
    say "Init already references darwin_interlude: $init_file"
    return 0
  fi

  cat <<'EOT' >> "$init_file"

service darwin_interlude /sbin/darwin_interlude
    class main
    user root
    group root
    oneshot

on boot
    start darwin_interlude
EOT

  say "Appended interlude service to init: $init_file"
}

say "PocketDarwin Interlude Boot Image Builder"
say "Drag-and-drop your Android kernel source directory and press Enter."

KERNEL_DIR="$(prompt "Kernel source directory")"
if [[ -z "$KERNEL_DIR" || ! -d "$KERNEL_DIR" ]]; then
  say "Invalid kernel directory: $KERNEL_DIR"
  exit 1
fi

ARCH_DEFAULT=""
if [[ -d "$KERNEL_DIR/arch/arm64" ]]; then
  ARCH_DEFAULT="arm64"
elif [[ -d "$KERNEL_DIR/arch/arm" ]]; then
  ARCH_DEFAULT="arm"
fi
ARCH="$(prompt "Kernel ARCH" "$ARCH_DEFAULT")"
CROSS_COMPILE="$(prompt "CROSS_COMPILE prefix (optional, e.g. aarch64-linux-android-)" "")"
DEFCONFIG="$(prompt "Defconfig (optional, e.g. vendor/device_defconfig)" "")"
EXTRA_MAKE_ARGS="$(prompt "Extra make args (optional)" "")"

say "\nBuilding kernel..."
MAKE_ENV=("ARCH=$ARCH")
if [[ -n "$CROSS_COMPILE" ]]; then
  MAKE_ENV+=("CROSS_COMPILE=$CROSS_COMPILE")
fi

if [[ -n "$DEFCONFIG" ]]; then
  (cd "$KERNEL_DIR" && env "${MAKE_ENV[@]}" make "$DEFCONFIG")
fi

CPU_COUNT=$(getconf _NPROCESSORS_ONLN || sysctl -n hw.ncpu || echo 4)
(cd "$KERNEL_DIR" && env "${MAKE_ENV[@]}" make -j"$CPU_COUNT" $EXTRA_MAKE_ARGS)

KERNEL_IMAGE=""
if [[ "$ARCH" == "arm64" ]]; then
  KERNEL_IMAGE=$(pick_first_existing \
    "$KERNEL_DIR/arch/arm64/boot/Image" \
    "$KERNEL_DIR/arch/arm64/boot/Image.gz" \
    "$KERNEL_DIR/arch/arm64/boot/Image.lz4" \
    "$KERNEL_DIR/arch/arm64/boot/Image.gz-dtb" \
    "$KERNEL_DIR/arch/arm64/boot/Image-dtb" \
  ) || true
else
  KERNEL_IMAGE=$(pick_first_existing \
    "$KERNEL_DIR/arch/arm/boot/zImage" \
    "$KERNEL_DIR/arch/arm/boot/zImage-dtb" \
    "$KERNEL_DIR/arch/arm/boot/Image" \
  ) || true
fi

if [[ -z "$KERNEL_IMAGE" ]]; then
  say "Failed to locate kernel image in build output."
  exit 1
fi

say "Kernel image: $KERNEL_IMAGE"

say "\nModify init to launch interlude on boot?"
if confirm "Modify init" "y"; then
  DEFAULT_INIT=""
  if [[ -f "$KERNEL_DIR/init.rc" ]]; then
    DEFAULT_INIT="$KERNEL_DIR/init.rc"
  else
    DEFAULT_INIT=$(find "$KERNEL_DIR" -maxdepth 6 -name "init.rc" -print -quit 2>/dev/null || true)
  fi
  INIT_FILE="$(prompt "Path to init.rc" "$DEFAULT_INIT")"
  if [[ -n "$INIT_FILE" && -f "$INIT_FILE" ]]; then
    add_interlude_to_init "$INIT_FILE"
  else
    say "init.rc not found; skipping init modification."
  fi
fi

RAMDISK_DIR="$(prompt "Ramdisk root directory to place interlude binary (optional)" "")"
if [[ -n "$RAMDISK_DIR" && -d "$RAMDISK_DIR" ]]; then
  INTERLUDE_CC="$(prompt "Compiler for interlude (optional, e.g. aarch64-linux-android-clang)" "")"
  say "Building interlude binary..."
  if [[ -n "$INTERLUDE_CC" ]]; then
    (cd "$INTERLUDE_DIR" && make CC="$INTERLUDE_CC")
  else
    (cd "$INTERLUDE_DIR" && make)
  fi

  INSTALL_DIR="$RAMDISK_DIR/sbin"
  mkdir -p "$INSTALL_DIR"
  cp "$INTERLUDE_DIR/darwin_interlude" "$INSTALL_DIR/darwin_interlude"
  chmod 0755 "$INSTALL_DIR/darwin_interlude"
  say "Installed interlude binary to $INSTALL_DIR/darwin_interlude"
else
  say "No ramdisk directory provided; skipping interlude binary install."
fi

RAMDISK_IMG=""
if [[ -n "$RAMDISK_DIR" && -d "$RAMDISK_DIR" ]]; then
  if command -v mkbootfs >/dev/null 2>&1; then
    if confirm "Build ramdisk image from directory?" "y"; then
      RAMDISK_IMG="$(prompt "Ramdisk image output path" "$ROOT_DIR/Boot/interlude/ramdisk.img")"
      mkbootfs "$RAMDISK_DIR" | gzip > "$RAMDISK_IMG"
      say "Built ramdisk image: $RAMDISK_IMG"
    fi
  else
    say "mkbootfs not found; cannot build ramdisk image automatically."
  fi
fi

if command -v mkbootimg >/dev/null 2>&1; then
  if confirm "Build boot.img using mkbootimg?" "y"; then
    if [[ -z "$RAMDISK_IMG" ]]; then
      RAMDISK_IMG="$(prompt "Path to ramdisk image" "")"
    fi
    if [[ -z "$RAMDISK_IMG" || ! -f "$RAMDISK_IMG" ]]; then
      say "Ramdisk image not found; cannot build boot.img."
      exit 1
    fi

    CMDLINE="$(prompt "Kernel cmdline (optional)" "")"
    BASE="$(prompt "mkbootimg base (optional)" "")"
    PAGESIZE="$(prompt "mkbootimg pagesize (optional)" "")"
    BOOT_OUT="$(prompt "boot.img output path" "$ROOT_DIR/Boot/interlude/boot.img")"

    MKBOOTIMG_ARGS=(--kernel "$KERNEL_IMAGE" --ramdisk "$RAMDISK_IMG" --output "$BOOT_OUT")
    if [[ -n "$CMDLINE" ]]; then
      MKBOOTIMG_ARGS+=(--cmdline "$CMDLINE")
    fi
    if [[ -n "$BASE" ]]; then
      MKBOOTIMG_ARGS+=(--base "$BASE")
    fi
    if [[ -n "$PAGESIZE" ]]; then
      MKBOOTIMG_ARGS+=(--pagesize "$PAGESIZE")
    fi

    mkbootimg "${MKBOOTIMG_ARGS[@]}"
    say "Built boot image: $BOOT_OUT"
  fi
else
  say "mkbootimg not found; skipping boot image creation."
fi

say "\nDone."
