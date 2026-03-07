#import "FDCompatibility.h"

#include <sys/types.h>

/// Represents the active login session for the current single PocketDarwin user.
@interface FDUserSession : NSObject {
@private
    NSString *_username;
    NSString *_homeDirectory;
    uid_t _uid;
    gid_t _gid;
}

@property (readonly, nonatomic, copy) NSString *username;
@property (readonly, nonatomic, copy) NSString *homeDirectory;
@property (readonly, nonatomic) uid_t uid;
@property (readonly, nonatomic) gid_t gid;

+ (FDUserSession *)currentSession;


@end