#import "FDStartup.h"

#import <spawn.h>
#import <errno.h>

NSString * const FDStartupErrorDomain = @"com.pocketdarwin.SystemStartup";

extern char **environ;

enum {
    FDStartupErrorMissingInfoPlist = 1,
    FDStartupErrorMissingExecutable = 2,
    FDStartupErrorSpawnFailed = 3
};

@interface FDStartup ()
- (BOOL)_loadBundleInfo:(NSError **)error;
@end

@implementation FDStartup

@synthesize displayName = _displayName;
@synthesize bundleIdentifier = _bundleIdentifier;
@synthesize bundlePath = _bundlePath;
@synthesize executablePath = _executablePath;
@synthesize icon = _icon;

- (id)initWithBundlePath:(NSString *)bundlePath {
    self = [super init];
    if (self != nil) {
        _bundlePath = [bundlePath copy];
        [self _loadBundleInfo:NULL];
    }
    return self;
}

+ (NSArray *)_defaultSearchPaths {
    return [NSArray arrayWithObjects:@"/System/Library/CoreServices", @"/Applications", nil];
}

+ (NSArray *)_bundlePathsFromConfiguration:(NSDictionary *)configuration {
    NSMutableArray *bundlePaths = [NSMutableArray array];
    NSArray *explicitBundles = [configuration objectForKey:@"StartupBundlePaths"];
    if ([explicitBundles isKindOfClass:[NSArray class]]) {
        [bundlePaths addObjectsFromArray:explicitBundles];
    }

    NSArray *searchPaths = [configuration objectForKey:@"StartupSearchPaths"];
    if (![searchPaths isKindOfClass:[NSArray class]] || [searchPaths count] == 0) {
        searchPaths = [self _defaultSearchPaths];
    }

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSEnumerator *pathEnumerator = [searchPaths objectEnumerator];
    NSString *directoryPath = nil;
    while ((directoryPath = [pathEnumerator nextObject]) != nil) {
        BOOL isDirectory = NO;
        if (![fileManager fileExistsAtPath:directoryPath isDirectory:&isDirectory] || !isDirectory) {
            continue;
        }

        NSError *directoryError = nil;
        NSArray *entries = [fileManager contentsOfDirectoryAtPath:directoryPath error:&directoryError];
        if (entries == nil) {
            continue;
        }

        NSEnumerator *entryEnumerator = [entries objectEnumerator];
        NSString *entry = nil;
        while ((entry = [entryEnumerator nextObject]) != nil) {
            if (![entry hasSuffix:@".app"]) {
                continue;
            }
            [bundlePaths addObject:[directoryPath stringByAppendingPathComponent:entry]];
        }
    }

    return bundlePaths;
}

+ (NSArray *)startupBundlesFromConfiguration:(NSString *)configurationPath {
    NSDictionary *configuration = nil;
    if (configurationPath != nil) {
        configuration = [NSDictionary dictionaryWithContentsOfFile:configurationPath];
    }
    if (![configuration isKindOfClass:[NSDictionary class]]) {
        configuration = [NSDictionary dictionary];
    }

    NSArray *bundlePaths = [self _bundlePathsFromConfiguration:configuration];
    NSMutableSet *seenPaths = [NSMutableSet set];
    NSMutableArray *bundles = [NSMutableArray array];

    NSEnumerator *pathEnumerator = [bundlePaths objectEnumerator];
    NSString *bundlePath = nil;
    while ((bundlePath = [pathEnumerator nextObject]) != nil) {
        if (![bundlePath isKindOfClass:[NSString class]]) {
            continue;
        }

        if ([seenPaths containsObject:bundlePath]) {
            continue;
        }
        [seenPaths addObject:bundlePath];

        FDStartup *startupBundle = [[[FDStartup alloc] initWithBundlePath:bundlePath] autorelease];
        if (startupBundle != nil && startupBundle.executablePath != nil) {
            [bundles addObject:startupBundle];
        }
    }

    [bundles sortUsingSelector:@selector(compare:)];

    NSString *preferredBundleID = [configuration objectForKey:@"StartupPreferredBundleIdentifier"];
    if ([preferredBundleID isKindOfClass:[NSString class]]) {
        NSUInteger preferredIndex = NSNotFound;
        NSUInteger i = 0;
        for (FDStartup *bundle in bundles) {
            if ([bundle.bundleIdentifier isEqualToString:preferredBundleID]) {
                preferredIndex = i;
                break;
            }
            i++;
        }
        if (preferredIndex != NSNotFound && preferredIndex != 0) {
            FDStartup *preferred = [[[bundles objectAtIndex:preferredIndex] retain] autorelease];
            [bundles removeObjectAtIndex:preferredIndex];
            [bundles insertObject:preferred atIndex:0];
        }
    }

    return bundles;
}

