include $(dir $(lastword $(MAKEFILE_LIST)))/system.mk

# System bundle metadata
SYSTEM_NAME ?= BaseOS
SYSTEM_IDENTIFIER ?= org.pocketdarwin.$(shell echo "$(SYSTEM_NAME)" | tr A-Z a-z)
SYSTEM_VERSION ?= 0.1
SYSTEM_ARCH ?= arm64
SYSTEM_TYPE ?= Base
SYSTEM_FORMAT_VERSION ?= 1

# Output + layout
SYSTEM_BUNDLE_DIR ?= $(SYSTEM_NAME).System
SYSTEM_PLIST ?= $(SYSTEM_BUNDLE_DIR)/System.plist

# Optional source trees copied into the resulting bundle.
SYSTEM_CONTENTS_DIR ?=
DATA_CONTENTS_DIR ?=

DESTDIR ?=
SYSTEM_INSTALL_ROOT ?= /System/Bundles

_MK_SYS_REQUIRED_DIRS = \
	$(SYSTEM_BUNDLE_DIR)/System \
	$(SYSTEM_BUNDLE_DIR)/System/Applications \
	$(SYSTEM_BUNDLE_DIR)/System/Frameworks \
	$(SYSTEM_BUNDLE_DIR)/System/usr \
	$(SYSTEM_BUNDLE_DIR)/Data \
	$(SYSTEM_BUNDLE_DIR)/Data/user \
	$(SYSTEM_BUNDLE_DIR)/Data/system

.PHONY: all bundle plist systembundle-dirs install uninstall systembundle-clean

all: bundle

bundle: systembundle-dirs plist
ifneq ($(strip $(SYSTEM_CONTENTS_DIR)),)
	$(CP) -r "$(SYSTEM_CONTENTS_DIR)/." "$(SYSTEM_BUNDLE_DIR)/System/"
endif
ifneq ($(strip $(DATA_CONTENTS_DIR)),)
	$(CP) -r "$(DATA_CONTENTS_DIR)/." "$(SYSTEM_BUNDLE_DIR)/Data/"
endif

systembundle-dirs:
	@for d in $(_MK_SYS_REQUIRED_DIRS); do \
		$(MKDIR_P) "$$d"; \
	done

plist: | $(SYSTEM_BUNDLE_DIR)
	@printf '%s\n' \
	'<?xml version="1.0" encoding="UTF-8"?>' \
	'<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' \
	'<plist version="1.0">' \
	'<dict>' \
	'  <key>CFBundleName</key>' \
	'  <string>$(SYSTEM_NAME)</string>' \
	'  <key>CFBundleIdentifier</key>' \
	'  <string>$(SYSTEM_IDENTIFIER)</string>' \
	'  <key>CFBundleVersion</key>' \
	'  <string>$(SYSTEM_VERSION)</string>' \
	'  <key>PDSystemArchitecture</key>' \
	'  <string>$(SYSTEM_ARCH)</string>' \
	'  <key>PDSystemFormatVersion</key>' \
	'  <integer>$(SYSTEM_FORMAT_VERSION)</integer>' \
	'  <key>PDSystemType</key>' \
	'  <string>$(SYSTEM_TYPE)</string>' \
	'</dict>' \
	'</plist>' > "$(SYSTEM_PLIST)"

$(SYSTEM_BUNDLE_DIR):
	$(MKDIR_P) "$@"

install: bundle
	$(MKDIR_P) "$(DESTDIR)$(SYSTEM_INSTALL_ROOT)"
	$(CP) -r "$(SYSTEM_BUNDLE_DIR)" "$(DESTDIR)$(SYSTEM_INSTALL_ROOT)/"

uninstall:
	$(RM) -r "$(DESTDIR)$(SYSTEM_INSTALL_ROOT)/$(SYSTEM_BUNDLE_DIR)"

clean: systembundle-clean

systembundle-clean:
	$(RM) -r "$(SYSTEM_BUNDLE_DIR)"
