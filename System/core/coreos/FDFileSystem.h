#import "FDCompatibility.h"

/// File system helpers shared by low-level system tools.
@interface FDFileSystem : NSObject

+ (BOOL)fileExists:(NSString *)path;

+ (NSArray *)contentsOfDirectory:(NSString *)path;

+ (BOOL)createDirectory:(NSString *)path;

+ (BOOL)removeItem:(NSString *)path;

@end