CROSS_PREFIX ?= aarch64-none-elf-

# detect cross toolchain: prefer aarch64-none-elf-clang, then gcc, otherwise use local clang with -target
ifneq ($(shell command -v $(CROSS_PREFIX)clang 2>/dev/null),)
CC := $(CROSS_PREFIX)clang
CXX := $(CROSS_PREFIX)clang++
CROSS_USED := 1
else ifneq ($(shell command -v $(CROSS_PREFIX)gcc 2>/dev/null),)
CC := $(CROSS_PREFIX)gcc
CXX := $(CROSS_PREFIX)g++
CROSS_USED := 1
else
CC := clang
CXX := clang++
CFLAGS += -target aarch64-none-elf
endif

CFLAGS ?= -ffreestanding -nostdlib -O2 -Wall -Wextra -Wimplicit-function-declaration -I.
AS := nasm
ASFLAGS := # later
DTC := dtc

sources := loader.c \
BootMenu.c \
Framebuffer.c \
ACPIParser.c \
platform.c \
Platform/plist/plist.c \
Platform/OpenPartitionDxe/Gpt.c \
Platform/OpenPartitionDxe/Mbr.c \
Platform/Kextld.c \
bootlib.c \
kernel_patcher.c

config.dtb: config.dts
	-$(DTC) -O dtb -o config.dtb config.dts

# build targets
TARGET := ocmboot
OBJ := $(patsubst %.c,%.o,$(sources))

# Optional variables for boot image creation
# Usage: make bootimg DEVICE_DT_PATH=/path/to/dtb [RAMDISK=ramdisk.img] [CMDLINE="console=tty0"]
MKBOOTIMG := $(shell command -v mkbootimg 2>/dev/null || true)


patch-kernel:
	./Tools/patch_kernel.sh

.PHONY: patch-kernel

all: config.dtb $(TARGET)

# compile objects
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET) config.dtb boot.img

.PHONY: clean bootimg help patch-kernel

help:
	@echo "Makefile targets:"
	@echo "  make            - build $(TARGET) (also builds config.dtb)"
	@echo "  make bootimg DEVICE_DT_PATH=path/to/dtb [RAMDISK=ramdisk.img] [CMDLINE='...'] - build Android boot.img using mkbootimg"
	@echo "  make clean      - remove build artifacts"

bootimg: all
	@echo "Building boot.img..."
	@if [ -z "$(DEVICE_DT_PATH)" ]; then echo "ERROR: DEVICE_DT_PATH is not set. Usage: make bootimg DEVICE_DT_PATH=path/to/dtb"; exit 1; fi
	@command -v mkbootimg >/dev/null 2>&1 || { echo "ERROR: mkbootimg not found. Install Android mkbootimg tool."; exit 1; }
	@echo "Using DEVICE_DT_PATH=$(DEVICE_DT_PATH)"
	@$(MKBOOTIMG) --kernel $(TARGET) $(if $(RAMDISK),--ramdisk $(RAMDISK)) $(if $(CMDLINE),--cmdline '$(CMDLINE)') --dt $(DEVICE_DT_PATH) -o boot.img
