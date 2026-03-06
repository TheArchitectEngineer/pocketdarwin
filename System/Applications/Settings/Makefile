APP_NAME = Settings
EXECUTABLE = Settings
BUNDLE_DIR = Settings.app
INFOPLIST = Resources/Info.plist
PREFIX = /Applications
DEPLOY_ROOT = /Applications

DEPLOY_POST = ssh root@$(DEVICE_IP) 'chmod +x /Applications/Settings.app/Settings'

SOURCES = \
	Sources/main.m \
	Sources/SettingsAppDelegate.m \
	Sources/PSSpecifier.m \
	Sources/PSTableCell.m \
	Sources/PSViewController.m \
	Sources/PSSettingsRootController.m \
	Sources/PSWiFiController.m \
	Sources/PSNetworkController.m \
	Sources/PSGeneralController.m \
	Sources/PSSoundController.m \
	Sources/PSDisplayController.m \
	Sources/PSAboutController.m

include ../common-app.mk
