include $(dir $(lastword $(MAKEFILE_LIST)))/system.mk

TARGET ?= tool
OUTPUT_DIR ?= $(MK_BUILD_DIR)
TARGET_FILE ?= $(OUTPUT_DIR)/$(TARGET)

LIBS ?=
FRAMEWORKS ?=

.PHONY: all link install

ifeq ($(MK_ENABLE_LINK),1)
all: $(TARGET_FILE)

$(TARGET_FILE): objects
	@$(MKDIR_P) "$(dir $@)"
	$(CXX) $(OBJECTS) $(MK_LDFLAGS) $(LIBS) $(FRAMEWORKS) -o "$@"
else
all: objects link-not-enabled
endif

install:
	@echo "MakeKit: tool install not configured for TARGET=$(TARGET)."
