#import "FDSystem.h"

#import <sys/utsname.h>

#import "FDUserSession.h"
#import "internal/FDInternal.h"

@implementation FDSystem

+ (NSString *)systemName {
    struct utsname info;
    if (uname(&info) == 0) {
        return [NSString stringWithUTF8String:info.sysname];
    }

    return @"FreeDarwin";
}

+ (NSString *)systemVersion {
    struct utsname info;
    if (uname(&info) == 0) {
        return [NSString stringWithUTF8String:info.version];
    }

    return @"0.1";
}

+ (NSString *)kernelVersion {
    struct utsname info;
    if (uname(&info) == 0) {
        return [NSString stringWithUTF8String:info.release];
    }

    return @"unknown";
}

+ (FDUserSession *)currentUserSession {
    return [FDUserSession currentSession];
}

+ (NSString *)rootDirectory {
    return @"/";
}

+ (NSString *)applicationsDirectory {
    return @"/Applications";
}

+ (NSDictionary *)environment {
    return FDEnvironmentDictionary();
}

@end
