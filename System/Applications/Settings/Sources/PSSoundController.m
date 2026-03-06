/*
 * PSSoundController.h / .m
 * Settings.app - PocketDarwin
 *
 * Sound settings: Ringer/Silent toggle, volume sliders,
 * vibrate, ring tone selection.
 *
 * iPhoneOS 2.0 compatible.
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import <UIKit/UIKit.h>
#import "PSViewController.h"
#import "PSSpecifier.h"

/* ---------- Header ---------- */

@interface PSSoundController : PSViewController

- (NSNumber *)getRingerVolume;
- (void)setRingerVolume:(NSNumber *)value;
- (NSNumber *)getAlertsVolume;
- (void)setAlertsVolume:(NSNumber *)value;

@end

/* ---------- Implementation ---------- */

@implementation PSSoundController

- (id)init
{
    if ((self = [super init])) {
        self.title = @"Sounds";
    }
    return self;
}

- (NSNumber *)getRingerVolume
{
    id v = [self readPreferenceValue:[self specifierForKey:@"ringerVolume"]];
    return v ? v : [NSNumber numberWithFloat:0.75f];
}

- (void)setRingerVolume:(NSNumber *)value
{
    [self setPreferenceValue:value specifier:[self specifierForKey:@"ringerVolume"]];
    /* In production: AudioServicesSetProperty(kAudioPropertyRingerVolume, ...) */
}

- (NSNumber *)getAlertsVolume
{
    id v = [self readPreferenceValue:[self specifierForKey:@"alertsVolume"]];
    return v ? v : [NSNumber numberWithFloat:0.75f];
}

- (void)setAlertsVolume:(NSNumber *)value
{
    [self setPreferenceValue:value specifier:[self specifierForKey:@"alertsVolume"]];
}

- (NSArray *)specifiers
{
    NSMutableArray *specs = [NSMutableArray array];

    /* ── Vibrate ───────────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];

    PSSpecifier *vibrateRing = [PSSpecifier switchSpecifierNamed:@"Vibrate on Ring"
                                                             key:@"vibrateOnRing"
                                                          target:self
                                                             set:@selector(setBoolForSpecifier:value:)
                                                             get:@selector(getBoolForSpecifier:)];
    vibrateRing.defaultValue = [NSNumber numberWithBool:YES];
    [specs addObject:vibrateRing];

    PSSpecifier *vibrateSilent = [PSSpecifier switchSpecifierNamed:@"Vibrate on Silent"
                                                               key:@"vibrateOnSilent"
                                                            target:self
                                                               set:@selector(setBoolForSpecifier:value:)
                                                               get:@selector(getBoolForSpecifier:)];
    vibrateSilent.defaultValue = [NSNumber numberWithBool:YES];
    [specs addObject:vibrateSilent];

    /* ── Ringer volume ─────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:@"Ringer and Alerts"]];

    PSSpecifier *ringerVol = [PSSpecifier preferenceSpecifierNamed:nil
                                                            target:self
                                                               set:@selector(setRingerVolume:)
                                                               get:@selector(getRingerVolume)
                                                          cellType:PSSliderCell
                                                          editType:PSSliderCell];
    ringerVol.key = @"ringerVolume";
    ringerVol.defaultValue = [NSNumber numberWithFloat:0.75f];
    [specs addObject:ringerVol];

    PSSpecifier *changeWithButtons = [PSSpecifier switchSpecifierNamed:@"Change with Buttons"
                                                                   key:@"ringerChangeWithButtons"
                                                                target:self
                                                                   set:@selector(setBoolForSpecifier:value:)
                                                                   get:@selector(getBoolForSpecifier:)];
    changeWithButtons.defaultValue = [NSNumber numberWithBool:YES];
    [specs addObject:changeWithButtons];

    /* ── Ring tone ─────────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Ringtone" controllerClass:nil]];

    /* ── Alerts ────────────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"New Mail"       controllerClass:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Sent Mail"      controllerClass:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Calendar Alerts"controllerClass:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Lock Sounds"    controllerClass:nil]];

    PSSpecifier *keyboard = [PSSpecifier switchSpecifierNamed:@"Keyboard Clicks"
                                                          key:@"keyboardClicks"
                                                       target:self
                                                          set:@selector(setBoolForSpecifier:value:)
                                                          get:@selector(getBoolForSpecifier:)];
    keyboard.defaultValue = [NSNumber numberWithBool:YES];
    [specs addObject:keyboard];

    return specs;
}

@end
