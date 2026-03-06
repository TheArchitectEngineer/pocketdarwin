# Makefile.am
# SpringBoard — PocketDarwin
#
# Automake input file.  Processed by 'automake' to produce Makefile.in,
# which configure then instantiates into the final Makefile.
#
# Build model:
#   - All .m (Objective-C) sources are compiled into a single executable
#     named "SpringBoard" using OBJCLINK (the ObjC linker).
#   - The executable plus Info.plist (and any resources) are assembled
#     into SpringBoard.app/ by install-exec-local / dist-hook rules.
#
# Automake does not natively understand .app bundles or Objective-C
# link steps, so we use a noinst_PROGRAMS primary and a custom
# install-exec-local hook to place the bundle correctly.

# ── Automake options ────────────────────────────────────────────────────────

AUTOMAKE_OPTIONS = foreign subdir-objects

# ── Compiler / linker flag variables ────────────────────────────────────────
# These are set by configure and exported into Makefile via AC_SUBST.
# We re-list them here so Automake passes them to the right tools.

IPHONEOS_SYSROOT = /Developer/Platforms/iPhoneOS.platform/iPhoneOS3.2.sdk
IPHONEOS_CRT1    = $(IPHONEOS_SYSROOT)/usr/lib/crt1.o

OBJC = clang

AM_OBJCFLAGS = \
    $(OBJCFLAGS) \
    -target armv6-apple-darwin9 \
    -isysroot $(IPHONEOS_SYSROOT) \
    -march=armv6 \
    -mthumb \
    -miphoneos-version-min=2.0 \
    -IHeaders

AM_CFLAGS = \
    $(CFLAGS) \
    -IHeaders

AM_LDFLAGS = \
    $(LDFLAGS) \
    $(FRAMEWORK_LIBS)

# ── Sources ──────────────────────────────────────────────────────────────────
#
# Automake determines which compiler to invoke per source extension:
#   .m  → OBJC (arm-apple-darwin9-gcc)
#   .c  → CC   (same cross-gcc, used only for C helper files if any)
#
# The final link step uses OBJCLINK because the program contains ObjC sources.

noinst_PROGRAMS = SpringBoard

SpringBoard_SOURCES = \
    Sources/main.m                      \
    Sources/SpringBoardAppDelegate.m    \
    Sources/SBApplication.m             \
    Sources/SBApplicationIcon.m         \
    Sources/SBIconList.m                \
    Sources/SBDock.m                    \
    Sources/SBStatusBar.m               \
    Sources/SBSpringBoardView.m

# Extra header files — listed so 'make dist' includes them and so that
# editors and IDEs that parse Makefile.am can discover all source files.
# Automake ignores them for compilation purposes (they are #imported).
EXTRA_DIST = \
    Headers/SBApplication.h             \
    Headers/SBApplicationIcon.h         \
    Headers/SBIconList.h                \
    Headers/SBDock.h                    \
    Headers/SBStatusBar.h               \
    Headers/SBSpringBoardView.h         \
    Headers/SpringBoardAppDelegate.h    \
    Resources/Info.plist

# Per-target LDADD: the framework flags resolved by configure
SpringBoard_LDADD = \
    -lSystem

SpringBoard_LINK = \
    /opt/darwin/bin/arm-apple-darwin9-ld \
    -arch armv6 \
    -syslibroot $(IPHONEOS_SYSROOT) \
    -o $@ \
    $(IPHONEOS_CRT1)

SpringBoard_LDFLAGS = \
    -framework UIKit

# ── .app bundle assembly ─────────────────────────────────────────────────────
#
# Automake installs noinst_PROGRAMS nowhere by default, so we do it manually
# in install-exec-local.  This keeps the logic visible and self-contained.

BUNDLE_DIR = SpringBoard.app

# The Info.plist lives in Resources/ inside the source tree.
INFOPLIST  = $(srcdir)/Resources/Info.plist

# Bundle contents we must create at install time
BUNDLE_CONTENTS = \
    $(BUNDLE_DIR)/SpringBoard \
    $(BUNDLE_DIR)/Info.plist

# Build the in-tree bundle after the executable is linked
all-local: $(BUNDLE_DIR)/Info.plist

$(BUNDLE_DIR)/Info.plist: $(INFOPLIST) | $(BUNDLE_DIR)
	$(INSTALL_DATA) "$(INFOPLIST)" "$@"

$(BUNDLE_DIR):
	$(MKDIR_P) "$@"

# Install the whole bundle under $(BUNDLE_INSTALL) (set by configure)
install-exec-local: SpringBoard $(BUNDLE_DIR)/Info.plist
	$(MKDIR_P) "$(DESTDIR)$(prefix)/$(BUNDLE_DIR)"
	$(INSTALL_PROGRAM) SpringBoard \
	    "$(DESTDIR)$(prefix)/$(BUNDLE_DIR)/SpringBoard"
	$(INSTALL_DATA) "$(BUNDLE_DIR)/Info.plist" \
	    "$(DESTDIR)$(prefix)/$(BUNDLE_DIR)/Info.plist"
	@echo "SpringBoard.app installed to $(DESTDIR)$(prefix)/$(BUNDLE_DIR)"

uninstall-local:
	rm -rf "$(DESTDIR)$(prefix)/$(BUNDLE_DIR)"

# ── Distribution ─────────────────────────────────────────────────────────────
#
# 'make dist' builds a .tar.gz that includes everything needed to
# reconfigure and rebuild from scratch on another machine.

dist-hook:
	$(MKDIR_P) "$(distdir)/Sources"
	$(MKDIR_P) "$(distdir)/Headers"
	$(MKDIR_P) "$(distdir)/Resources"
	cp -p $(srcdir)/Sources/*.m    "$(distdir)/Sources/"
	cp -p $(srcdir)/Headers/*.h    "$(distdir)/Headers/"
	cp -p $(srcdir)/Resources/*.plist "$(distdir)/Resources/" 2>/dev/null || true
	cp -p $(srcdir)/configure.ac   "$(distdir)/"
	cp -p $(srcdir)/Makefile.am    "$(distdir)/"

# ── Cleaning ─────────────────────────────────────────────────────────────────

clean-local:
	rm -rf $(BUNDLE_DIR)

# ── Convenience targets ───────────────────────────────────────────────────────

# Deploy to a live PocketDarwin device over SSH.
# Usage: make deploy DEVICE_IP=192.168.1.100
DEVICE_IP ?= 192.168.1.100

.PHONY: deploy

deploy: all
	ssh root@$(DEVICE_IP) 'killall SpringBoard 2>/dev/null; sleep 1; true'
	scp -r $(BUNDLE_DIR) \
	    root@$(DEVICE_IP):/System/Library/CoreServices/
	ssh root@$(DEVICE_IP) \
	    'chmod +x /System/Library/CoreServices/SpringBoard.app/SpringBoard'
	ssh root@$(DEVICE_IP) \
	    'launchctl start com.pocketdarwin.SpringBoard'
	@echo "Deployed to $(DEVICE_IP)"
