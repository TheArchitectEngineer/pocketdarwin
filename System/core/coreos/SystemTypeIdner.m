#import "include/SystemHeader.h"

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

@implementation FDSystemTypeIdentifier

+ (FDSystemPlatform)currentPlatform
{
#ifdef __APPLE__
#if TARGET_OS_IPHONE
    return FDSystemPlatformiPhoneOS;
#elif TARGET_OS_MAC
    return FDSystemPlatformMacOS;
#else
    return FDSystemPlatformUnknown;
#endif
#else
    return FDSystemPlatformGNUstep;
#endif
}

+ (NSString *)currentPlatformName
{
    switch ([self currentPlatform]) {
        case FDSystemPlatformiPhoneOS:
            return @"iPhoneOS";
        case FDSystemPlatformMacOS:
            return @"macOS";
        case FDSystemPlatformGNUstep:
            return @"GNUstep";
        default:
            return @"Unknown";
    }
}

@end
