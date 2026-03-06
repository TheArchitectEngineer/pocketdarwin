MAKEKIT_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
MAKEFILES_DIR := $(MAKEKIT_ROOT)/makefiles

.PHONY: help show env smoke

help:
	@echo "MakeKit"
	@echo
	@echo "Usage:"
	@echo "  include \$$\(SRCROOT\)/Frameworks/MakeKit/makefiles/iphoneApp.mk"
	@echo "  include \$$\(SRCROOT\)/Frameworks/MakeKit/makefiles/lib.mk"
	@echo "  include \$$\(SRCROOT\)/Frameworks/MakeKit/makefiles/tool.mk"
	@echo "  include \$$\(SRCROOT\)/Frameworks/MakeKit/makefiles/systemBundle.mk"
	@echo
	@echo "Common variables:"
	@echo "  SDK_SYSROOT=...      (optional -isysroot path)"
	@echo "  TARGET_TRIPLE=...    (optional clang -target)"
	@echo "  ARCH=...             (optional arch)"
	@echo "  IPHONEOS_MIN=...     (optional min iPhone OS version)"
	@echo "  MK_ENABLE_LINK=1     (disabled by default)"
	@echo "  SOURCES=...          (source list; defaults to Sources/*)"

show:
	@echo "MAKEKIT_ROOT=$(MAKEKIT_ROOT)"
	@echo "MAKEFILES_DIR=$(MAKEFILES_DIR)"

env:
	@echo "Source one of:"
	@echo "  $(MAKEKIT_ROOT)/CoreObjc.sh"
	@echo "  $(MAKEKIT_ROOT)/CoreObjc.csh"
	@echo "  $(MAKEKIT_ROOT)/CoreObjc.fish"

smoke:
	@$(MAKE) -f "$(MAKEFILES_DIR)/system.mk" show-config
