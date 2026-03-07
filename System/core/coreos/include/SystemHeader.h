#ifndef COREOS_SYSTEMHEADER_H
#define COREOS_SYSTEMHEADER_H

#import <Foundation/Foundation.h>

typedef enum {
    FDSystemPlatformUnknown = 0,
    FDSystemPlatformiPhoneOS,
    FDSystemPlatformMacOS,
    FDSystemPlatformGNUstep
} FDSystemPlatform;

@interface FDSystemTypeIdentifier : NSObject
+ (FDSystemPlatform)currentPlatform;
+ (NSString *)currentPlatformName;
@end

#endif
