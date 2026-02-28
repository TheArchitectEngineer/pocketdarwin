/*
 * PSSettingsRootController.h
 * Settings.app - PocketDarwin
 *
 * The root table of Settings.app.
 * Lists all top-level preference panels (Airplane Mode, WiFi, Brightness, etc.)
 *
 * iPhoneOS 2.0 compatible.
 */

#import "PSViewController.h"

@interface PSSettingsRootController : PSViewController {
    BOOL _airplaneModeEnabled;
}

/* Airplane mode */
- (NSNumber *)getAirplaneMode;
- (void)setAirplaneMode:(NSNumber *)value;

@end
