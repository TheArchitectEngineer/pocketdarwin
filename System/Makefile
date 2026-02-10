CC ?= clang
AS ?= as

SDKROOT := $(shell xcrun --show-sdk-path)
TARGETMK := scripts/mktarget
XNUKERNELSRC := $(PWD)/darwinKernel/xnu-arm
LINUXKERNELSRC ?= Boot/linuxTemp/android_kernel_$(MANUFACTURER)_$(DEVICE)

MANUFACTURER ?= none
DEVICE ?= ibroke

RELEASETYPE ?= DEBUG

$(PATCHESPATH) := $(PWD)/darwinKernel/patches

export SDKROOT

harpia:
	$(MAKE) -C $(XNUKERNELSRC) TARGET_CONFIGS="$(RELEASETYPE) arm ARMPBA8"
	$(MAKE) -C $(LINUXKERNELSRC) ARCH="arm"
	@echo "darwinkernelbin='$XNUSource/build/arm/kernel'; linuxkernelbin=none" > kernelconfig

