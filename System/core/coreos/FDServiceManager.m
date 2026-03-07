#import "FDServiceManager.h"

#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

@implementation FDServiceManager

+ (NSString *)pidFilePathForService:(NSString *)serviceName {
    return [@"/tmp" stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.pid", serviceName]];
}

+ (BOOL)startService:(NSString *)serviceName
             command:(NSArray *)commandArguments
               error:(NSError **)error {
    if ([commandArguments count] == 0) {
        if (error != NULL) {
            NSDictionary *userInfo = [NSDictionary dictionaryWithObject:@"Missing command arguments."
                                                                       forKey:NSLocalizedDescriptionKey];
            *error = [NSError errorWithDomain:@"FDServiceManagerError"
                                         code:1
                                     userInfo:userInfo];
        }
        return NO;
    }

    if ([self isServiceRunning:serviceName]) {
        if (error != NULL) {
            NSDictionary *userInfo = [NSDictionary dictionaryWithObject:@"Service is already running."
                                                                       forKey:NSLocalizedDescriptionKey];
            *error = [NSError errorWithDomain:@"FDServiceManagerError"
                                         code:2
                                     userInfo:userInfo];
        }
        return NO;
    }

    pid_t child = fork();
    if (child < 0) {
        if (error != NULL) {
            NSDictionary *userInfo = [NSDictionary dictionaryWithObject:@"Failed to fork service process."
                                                                       forKey:NSLocalizedDescriptionKey];
            *error = [NSError errorWithDomain:@"FDServiceManagerError"
                                         code:errno
                                     userInfo:userInfo];
        }
        return NO;
    }

    if (child == 0) {
        setsid();

        NSUInteger count = [commandArguments count];
        char **argv = (char **)calloc((count + 1), sizeof(char *));
        if (argv == NULL) {
            _exit(127);
        }

        NSUInteger i = 0;
        for (i = 0; i < count; i++) {
            NSString *argument = [commandArguments objectAtIndex:i];
            argv[i] = strdup([argument UTF8String]);
        }
        argv[count] = NULL;

        execvp(argv[0], argv);
        _exit(127);
    }

    NSString *pidPath = [self pidFilePathForService:serviceName];
    NSString *pidText = [NSString stringWithFormat:@"%d\n", child];
    return [pidText writeToFile:pidPath atomically:YES encoding:NSUTF8StringEncoding error:error];
}

+ (BOOL)stopService:(NSString *)serviceName
              error:(NSError **)error {
    NSString *pidPath = [self pidFilePathForService:serviceName];
    NSString *pidText = [NSString stringWithContentsOfFile:pidPath encoding:NSUTF8StringEncoding error:error];
    if (pidText == nil) {
        return NO;
    }

    pid_t pid = (pid_t)[pidText intValue];
    if (kill(pid, SIGTERM) != 0 && errno != ESRCH) {
        if (error != NULL) {
            NSDictionary *userInfo = [NSDictionary dictionaryWithObject:@"Failed to stop service process."
                                                                       forKey:NSLocalizedDescriptionKey];
            *error = [NSError errorWithDomain:@"FDServiceManagerError"
                                         code:errno
                                     userInfo:userInfo];
        }
        return NO;
    }

    [[NSFileManager defaultManager] removeItemAtPath:pidPath error:NULL];
    return YES;
}

+ (BOOL)isServiceRunning:(NSString *)serviceName {
    NSString *pidPath = [self pidFilePathForService:serviceName];
    NSString *pidText = [NSString stringWithContentsOfFile:pidPath encoding:NSUTF8StringEncoding error:NULL];
    if (pidText == nil) {
        return NO;
    }

    pid_t pid = (pid_t)[pidText intValue];
    if (pid <= 0) {
        return NO;
    }

    if (kill(pid, 0) == 0) {
        return YES;
    }

    if (errno == ESRCH) {
        [[NSFileManager defaultManager] removeItemAtPath:pidPath error:NULL];
    }
    return NO;
}

@end
