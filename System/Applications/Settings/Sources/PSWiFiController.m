/*
 * PSWiFiController.m
 * Settings.app - PocketDarwin
 *
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import <UIKit/UIKit.h>
#import "PSWiFiController.h"
#import "PSSpecifier.h"

@implementation PSWiFiController

- (id)init
{
    if ((self = [super init])) {
        self.title       = @"Wi-Fi";
        _wifiEnabled     = YES;
        _connectedSSID   = [@"PocketDarwin-Net" retain];
        _scannedNetworks = [[NSMutableArray alloc] init];

        /* Seed some example networks */
        [_scannedNetworks addObject:@"PocketDarwin-Net"];
        [_scannedNetworks addObject:@"iPhone Network"];
        [_scannedNetworks addObject:@"linksys"];
        [_scannedNetworks addObject:@"NETGEAR"];
    }
    return self;
}

- (NSNumber *)getWiFiEnabled
{
    id stored = [self readPreferenceValue:[self specifierForKey:@"wifiEnabled"]];
    if (stored) return stored;
    return [NSNumber numberWithBool:_wifiEnabled];
}

- (void)setWiFiEnabled:(NSNumber *)value
{
    _wifiEnabled = [value boolValue];
    [self setPreferenceValue:value specifier:[self specifierForKey:@"wifiEnabled"]];

    /* In production: toggle via Apple80211 framework or ioctl */
    NSLog(@"Settings/WiFi: Wi-Fi %@", _wifiEnabled ? @"enabled" : @"disabled");
    [self reloadSpecifiers];
}

- (void)_scanNetworks
{
    /* In production: invoke Apple80211Scan() */
    NSLog(@"Settings/WiFi: Scanning for networks...");
}

- (NSArray *)specifiers
{
    NSMutableArray *specs = [NSMutableArray array];

    /* ── Section: Wi-Fi toggle ── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];

    PSSpecifier *wifiSwitch = [PSSpecifier switchSpecifierNamed:@"Wi-Fi"
                                                            key:@"wifiEnabled"
                                                         target:self
                                                            set:@selector(setWiFiEnabled:)
                                                            get:@selector(getWiFiEnabled)];
    wifiSwitch.defaultValue = [NSNumber numberWithBool:YES];
    [specs addObject:wifiSwitch];

    if (_wifiEnabled) {
        /* ── Section: Current network ── */
        [specs addObject:[PSSpecifier groupSpecifierWithName:@"Choose a Network..."]];

        /* Connected network with checkmark */
        PSSpecifier *connected = [PSSpecifier titleValueSpecifierNamed:_connectedSSID
                                                                 value:@"✓"];
        [specs addObject:connected];

        /* Other scanned networks */
        for (NSString *ssid in _scannedNetworks) {
            if ([ssid isEqualToString:_connectedSSID]) continue;
            PSSpecifier *net = [PSSpecifier linkSpecifierNamed:ssid controllerClass:nil];
            [specs addObject:net];
        }

        /* Other Network... */
        PSSpecifier *other = [PSSpecifier linkSpecifierNamed:@"Other..." controllerClass:nil];
        [specs addObject:other];

        /* ── Section: Options ── */
        [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];

        PSSpecifier *askJoin = [PSSpecifier switchSpecifierNamed:@"Ask to Join Networks"
                                                             key:@"wifiAskJoin"
                                                          target:self
                                                             set:@selector(setBoolForSpecifier:value:)
                                                             get:@selector(getBoolForSpecifier:)];
        askJoin.defaultValue = [NSNumber numberWithBool:YES];
        [specs addObject:askJoin];
    }

    return specs;
}

- (void)dealloc
{
    [_connectedSSID   release];
    [_scannedNetworks release];
    [super dealloc];
}

@end
