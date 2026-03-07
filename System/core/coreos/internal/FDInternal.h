#import "FDCompatibility.h"

/// Shared internal helpers used by CoreOS implementation files.
static inline NSDictionary *FDEnvironmentDictionary(void) {
    NSMutableDictionary *environment = [NSMutableDictionary dictionary];

    extern char **environ;

    if (environ == NULL) {
        return environment;
    }

    char **entry = environ;
    for (; *entry != NULL; entry++) {
        NSString *line = [NSString stringWithUTF8String:*entry];
        NSRange separator = [line rangeOfString:@"="];

        if (separator.location == NSNotFound) {
            continue;
        }

        NSString *key = [line substringToIndex:separator.location];
        NSString *value = [line substringFromIndex:(separator.location + 1)];

        if ([key length] > 0) {
            [environment setObject:value forKey:key];
        }
    }

    return environment;
}