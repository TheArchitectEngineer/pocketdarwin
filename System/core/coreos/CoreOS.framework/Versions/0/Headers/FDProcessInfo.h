#import "FDCompatibility.h"

#include <sys/types.h>

/// Lightweight process inspection utilities for the current process.
@interface FDProcessInfo : NSObject

+ (pid_t)processIdentifier;
+ (pid_t)parentProcessIdentifier;

+ (NSString *)processName;

+ (NSArray *)arguments;

@end