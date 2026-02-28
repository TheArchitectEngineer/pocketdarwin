/*
 * PSAboutController.m
 * Settings.app - PocketDarwin
 *
 * iPhoneOS 2.0 — Manual Retain/Release
 *
 * Reads device information via sysctl and CFPreferences
 * and displays it in a table.
 */

#import <UIKit/UIKit.h>
#import <sys/sysctl.h>
#import "PSAboutController.h"
#import "PSSpecifier.h"

static NSString *_sysctlString(const char *name)
{
    size_t len = 0;
    sysctlbyname(name, NULL, &len, NULL, 0);
    if (len == 0) return @"Unknown";

    char *buf = (char *)malloc(len);
    if (!buf) return @"Unknown";
    sysctlbyname(name, buf, &len, NULL, 0);
    NSString *result = [NSString stringWithUTF8String:buf];
    free(buf);
    return result ? result : @"Unknown";
}

@implementation PSAboutController

- (id)init
{
    if ((self = [super init])) {
        self.title = @"About";
    }
    return self;
}

- (NSArray *)specifiers
{
    NSMutableArray *specs = [NSMutableArray array];

    /* ── Device name ── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];
    [specs addObject:[PSSpecifier titleValueSpecifierNamed:@"Name"
                                                    value:@"PocketDarwin"]];

    /* ── Hardware info ── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];

    NSString *hwModel = _sysctlString("hw.machine");
    [specs addObject:[PSSpecifier titleValueSpecifierNamed:@"Model"
                                                    value:hwModel]];

    /* Storage — read /var partition size */
    NSString *capacity = @"N/A";
    NSDictionary *fsAttrs = [[NSFileManager defaultManager]
                             attributesOfFileSystemForPath:@"/var" error:nil];
    if (fsAttrs) {
        unsigned long long total = [[fsAttrs objectForKey:NSFileSystemSize] unsignedLongLongValue];
        unsigned long long totalGB = total / (1024ULL * 1024ULL * 1024ULL);
        capacity = [NSString stringWithFormat:@"%llu GB", totalGB];
    }
    [specs addObject:[PSSpecifier titleValueSpecifierNamed:@"Capacity"
                                                    value:capacity]];

    /* Available */
    NSString *available = @"N/A";
    if (fsAttrs) {
        unsigned long long free = [[fsAttrs objectForKey:NSFileSystemFreeSize] unsignedLongLongValue];
        unsigned long long freeMB = free / (1024ULL * 1024ULL);
        available = [NSString stringWithFormat:@"%llu MB", freeMB];
    }
    [specs addObject:[PSSpecifier titleValueSpecifierNamed:@"Available"
                                                    value:available]];

    /* ── Software ── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];

    [specs addObject:[PSSpecifier titleValueSpecifierNamed:@"Version"
                                                    value:@"PocketDarwin 2.0"]];

    /* Kernel version */
    NSString *kernel = _sysctlString("kern.osrelease");
    [specs addObject:[PSSpecifier titleValueSpecifierNamed:@"Kernel Version"
                                                    value:kernel]];

    /* ── Network ── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];

    [specs addObject:[PSSpecifier titleValueSpecifierNamed:@"Wi-Fi Address"
                                                    value:@"Not Connected"]];

    [specs addObject:[PSSpecifier titleValueSpecifierNamed:@"Bluetooth"
                                                    value:@"N/A"]];

    /* ── Serial & IMEI (PocketDarwin: N/A) ── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];

    [specs addObject:[PSSpecifier titleValueSpecifierNamed:@"Serial Number"
                                                    value:@"PD0000000000"]];

    [specs addObject:[PSSpecifier titleValueSpecifierNamed:@"IMEI"
                                                    value:@"N/A (PocketDarwin)"]];

    /* ── Legal ── */
    [specs addObject:[PSSpecifier groupSpecifierWithName:nil]];

    [specs addObject:[PSSpecifier linkSpecifierNamed:@"Legal"
                                    controllerClass:nil]];

    return specs;
}

@end
