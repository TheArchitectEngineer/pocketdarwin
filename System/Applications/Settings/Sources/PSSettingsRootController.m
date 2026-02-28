/*
 * PSSettingsRootController.m
 * Settings.app - PocketDarwin
 *
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import <UIKit/UIKit.h>
#import "PSSettingsRootController.h"
#import "PSSpecifier.h"

@implementation PSSettingsRootController

- (id)init
{
    if ((self = [super init])) {
        self.title = @"Settings";
        _airplaneModeEnabled = NO;
    }
    return self;
}

- (NSNumber *)getAirplaneMode
{
    return [self readPreferenceValue:[self specifierForKey:@"airplaneMode"]];
}

- (void)setAirplaneMode:(NSNumber *)value
{
    _airplaneModeEnabled = [value boolValue];
    [self setPreferenceValue:value
                   specifier:[self specifierForKey:@"airplaneMode"]];

    /* In production: toggle the radio stack via CommCenter IPC */
    NSLog(@"Settings: Airplane mode %@", _airplaneModeEnabled ? @"ON" : @"OFF");
}

- (NSArray *)specifiers
{
    NSMutableArray *specs = [NSMutableArray array];

    /* ── Section: Connectivity ────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];

    /* Airplane Mode */
    PSSpecifier *airplane = [PSSpecifier switchSpecifierNamed:@"Airplane Mode"
                                                          key:@"airplaneMode"
                                                       target:self
                                                          set:@selector(setAirplaneMode:)
                                                          get:@selector(getAirplaneMode)];
    airplane.defaultValue = [NSNumber numberWithBool:NO];
    [specs addObject:airplane];

    /* Wi-Fi → detail */
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Wi-Fi"
                                    controllerClass:@"PSWiFiController"]];

    /* Carrier (PocketDarwin: no cellular, hide or show as info) */
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Carrier"
                                    controllerClass:@"PSNetworkController"]];

    /* ── Section: Device ──────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];

    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Sounds"
                                    controllerClass:@"PSSoundController"]];

    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Brightness"
                                    controllerClass:@"PSDisplayController"]];

    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Wallpaper"
                                    controllerClass:@"PSWallpaperController"]];

    /* ── Section: General ─────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];

    [specs addObject:[PSSpecifier linkSpecifierNamed:@"General"
                                    controllerClass:@"PSGeneralController"]];

    /* ── Section: Apps ────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];

    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Mail"
                                    controllerClass:@"PSMailController"]];

    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Safari"
                                    controllerClass:@"PSSafariController"]];

    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Photos"
                                    controllerClass:@"PSPhotosController"]];

    return specs;
}

@end
