/*
 * PSGeneralController.h
 * Settings.app - PocketDarwin
 *
 * "General" settings panel (mirrors iPhoneOS 2.0 General settings).
 * Covers: About, Auto-Lock, Passcode Lock, Network, Bluetooth,
 *         Location Services, Keyboard, Accessibility, Reset.
 *
 * iPhoneOS 2.0 compatible.
 */

#import "PSViewController.h"

@interface PSGeneralController : PSViewController

/* About sub-panel values */
- (NSString *)_deviceName;
- (NSString *)_softwareVersion;
- (NSString *)_modelIdentifier;
- (NSString *)_serialNumber;

/* Reset all settings */
- (void)_resetAllSettings:(PSSpecifier *)specifier;
- (void)_eraseAllContent:(PSSpecifier *)specifier;

@end
