#import "FDUserSession.h"

#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>

@interface FDUserSession ()

- (instancetype)initWithUsername:(NSString *)username
                   homeDirectory:(NSString *)homeDirectory
                             uid:(uid_t)uid
                             gid:(gid_t)gid NS_DESIGNATED_INITIALIZER;

@end

@implementation FDUserSession

- (instancetype)init {
    return [[self class] currentSession];
}

+ (FDUserSession *)currentSession {
    uid_t uid = getuid();
    struct passwd *entry = getpwuid(uid);

    NSString *username = @"mobile";
    gid_t gid = getgid();

    if (entry != NULL) {
        if (entry->pw_name != NULL) {
            username = [NSString stringWithUTF8String:entry->pw_name];
        }
        gid = entry->pw_gid;
    }

    const char *homeValue = getenv("HOME");
    NSString *homeDirectory = nil;

    if (homeValue != NULL) {
        homeDirectory = [NSString stringWithUTF8String:homeValue];
    } else if (entry != NULL && entry->pw_dir != NULL) {
        homeDirectory = [NSString stringWithUTF8String:entry->pw_dir];
    }

    if (homeDirectory == nil || [homeDirectory length] == 0) {
        homeDirectory = @"/";
    }

    return [[self alloc] initWithUsername:username
                            homeDirectory:homeDirectory
                                      uid:uid
                                      gid:gid];
}

- (instancetype)initWithUsername:(NSString *)username
                   homeDirectory:(NSString *)homeDirectory
                             uid:(uid_t)uid
                             gid:(gid_t)gid {
    self = [super init];
    if (self != nil) {
        _username = [username copy];
        _homeDirectory = [homeDirectory copy];
        _uid = uid;
        _gid = gid;
    }

    return self;
}

- (NSString *)username {
    return _username;
}

- (NSString *)homeDirectory {
    return _homeDirectory;
}

- (uid_t)uid {
    return _uid;
}

- (gid_t)gid {
    return _gid;
}

@end
