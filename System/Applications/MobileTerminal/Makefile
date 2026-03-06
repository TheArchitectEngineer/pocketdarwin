APP_NAME = MobileTerminal
EXECUTABLE = MobileTerminal
BUNDLE_DIR = MobileTerminal.app
INFOPLIST = Resources/Info.plist
PREFIX = /Applications
DEPLOY_ROOT = /Applications

DEPLOY_PRE = ssh root@$(DEVICE_IP) 'killall MobileTerminal 2>/dev/null; true'
DEPLOY_POST = ssh root@$(DEVICE_IP) 'chmod +x /Applications/MobileTerminal.app/MobileTerminal'

SOURCES = \
	Sources/main.m \
	Sources/MobileTerminalAppDelegate.m \
	Sources/MTTerminalSession.m \
	Sources/MTTerminalViewController.m

LIBS = -lSystem -lobjc -lgcc_s.1 -lutil

include ../common-app.mk
