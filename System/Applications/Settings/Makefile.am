# Makefile.am
# Settings.app — PocketDarwin
#
# Automake input file. Processed by 'automake' into Makefile.in,
# then instantiated by 'configure' into the final Makefile.
#
# Build model:
#   All Objective-C source files (.m) are compiled and linked into
#   a single "Settings" executable. A Settings.app bundle is assembled
#   from the executable plus Info.plist via install-exec-local.
#
# The Preference-pane architecture (PSViewController / PSSpecifier) is
# compiled directly into the main binary — there is no separate
#  Preferences.framework shared library here (unlike the real iPhoneOS).

# ── Automake options ─────────────────────────────────────────────────────────

AUTOMAKE_OPTIONS = foreign subdir-objects

# ── Per-target compile flags ─────────────────────────────────────────────────

AM_OBJCFLAGS = \
    $(OBJCFLAGS) \
    -IHeaders

AM_CFLAGS = \
    $(CFLAGS) \
    -IHeaders

AM_LDFLAGS = \
    $(LDFLAGS) \
    $(FRAMEWORK_LIBS)

# ── Sources ───────────────────────────────────────────────────────────────────
#
# Source file grouping follows the internal architecture:
#
#   Infrastructure layer
#       PSSpecifier        — row descriptor (cell type, key, getter/setter)
#       PSTableCell        — UITableViewCell renderer for each PSSpecifier type
#       PSViewController   — base table controller (manages sections, prefs I/O)
#
#   Root controller
#       PSSettingsRootController — top-level settings list
#
#   Panel controllers (push onto UINavigationController stack)
#       PSWiFiController
#       PSNetworkController
#       PSGeneralController
#       PSSoundController
#       PSDisplayController
#       PSAboutController
#
#   Application layer
#       SettingsAppDelegate
#       main

noinst_PROGRAMS = Settings

Settings_SOURCES = \
    Sources/main.m                      \
    Sources/SettingsAppDelegate.m       \
    \
    Sources/PSSpecifier.m               \
    Sources/PSTableCell.m               \
    Sources/PSViewController.m          \
    \
    Sources/PSSettingsRootController.m  \
    \
    Sources/PSWiFiController.m          \
    Sources/PSNetworkController.m       \
    Sources/PSGeneralController.m       \
    Sources/PSSoundController.m         \
    Sources/PSDisplayController.m       \
    Sources/PSAboutController.m

# Headers — listed so 'make dist' includes them.
# Automake does not compile these; they are #imported by the .m files.
EXTRA_DIST = \
    Headers/SettingsAppDelegate.h       \
    Headers/PSSpecifier.h               \
    Headers/PSTableCell.h               \
    Headers/PSViewController.h          \
    Headers/PSSettingsRootController.h  \
    Headers/PSWiFiController.h          \
    Headers/PSNetworkController.h       \
    Headers/PSGeneralController.h       \
    Headers/PSSoundController.h         \
    Headers/PSDisplayController.h       \
    Headers/PSAboutController.h         \
    Resources/Info.plist

Settings_LDADD   = $(FRAMEWORK_LIBS)
Settings_LDFLAGS = $(AM_LDFLAGS)

# ── .app bundle assembly ──────────────────────────────────────────────────────

BUNDLE_DIR = Settings.app
INFOPLIST  = $(srcdir)/Resources/Info.plist

all-local: $(BUNDLE_DIR)/Info.plist

$(BUNDLE_DIR)/Info.plist: $(INFOPLIST) | $(BUNDLE_DIR)
	$(INSTALL_DATA) "$(INFOPLIST)" "$@"

$(BUNDLE_DIR):
	$(MKDIR_P) "$@"

# ── Install ───────────────────────────────────────────────────────────────────
#
# Settings.app is installed under /Applications on PocketDarwin.
# configure sets prefix=/Applications by default.

install-exec-local: Settings $(BUNDLE_DIR)/Info.plist
	$(MKDIR_P) "$(DESTDIR)$(prefix)/$(BUNDLE_DIR)"
	$(INSTALL_PROGRAM) Settings \
	    "$(DESTDIR)$(prefix)/$(BUNDLE_DIR)/Settings"
	$(INSTALL_DATA) "$(BUNDLE_DIR)/Info.plist" \
	    "$(DESTDIR)$(prefix)/$(BUNDLE_DIR)/Info.plist"
	@echo "Settings.app installed to $(DESTDIR)$(prefix)/$(BUNDLE_DIR)"

uninstall-local:
	rm -rf "$(DESTDIR)$(prefix)/$(BUNDLE_DIR)"

# ── Distribution ──────────────────────────────────────────────────────────────

dist-hook:
	$(MKDIR_P) "$(distdir)/Sources"
	$(MKDIR_P) "$(distdir)/Headers"
	$(MKDIR_P) "$(distdir)/Resources"
	cp -p $(srcdir)/Sources/*.m          "$(distdir)/Sources/"
	cp -p $(srcdir)/Headers/*.h          "$(distdir)/Headers/"
	cp -p $(srcdir)/Resources/*.plist    "$(distdir)/Resources/" 2>/dev/null || true
	cp -p $(srcdir)/configure.ac         "$(distdir)/"
	cp -p $(srcdir)/Makefile.am          "$(distdir)/"

# ── Cleaning ──────────────────────────────────────────────────────────────────

clean-local:
	rm -rf $(BUNDLE_DIR)

# ── Convenience: deploy over SSH ─────────────────────────────────────────────
#
# Usage: make deploy DEVICE_IP=192.168.1.100

DEVICE_IP ?= 192.168.1.100

.PHONY: deploy

deploy: all
	scp -r $(BUNDLE_DIR) \
	    root@$(DEVICE_IP):/Applications/
	ssh root@$(DEVICE_IP) \
	    'chmod +x /Applications/Settings.app/Settings'
	@echo "Settings.app deployed to $(DEVICE_IP)"
