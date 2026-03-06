/*
 * PSGeneralController.m
 * Settings.app - PocketDarwin
 *
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import <UIKit/UIKit.h>
#import "PSGeneralController.h"
#import "PSAboutController.h"
#import "PSSpecifier.h"

@implementation PSGeneralController

- (id)init
{
    if ((self = [super init])) {
        self.title = @"General";
    }
    return self;
}

- (NSString *)_deviceName
{
    /* In production: read from SCDynamicStoreCopyComputerName() */
    return @"PocketDarwin";
}

- (NSString *)_softwareVersion
{
    /* PocketDarwin version string */
    NSDictionary *info = [[NSBundle mainBundle] infoDictionary];
    NSString *v = [info objectForKey:@"CFBundleVersion"];
    return v ? v : @"2.0 (PocketDarwin)";
}

- (NSString *)_modelIdentifier
{
    /* In production: read via sysctl hw.machine */
    return @"PocketDarwin/armv6";
}

- (NSString *)_serialNumber
{
    return @"PD0000000000";
}

- (void)_resetAllSettings:(PSSpecifier *)specifier
{
    UIActionSheet *sheet = [[UIActionSheet alloc]
        initWithTitle:@"This will reset all settings."
             delegate:nil
    cancelButtonTitle:@"Cancel"
    destructiveButtonTitle:@"Reset All Settings"
    otherButtonTitles:nil];
    [sheet showInView:self.view];
    [sheet release];
}

- (void)_eraseAllContent:(PSSpecifier *)specifier
{
    UIActionSheet *sheet = [[UIActionSheet alloc]
        initWithTitle:@"This will delete all media and data, and reset all settings."
             delegate:nil
    cancelButtonTitle:@"Cancel"
    destructiveButtonTitle:@"Erase iPhone"
    otherButtonTitles:nil];
    [sheet showInView:self.view];
    [sheet release];
}

- (NSArray *)specifiers
{
    NSMutableArray *specs = [NSMutableArray array];

    /* ── About ─────────────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"About" controllerClass:@"PSAboutController"]];

    /* ── Software Update ────────────────────────────────────── */
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Software Update" controllerClass:nil]];

    /* ── Usage ──────────────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Usage" controllerClass:nil]];

    /* ── Network ────────────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Network" controllerClass:@"PSNetworkController"]];

    /* Bluetooth */
    PSSpecifier *bluetooth = [PSSpecifier switchSpecifierNamed:@"Bluetooth"
                                                           key:@"bluetoothEnabled"
                                                        target:self
                                                           set:@selector(setBoolForSpecifier:value:)
                                                           get:@selector(getBoolForSpecifier:)];
    bluetooth.defaultValue = [NSNumber numberWithBool:NO];
    [specs addObject:bluetooth];

    /* Location Services */
    PSSpecifier *location = [PSSpecifier switchSpecifierNamed:@"Location Services"
                                                          key:@"locationEnabled"
                                                       target:self
                                                          set:@selector(setBoolForSpecifier:value:)
                                                          get:@selector(getBoolForSpecifier:)];
    location.defaultValue = [NSNumber numberWithBool:YES];
    [specs addObject:location];

    /* ── Auto-Lock ──────────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Auto-Lock" controllerClass:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Passcode Lock" controllerClass:nil]];

    /* ── Keyboard ───────────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Keyboard" controllerClass:nil]];

    /* ── International ──────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"International" controllerClass:nil]];

    /* ── Accessibility ──────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Accessibility" controllerClass:nil]];

    /* ── Reset ──────────────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];
    [specs addObject:[PSSpecifier buttonSpecifierNamed:@"Reset All Settings"
                                               target:self
                                               action:@selector(_resetAllSettings:)]];
    [specs addObject:[PSSpecifier buttonSpecifierNamed:@"Erase All Content and Settings"
                                               target:self
                                               action:@selector(_eraseAllContent:)]];

    return specs;
}

@end
