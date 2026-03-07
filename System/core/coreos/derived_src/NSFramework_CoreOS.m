#include <Foundation/NSString.h>
@interface NSFramework_CoreOS : NSObject
+ (NSString *)frameworkVersion;
+ (NSString *const*)frameworkClasses;
@end
@implementation NSFramework_CoreOS
+ (NSString *)frameworkVersion { return @"0"; }
static NSString *allClasses[] = {@"FDSystem", @"FDUserSession", @"FDProcessInfo", @"FDFileSystem", @"FDServiceManager", NULL};
+ (NSString *const*)frameworkClasses { return allClasses; }
@end
