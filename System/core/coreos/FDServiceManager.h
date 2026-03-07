#import "FDCompatibility.h"

/// Minimal service lifecycle helpers for CoreOS command-line usage.
@interface FDServiceManager : NSObject

+ (NSString *)pidFilePathForService:(NSString *)serviceName;

+ (BOOL)startService:(NSString *)serviceName
             command:(NSArray *)commandArguments
               error:(NSError **)error;

+ (BOOL)stopService:(NSString *)serviceName
              error:(NSError **)error;

+ (BOOL)isServiceRunning:(NSString *)serviceName;

@end
