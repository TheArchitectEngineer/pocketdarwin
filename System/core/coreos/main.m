#import "CoreOS.h"

static void FDPrintUsage(void) {
    fprintf(stderr,
            "Usage:\n"
            "  coreosctl info\n"
            "  coreosctl service start <name> <command> [args...]\n"
            "  coreosctl service stop <name>\n"
            "  coreosctl service status <name>\n");
}

int main(int argc, const char *argv[]) {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    int exitCode = 0;

    if (argc < 2) {
        FDPrintUsage();
        exitCode = 1;
        goto cleanup;
    }

    NSString *action = [NSString stringWithUTF8String:argv[1]];

    if ([action isEqualToString:@"info"]) {
        NSLog(@"System: %@ %@ (kernel %@)", [FDSystem systemName], [FDSystem systemVersion], [FDSystem kernelVersion]);
        exitCode = 0;
        goto cleanup;
    }

    if (![action isEqualToString:@"service"]) {
        FDPrintUsage();
        exitCode = 1;
        goto cleanup;
    }

    if (argc < 4) {
        FDPrintUsage();
        exitCode = 1;
        goto cleanup;
    }

    NSString *verb = [NSString stringWithUTF8String:argv[2]];
    NSString *serviceName = [NSString stringWithUTF8String:argv[3]];

    if ([verb isEqualToString:@"status"]) {
        BOOL running = [FDServiceManager isServiceRunning:serviceName];
        printf("%s\n", running ? "running" : "stopped");
        exitCode = running ? 0 : 3;
        goto cleanup;
    }

    if ([verb isEqualToString:@"stop"]) {
        NSError *error = nil;
        if (![FDServiceManager stopService:serviceName error:&error]) {
            NSLog(@"Failed to stop %@: %@", serviceName, [error localizedDescription]);
            exitCode = 2;
            goto cleanup;
        }
        NSLog(@"Stopped service %@", serviceName);
        exitCode = 0;
        goto cleanup;
    }

    if ([verb isEqualToString:@"start"]) {
        if (argc < 5) {
            FDPrintUsage();
            exitCode = 1;
            goto cleanup;
        }

        NSMutableArray *command = [NSMutableArray array];
        int i = 0;
        for (i = 4; i < argc; i++) {
            [command addObject:[NSString stringWithUTF8String:argv[i]]];
        }

        NSError *error = nil;
        if (![FDServiceManager startService:serviceName command:command error:&error]) {
            NSLog(@"Failed to start %@: %@", serviceName, [error localizedDescription]);
            exitCode = 2;
            goto cleanup;
        }
        NSLog(@"Started service %@", serviceName);
        exitCode = 0;
        goto cleanup;
    }

    FDPrintUsage();
    exitCode = 1;

cleanup:
    [pool drain];
    return exitCode;
}
