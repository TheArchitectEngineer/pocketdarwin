#import "FDProcessInfo.h"

#include <unistd.h>

@implementation FDProcessInfo

+ (pid_t)processIdentifier {
    return getpid();
}

+ (pid_t)parentProcessIdentifier {
    return getppid();
}

+ (NSString *)processName {
    return [[NSProcessInfo processInfo] processName];
}

+ (NSArray *)arguments {
    return [[NSProcessInfo processInfo] arguments];
}

@end
