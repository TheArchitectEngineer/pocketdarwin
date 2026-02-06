
# Cross-toolchain prefix (e.g. aarch64-none-elf-)
CROSS_PREFIX ?=

# Device tree (DTB) to bundle into build
DEVICE_TREE ?=

# macOS SDK bundle path (optional; when provided adds -isysroot)
MACOS_SDK ?=

BUILD ?= build

# Source files (root of OCMobile). Add/remove as needed.
SRCS := loader.c \
	bootlib.c \
	BootMenu.c \
	Framebuffer.c \
	kernel_patcher.c \
	platform.c \
	ACPIParser.c

OBJS := $(patsubst %.c,$(BUILD)/%.o,$(SRCS))

ifeq ($(CROSS_PREFIX),)
CC := clang
CXX := clang++
AS := nasm
else
CC := $(CROSS_PREFIX)gcc
CXX := $(CROSS_PREFIX)g++
AS := $(CROSS_PREFIX)as
endif

CFLAGS := -Wall -Wextra -ffreestanding -fno-builtin -nostdlib -I.
ifdef MACOS_SDK
CFLAGS += -isysroot $(MACOS_SDK)
endif

LDFLAGS := -static

OUT := $(BUILD)/ocmobile.elf

.PHONY: all clean help device_tree

all: $(OUT) device_tree

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

device_tree:
	@if [ -z "$(DEVICE_TREE)" ]; then \
		echo "No DEVICE_TREE set; skipping device_tree target"; \
	else \
		mkdir -p $(BUILD); cp $(DEVICE_TREE) $(BUILD)/device_tree.dtb; \
		echo "Copied $(DEVICE_TREE) -> $(BUILD)/device_tree.dtb"; \
	fi

clean:
	@rm -rf $(BUILD)

help:
	@echo "Usage: make DEVICE_TREE=path MACOS_SDK=path CROSS_PREFIX=prefix"
	@echo "Examples:" \
		"  make DEVICE_TREE=/path/to/my.dtb" \
		"  make CROSS_PREFIX=aarch64-none-elf- DEVICE_TREE=dtb.dtb"

# ----- Platform drivers -----
# Build Platform/OpenPartitionDxe into a static archive
PLATFORM_SRCS := $(wildcard Platform/OpenPartitionDxe/*.c)
PLATFORM_OBJS := $(patsubst Platform/OpenPartitionDxe/%.c,$(BUILD)/platform/%.o,$(PLATFORM_SRCS))

$(BUILD)/platform:
	@mkdir -p $(BUILD)/platform

$(BUILD)/platform/%.o: Platform/OpenPartitionDxe/%.c | $(BUILD)/platform
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: platform
platform: $(BUILD)/libplatform.a

$(BUILD)/libplatform.a: $(PLATFORM_OBJS)
	ar rcs $@ $^

# ----- x64.debug (host testing) -----
X64_BUILD := $(BUILD)/x64.debug
X64_CFLAGS := -m64 -g -O0 -ffreestanding -fno-builtin -nostdlib -I.
X64_ASFLAGS := -f elf64

X64_SRCS := $(wildcard arch/genericintel/*.c)
X64_OBJS := $(patsubst arch/genericintel/%.c,$(X64_BUILD)/%.o,$(X64_SRCS))
X64_ASMS := $(wildcard arch/genericintel/*.asm)
X64_ASM_OBJS := $(patsubst arch/genericintel/%.asm,$(X64_BUILD)/%.o,$(X64_ASMS))

$(X64_BUILD):
	@mkdir -p $(X64_BUILD)

$(X64_BUILD)/%.o: arch/genericintel/%.c | $(X64_BUILD)
	clang $(X64_CFLAGS) -c $< -o $@

$(X64_BUILD)/%.o: arch/genericintel/%.asm | $(X64_BUILD)
	nasm -f elf64 $< -o $@

.PHONY: x64.debug
x64.debug: $(X64_OBJS) $(X64_ASM_OBJS)
	clang -m64 -nostdlib -Wl,-T,arch/genericintel/linker.ld -o $(X64_BUILD)/ocmobile $^ 
	@echo "Built x64 debug binary: $(X64_BUILD)/ocmobile"

# ----- UEFI target -----
# Set UEFI_SDK to GNU-EFI or EDK2 include/lib root. Example:
#  make uefi UEFI_SDK=/usr/local/gnu-efi CROSS_PREFIX=aarch64-none-elf-
UEFI_SDK ?=

.PHONY: uefi
uefi:
	@if [ -z "$(UEFI_SDK)" ]; then \
		echo "UEFI_SDK not set; skipping uefi build (set UEFI_SDK to build)"; \
		exit 0; \
	fi; \
	mkdir -p $(BUILD)/uefi; \
	$(CC) -I$(UEFI_SDK)/include -I$(UEFI_SDK)/include/gnu-efi -D_FILE_OFFSET_BITS=64 -fshort-wchar -c UEFI/main.c -o $(BUILD)/uefi/main.o; \
	$(CC) -nostdlib -Wl,-Bsymbolic -Wl,--no-undefined -Wl,-shared -o $(BUILD)/uefi/OCMobile.efi $(BUILD)/uefi/main.o -L$(UEFI_SDK)/lib -lgnuefi -lefi || { echo "link failed; ensure UEFI_SDK points to GNU-EFI or EDK2 libs"; exit 1; }; \
	@echo "Built UEFI app: $(BUILD)/uefi/OCMobile.efi"

# ----- UEFI compatibility shim -----
UEFI_COMPAT_SRC := UEFICompat/compat.c
UEFI_COMPAT_BIN := $(BUILD)/uefi_compat/compat

$(BUILD)/uefi_compat:
	@mkdir -p $(BUILD)/uefi_compat

$(UEFI_COMPAT_BIN): $(UEFI_COMPAT_SRC) | $(BUILD)/uefi_compat
	$(CC) $(CFLAGS) -static -O2 $(UEFI_COMPAT_SRC) -o $@

.PHONY: uefi-compat install-uefi-ramdisk
uefi-compat: $(UEFI_COMPAT_BIN)
	@echo "Built uefi-compat: $(UEFI_COMPAT_BIN)"

install-uefi-ramdisk:
	@echo "Run: UEFICompat/install_to_ramdisk.sh <extracted-ramdisk-dir> [compat] [efi]"

