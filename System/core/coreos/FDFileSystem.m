#import "FDFileSystem.h"

@implementation FDFileSystem

+ (BOOL)fileExists:(NSString *)path {
    return [[NSFileManager defaultManager] fileExistsAtPath:path];
}

+ (NSArray *)contentsOfDirectory:(NSString *)path {
    NSError *error = nil;
    NSArray *entries = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:path error:&error];
    if (entries == nil) {
        return [NSArray array];
    }

    return entries;
}

+ (BOOL)createDirectory:(NSString *)path {
    NSError *error = nil;
    return [[NSFileManager defaultManager] createDirectoryAtPath:path
                                     withIntermediateDirectories:YES
                                                      attributes:nil
                                                           error:&error];
}

+ (BOOL)removeItem:(NSString *)path {
    NSError *error = nil;
    return [[NSFileManager defaultManager] removeItemAtPath:path error:&error];
}

@end
