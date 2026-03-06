/*
 * PSWiFiController.h
 * Settings.app - PocketDarwin
 *
 * Wi-Fi settings panel.
 * iPhoneOS 2.0 compatible.
 */

#import "PSViewController.h"

@interface PSWiFiController : PSViewController {
    BOOL     _wifiEnabled;
    NSString *_connectedSSID;
    NSMutableArray *_scannedNetworks;
}

- (NSNumber *)getWiFiEnabled;
- (void)setWiFiEnabled:(NSNumber *)value;

/* Trigger a network scan */
- (void)_scanNetworks;

@end
