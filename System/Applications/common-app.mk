SDK_SYSROOT ?= /Developer/Platforms/iPhoneOS.platform/iPhoneOS3.2.sdk
CRT1       ?= $(SDK_SYSROOT)/usr/lib/crt1.o

CC = clang
LD = /opt/darwin/bin/arm-apple-darwin9-ld

IPHONEOS_MIN ?= 2.0
TARGET_TRIPLE ?= armv6-apple-darwin9
ARCH ?= armv6

CFLAGS_COMMON = \
	-target $(TARGET_TRIPLE) \
	-isysroot $(SDK_SYSROOT) \
	-march=$(ARCH) \
	-mthumb \
	-fno-stack-protector \
	-miphoneos-version-min=$(IPHONEOS_MIN) \
	-IHeaders

LDFLAGS_COMMON = \
	-arch $(ARCH) \
	-syslibroot $(SDK_SYSROOT)

FRAMEWORKS ?= -framework UIKit -framework Foundation -framework CoreGraphics -framework QuartzCore
LIBS ?= -lSystem -lobjc -lgcc_s.1

INSTALL ?= install
MKDIR_P ?= mkdir -p
RM ?= rm -f
CP ?= cp -f

DESTDIR ?=
PREFIX ?= /Applications
DEVICE_IP ?= 192.168.1.100

OBJECTS = $(SOURCES:.m=.o)
INFOPLIST_SRC = $(wildcard $(INFOPLIST))

.PHONY: all bundle install uninstall clean deploy

all: $(EXECUTABLE) bundle

$(EXECUTABLE): $(OBJECTS)
	$(LD) $(LDFLAGS_COMMON) -o $@ $(CRT1) $(OBJECTS) $(LIBS) $(FRAMEWORKS)

Sources/%.o: Sources/%.m
	$(CC) $(CFLAGS_COMMON) -c $< -o $@

bundle: $(BUNDLE_DIR)/$(EXECUTABLE) | $(BUNDLE_DIR)
	@if [ -f "$(INFOPLIST)" ]; then \
		$(INSTALL) -m 644 "$(INFOPLIST)" "$(BUNDLE_DIR)/Info.plist"; \
	else \
		printf '%s\n' '<?xml version="1.0" encoding="UTF-8"?>' \
		'<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' \
		'<plist version="1.0"><dict></dict></plist>' > "$(BUNDLE_DIR)/Info.plist"; \
	fi

$(BUNDLE_DIR):
	$(MKDIR_P) "$@"

$(BUNDLE_DIR)/$(EXECUTABLE): $(EXECUTABLE) | $(BUNDLE_DIR)
	$(INSTALL) -m 755 "$(EXECUTABLE)" "$@"

install: all
	$(MKDIR_P) "$(DESTDIR)$(PREFIX)/$(BUNDLE_DIR)"
	$(INSTALL) -m 755 "$(BUNDLE_DIR)/$(EXECUTABLE)" \
		"$(DESTDIR)$(PREFIX)/$(BUNDLE_DIR)/$(EXECUTABLE)"
	$(INSTALL) -m 644 "$(BUNDLE_DIR)/Info.plist" \
		"$(DESTDIR)$(PREFIX)/$(BUNDLE_DIR)/Info.plist"

uninstall:
	rm -rf "$(DESTDIR)$(PREFIX)/$(BUNDLE_DIR)"

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
	rm -rf "$(BUNDLE_DIR)"

deploy: all
	$(if $(DEPLOY_PRE),$(DEPLOY_PRE),:)
	scp -r "$(BUNDLE_DIR)" root@$(DEVICE_IP):$(DEPLOY_ROOT)/
	$(if $(DEPLOY_POST),$(DEPLOY_POST),:)
