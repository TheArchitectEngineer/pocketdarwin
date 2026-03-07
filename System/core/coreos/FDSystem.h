#import "FDCompatibility.h"

@class FDUserSession;

/// CoreOS framework entry point for low-level system information and paths.
@interface FDSystem : NSObject

+ (NSString *)systemName;
+ (NSString *)systemVersion;
+ (NSString *)kernelVersion;

+ (FDUserSession *)currentUserSession;

+ (NSString *)rootDirectory;
+ (NSString *)applicationsDirectory;

+ (NSDictionary *)environment;

@end