/*
 * PSNetworkController.h / .m
 * Settings.app - PocketDarwin
 *
 * Network settings: Carrier, Cellular Data, VPN, Wi-Fi calling.
 * On PocketDarwin there is no cellular radio, so cellular rows
 * are greyed out. VPN configuration is shown.
 *
 * iPhoneOS 2.0 compatible.
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import <UIKit/UIKit.h>
#import "PSViewController.h"
#import "PSSpecifier.h"

/* ---------- Header ---------- */

@interface PSNetworkController : PSViewController
@end

/* ---------- Implementation ---------- */

@implementation PSNetworkController

- (id)init
{
    if ((self = [super init])) {
        self.title = @"Network";
    }
    return self;
}

- (NSArray *)specifiers
{
    NSMutableArray *specs = [NSMutableArray array];

    /* ── Cellular Data ─────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:@"Cellular"]];

    PSSpecifier *cellData = [PSSpecifier switchSpecifierNamed:@"Cellular Data"
                                                          key:@"cellularData"
                                                       target:self
                                                          set:@selector(setBoolForSpecifier:value:)
                                                          get:@selector(getBoolForSpecifier:)];
    cellData.defaultValue = [NSNumber numberWithBool:NO];
    cellData.enabled = NO; /* PocketDarwin: no cellular radio */
    [specs addObject:cellData];

    PSSpecifier *dataRoaming = [PSSpecifier switchSpecifierNamed:@"Data Roaming"
                                                             key:@"dataRoaming"
                                                          target:self
                                                             set:@selector(setBoolForSpecifier:value:)
                                                             get:@selector(getBoolForSpecifier:)];
    dataRoaming.defaultValue = [NSNumber numberWithBool:NO];
    dataRoaming.enabled = NO;
    [specs addObject:dataRoaming];

    /* ── VPN ───────────────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:@"VPN"]];

    PSSpecifier *vpnSwitch = [PSSpecifier switchSpecifierNamed:@"VPN"
                                                           key:@"vpnEnabled"
                                                        target:self
                                                           set:@selector(setBoolForSpecifier:value:)
                                                           get:@selector(getBoolForSpecifier:)];
    vpnSwitch.defaultValue = [NSNumber numberWithBool:NO];
    [specs addObject:vpnSwitch];

    [specs addObject:[PSSpecifier linkSpecifierNamed:@"VPN Settings" controllerClass:nil]];

    /* ── Wi-Fi ─────────────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:@"Wi-Fi"]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Wi-Fi" controllerClass:@"PSWiFiController"]];

    /* ── DNS / Proxy ───────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:@"Advanced"]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"DNS Servers" controllerClass:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"HTTP Proxy"  controllerClass:nil]];

    return specs;
}

@end
