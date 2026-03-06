include $(dir $(lastword $(MAKEFILE_LIST)))/system.mk

LIBRARY_NAME ?= MyLib
LIBRARY_KIND ?= static
OUTPUT_DIR ?= $(MK_BUILD_DIR)

STATIC_LIB := $(OUTPUT_DIR)/lib$(LIBRARY_NAME).a
DYLIB := $(OUTPUT_DIR)/lib$(LIBRARY_NAME).dylib

LIBS ?=
FRAMEWORKS ?=

.PHONY: all

ifeq ($(LIBRARY_KIND),static)
all: $(STATIC_LIB)

$(STATIC_LIB): objects
	@$(MKDIR_P) "$(dir $@)"
	$(AR) rcs "$@" $(OBJECTS)
	$(RANLIB) "$@"
else
ifeq ($(MK_ENABLE_LINK),1)
all: $(DYLIB)

$(DYLIB): objects
	@$(MKDIR_P) "$(dir $@)"
	$(CXX) -dynamiclib $(OBJECTS) $(MK_LDFLAGS) $(LIBS) $(FRAMEWORKS) -o "$@"
else
all: objects link-not-enabled
endif
endif