- (NSComparisonResult)compare:(FDStartup *)otherBundle {
    NSString *left = self.displayName ? self.displayName : @"";
    NSString *right = otherBundle.displayName ? otherBundle.displayName : @"";
    return [left caseInsensitiveCompare:right];
}

- (BOOL)_loadBundleInfo:(NSError **)error {
    NSString *plistPath = [_bundlePath stringByAppendingPathComponent:@"Info.plist"];
    NSDictionary *info = [NSDictionary dictionaryWithContentsOfFile:plistPath];
    if (![info isKindOfClass:[NSDictionary class]]) {
        if (error != NULL) {
            *error = [NSError errorWithDomain:FDStartupErrorDomain
                                         code:FDStartupErrorMissingInfoPlist
                                     userInfo:[NSDictionary dictionaryWithObject:@"Invalid or missing Info.plist"
                                                                          forKey:NSLocalizedDescriptionKey]];
        }
        return NO;
    }

    NSString *displayName = [info objectForKey:@"CFBundleDisplayName"];
    if (displayName == nil) {
        displayName = [info objectForKey:@"CFBundleName"];
    }
    if (displayName == nil) {
        displayName = [[_bundlePath lastPathComponent] stringByDeletingPathExtension];
    }
    self.displayName = displayName;
    self.bundleIdentifier = [info objectForKey:@"CFBundleIdentifier"];

    NSString *executableName = [info objectForKey:@"CFBundleExecutable"];
    if (![executableName isKindOfClass:[NSString class]] || [executableName length] == 0) {
        if (error != NULL) {
            *error = [NSError errorWithDomain:FDStartupErrorDomain
                                         code:FDStartupErrorMissingExecutable
                                     userInfo:[NSDictionary dictionaryWithObject:@"CFBundleExecutable not set"
                                                                          forKey:NSLocalizedDescriptionKey]];
        }
        return NO;
    }
    self.executablePath = [_bundlePath stringByAppendingPathComponent:executableName];

    NSString *iconFile = [info objectForKey:@"CFBundleIconFile"];
    if (![iconFile isKindOfClass:[NSString class]] || [iconFile length] == 0) {
        iconFile = @"Icon.png";
    }
    NSString *iconPath = [_bundlePath stringByAppendingPathComponent:iconFile];
    UIImage *iconImage = [UIImage imageWithContentsOfFile:iconPath];
    if (iconImage == nil && ![iconFile hasSuffix:@".png"]) {
        iconPath = [_bundlePath stringByAppendingPathComponent:[iconFile stringByAppendingString:@".png"]];
        iconImage = [UIImage imageWithContentsOfFile:iconPath];
    }
    self.icon = iconImage;

    return YES;
}

- (BOOL)launchAndReturnError:(NSError **)error {
    if (_executablePath == nil || [[NSFileManager defaultManager] fileExistsAtPath:_executablePath] == NO) {
        if (error != NULL) {
            *error = [NSError errorWithDomain:FDStartupErrorDomain
                                         code:FDStartupErrorMissingExecutable
                                     userInfo:[NSDictionary dictionaryWithObject:@"Startup bundle executable not found"
                                                                          forKey:NSLocalizedDescriptionKey]];
        }
        return NO;
    }

    const char *path = [_executablePath UTF8String];
    char * const argv[] = { (char *)path, NULL };
    pid_t child = 0;
    int spawnResult = posix_spawn(&child, path, NULL, NULL, argv, environ);
    if (spawnResult != 0) {
        if (error != NULL) {
            NSString *message = [NSString stringWithFormat:@"posix_spawn failed (%d: %s)",
                                 spawnResult,
                                 strerror(spawnResult)];
            *error = [NSError errorWithDomain:FDStartupErrorDomain
                                         code:FDStartupErrorSpawnFailed
                                     userInfo:[NSDictionary dictionaryWithObject:message
                                                                          forKey:NSLocalizedDescriptionKey]];
        }
        return NO;
    }

    return YES;
}

- (void)dealloc {
    [_displayName release];
    [_bundleIdentifier release];
    [_bundlePath release];
    [_executablePath release];
    [_icon release];
    [super dealloc];
}

@end
