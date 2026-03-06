include $(dir $(lastword $(MAKEFILE_LIST)))/system.mk

APP_NAME ?= MyApp
BUNDLE_DIR ?= $(APP_NAME).app
INFOPLIST ?= Resources/Info.plist
OUTPUT_DIR ?= $(MK_BUILD_DIR)
EXECUTABLE ?= $(APP_NAME)
EXECUTABLE_PATH ?= $(OUTPUT_DIR)/$(EXECUTABLE)

FRAMEWORKS ?= -framework UIKit -framework Foundation
LIBS ?= -lobjc -lSystem

INSTALL_ROOT ?= /Applications
DESTDIR ?=

.PHONY: all bundle install

ifeq ($(MK_ENABLE_LINK),1)
all: $(EXECUTABLE_PATH) bundle

$(EXECUTABLE_PATH): objects
	@$(MKDIR_P) "$(dir $@)"
	$(CXX) $(OBJECTS) $(MK_LDFLAGS) $(LIBS) $(FRAMEWORKS) -o "$@"
else
all: objects link-not-enabled
endif

bundle: | $(BUNDLE_DIR)
ifeq ($(MK_ENABLE_LINK),1)
	$(INSTALL) -m 755 "$(EXECUTABLE_PATH)" "$(BUNDLE_DIR)/$(EXECUTABLE)"
endif
	@if [ -f "$(INFOPLIST)" ]; then \
		$(INSTALL) -m 644 "$(INFOPLIST)" "$(BUNDLE_DIR)/Info.plist"; \
	else \
		printf '%s\n' '<?xml version="1.0" encoding="UTF-8"?>' \
		'<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' \
		'<plist version="1.0"><dict></dict></plist>' > "$(BUNDLE_DIR)/Info.plist"; \
	fi

$(BUNDLE_DIR):
	$(MKDIR_P) "$@"

install: bundle
	$(MKDIR_P) "$(DESTDIR)$(INSTALL_ROOT)/$(BUNDLE_DIR)"
	$(CP) -r "$(BUNDLE_DIR)/." "$(DESTDIR)$(INSTALL_ROOT)/$(BUNDLE_DIR)/"
