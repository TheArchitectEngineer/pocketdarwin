# Core MakeKit settings and compile rules.
#
# Include this from target-specific makefiles (`tool.mk`, `lib.mk`, etc).

ifndef MAKEKIT_SYSTEM_MK
MAKEKIT_SYSTEM_MK := 1

MK_THIS_FILE := $(lastword $(MAKEFILE_LIST))
MK_MAKEFILES_DIR := $(abspath $(dir $(MK_THIS_FILE)))
MK_ROOT ?= $(abspath $(MK_MAKEFILES_DIR)/..)

ifeq ($(origin CC), default)
CC := clang
endif
ifeq ($(origin CXX), default)
CXX := clang++
endif
ifeq ($(origin AR), default)
AR := llvm-ar
endif
ifeq ($(origin RANLIB), default)
RANLIB := llvm-ranlib
endif
INSTALL ?= install
MKDIR_P ?= mkdir -p
RM ?= rm -f
CP ?= cp -f

SDK_SYSROOT ?=
TARGET_TRIPLE ?=
ARCH ?=
IPHONEOS_MIN ?=
MK_ENABLE_LINK ?= 0
MK_BUILD_DIR ?= $(CURDIR)/build
MK_OBJ_DIR ?= $(MK_BUILD_DIR)/obj

MK_WARN_FLAGS ?= -Wall -Wextra
MK_OPT_FLAGS ?= -O2 -g
MK_CPPFLAGS ?=

MK_ISYSROOT_FLAG := $(if $(strip $(SDK_SYSROOT)),-isysroot $(SDK_SYSROOT),)
MK_TARGET_FLAG := $(if $(strip $(TARGET_TRIPLE)),-target $(TARGET_TRIPLE),)
MK_ARCH_FLAG := $(if $(strip $(ARCH)),-arch $(ARCH),)
MK_MINVER_FLAG := $(if $(strip $(IPHONEOS_MIN)),-miphoneos-version-min=$(IPHONEOS_MIN),)

MK_CFLAGS ?= $(MK_WARN_FLAGS) $(MK_OPT_FLAGS) $(MK_TARGET_FLAG) $(MK_ISYSROOT_FLAG) $(MK_ARCH_FLAG) $(MK_MINVER_FLAG)
MK_OBJCFLAGS ?= $(MK_CFLAGS)
MK_CXXFLAGS ?= $(MK_CFLAGS)
MK_LDFLAGS ?= $(MK_TARGET_FLAG) $(if $(strip $(SDK_SYSROOT)),-syslibroot $(SDK_SYSROOT),) $(MK_ARCH_FLAG)

SOURCES ?= $(wildcard Sources/*.m Sources/*.mm Sources/*.c Sources/*.cc Sources/*.cpp)

MK_OBJS_M := $(patsubst %.m,$(MK_OBJ_DIR)/%.o,$(filter %.m,$(SOURCES)))
MK_OBJS_MM := $(patsubst %.mm,$(MK_OBJ_DIR)/%.o,$(filter %.mm,$(SOURCES)))
MK_OBJS_C := $(patsubst %.c,$(MK_OBJ_DIR)/%.o,$(filter %.c,$(SOURCES)))
MK_OBJS_CC := $(patsubst %.cc,$(MK_OBJ_DIR)/%.o,$(filter %.cc,$(SOURCES)))
MK_OBJS_CPP := $(patsubst %.cpp,$(MK_OBJ_DIR)/%.o,$(filter %.cpp,$(SOURCES)))
OBJECTS := $(MK_OBJS_M) $(MK_OBJS_MM) $(MK_OBJS_C) $(MK_OBJS_CC) $(MK_OBJS_CPP)

DEPFILES := $(OBJECTS:.o=.d)

.PHONY: dirs objects clean show-config

dirs:
	@$(MKDIR_P) "$(MK_OBJ_DIR)"

objects: $(OBJECTS)

$(MK_OBJ_DIR)/%.o: %.m
	@$(MKDIR_P) "$(dir $@)"
	$(CC) $(MK_CPPFLAGS) $(MK_OBJCFLAGS) -MMD -MP -c "$<" -o "$@"

$(MK_OBJ_DIR)/%.o: %.mm
	@$(MKDIR_P) "$(dir $@)"
	$(CXX) $(MK_CPPFLAGS) $(MK_CXXFLAGS) -MMD -MP -c "$<" -o "$@"

$(MK_OBJ_DIR)/%.o: %.c
	@$(MKDIR_P) "$(dir $@)"
	$(CC) $(MK_CPPFLAGS) $(MK_CFLAGS) -MMD -MP -c "$<" -o "$@"

$(MK_OBJ_DIR)/%.o: %.cc
	@$(MKDIR_P) "$(dir $@)"
	$(CXX) $(MK_CPPFLAGS) $(MK_CXXFLAGS) -MMD -MP -c "$<" -o "$@"

$(MK_OBJ_DIR)/%.o: %.cpp
	@$(MKDIR_P) "$(dir $@)"
	$(CXX) $(MK_CPPFLAGS) $(MK_CXXFLAGS) -MMD -MP -c "$<" -o "$@"

ifeq ($(MK_ENABLE_LINK),0)
.PHONY: link-not-enabled
link-not-enabled:
	@echo "MakeKit: linking disabled (set MK_ENABLE_LINK=1 and provide a linker toolchain)."
endif

show-config:
	@echo "MK_ROOT=$(MK_ROOT)"
	@echo "SDK_SYSROOT=$(SDK_SYSROOT)"
	@echo "TARGET_TRIPLE=$(TARGET_TRIPLE)"
	@echo "ARCH=$(ARCH)"
	@echo "IPHONEOS_MIN=$(IPHONEOS_MIN)"
	@echo "MK_ENABLE_LINK=$(MK_ENABLE_LINK)"
	@echo "CC=$(CC)"
	@echo "CXX=$(CXX)"
	@echo "AR=$(AR)"
	@echo "RANLIB=$(RANLIB)"
	@echo "MK_BUILD_DIR=$(MK_BUILD_DIR)"
	@echo "MK_OBJ_DIR=$(MK_OBJ_DIR)"
	@echo "SOURCES=$(SOURCES)"

clean:
	$(RM) -r "$(MK_BUILD_DIR)"

-include $(DEPFILES)

endif
