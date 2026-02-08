# ---- tools ----
GNUMAKE := gmake
JOBS    := $(shell sysctl -n hw.ncpu)

# ---- project layout ----
ROOT    := $(CURDIR)
XNU_SRC := $(ROOT)/xnu

BUILD   := $(ROOT)/build
OBJROOT := $(BUILD)/obj
SYMROOT := $(BUILD)/sym
DSTROOT := $(BUILD)/dst

BUILDTYPE      ?= DEVELOPMENT
ARCH           ?= ARM64
SDKROOT        ?= $(shell xcrun --show-sdk-path)
KERNELVERSION  ?= 0.0.0

ARM_CPU	       := generic
PLATFORM       := ARM64
TARGET_CONFIGS := ARM64
CURRENT_ARCH_CONFIG := ARM64
ARCH_CONFIGS := ARM64
ARCH := ARM64

export SRCROOT := $(XNU_SRC)
export OBJROOT
export SYMROOT
export DSTROOT
export SDKROOT
export ARM_CPU
export PLATFORM
export TARGET_CONFIGS
export CURRENT_ARCH_CONFIGS
export ARCH_CONFIGS
export ARCH

.PHONY: all xnu dirs clean

all: dirs xnu

dirs:
	mkdir -p $(OBJROOT) $(SYMROOT) $(DSTROOT)

xnu:
	cd $(XNU_SRC) && \
	$(GNUMAKE) -j$(JOBS) \
	  LOGCOLORS=y \
	  PLATFORM=MacOSX \
	  BUILD_LTO=0 \
	  BUILD_WERROR=0 \
	  DO_CTFMERGE=0 \
	  BUILD_STATIC_LINK=1 \
	  KERNEL_CONFIGS=$(BUILDTYPE) \
	  ARCH_CONFIGS=$(ARCH) \
	  VERBOSE=YES \
	  RC_DARWIN_KERNEL_VERSION=$(KERNELVERSION)

clean:
	rm -rf $(BUILD)

