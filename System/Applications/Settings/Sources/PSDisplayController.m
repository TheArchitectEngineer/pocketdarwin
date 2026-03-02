/*
 * PSDisplayController.h / .m
 * Settings.app - PocketDarwin
 *
 * Brightness & Wallpaper settings panel.
 * Adjusts screen brightness via IOKit backlight control.
 *
 * iPhoneOS 2.0 compatible.
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import <UIKit/UIKit.h>
#import "PSViewController.h"
#import "PSSpecifier.h"

/* ---------- Header ---------- */

@interface PSDisplayController : PSViewController

- (NSNumber *)readBrightnessValue;
- (void)applyBrightnessValue:(NSNumber *)value;

@end

/* ---------- Implementation ---------- */

@implementation PSDisplayController

- (id)init
{
    if ((self = [super init])) {
        self.title = @"Brightness";
    }
    return self;
}

- (NSNumber *)readBrightnessValue
{
    id v = [self readPreferenceValue:[self specifierForKey:@"screenBrightness"]];
    if (v) return v;
    return [NSNumber numberWithFloat:0.5f];
}

- (void)applyBrightnessValue:(NSNumber *)value
{
    [self setPreferenceValue:value specifier:[self specifierForKey:@"screenBrightness"]];

    /* Runtime brightness apply is intentionally omitted in this SDK shim build. */
}

- (NSArray *)specifiers
{
    NSMutableArray *specs = [NSMutableArray array];

    /* ── Brightness slider ─────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];

    PSSpecifier *brightness = [PSSpecifier preferenceSpecifierNamed:nil
                                                             target:self
                                                                set:@selector(applyBrightnessValue:)
                                                                get:@selector(readBrightnessValue)
                                                           cellType:PSSliderCell
                                                           editType:PSSliderCell];
    brightness.key = @"screenBrightness";
    brightness.defaultValue = [NSNumber numberWithFloat:0.5f];
    [specs addObject:brightness];

    /* Auto-brightness */
    PSSpecifier *autoBright = [PSSpecifier switchSpecifierNamed:@"Auto-Brightness"
                                                            key:@"autoBrightness"
                                                         target:self
                                                            set:@selector(setBoolForSpecifier:value:)
                                                            get:@selector(getBoolForSpecifier:)];
    autoBright.defaultValue = [NSNumber numberWithBool:YES];
    [specs addObject:autoBright];

    /* ── Wallpaper ─────────────────────────────────────────── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];
    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Wallpaper" controllerClass:nil]];

    return specs;
}

@end
