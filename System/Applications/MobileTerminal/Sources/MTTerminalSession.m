#import "MTTerminalSession.h"

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <util.h>

@interface MTTerminalSession ()
- (void)readLoop;
- (void)dispatchOutputBytes:(const char *)bytes length:(ssize_t)length;
- (void)notifyExit;
@end

@implementation MTTerminalSession

- (id)initWithDelegate:(id<MTTerminalSessionDelegate>)delegate
{
    self = [super init];
    if (self != nil) {
        _delegate = delegate;
        _masterFD = -1;
        _childPID = -1;
        _running = NO;
    }
    return self;
}

- (BOOL)start
{
    if (_running) {
        return YES;
    }

    struct winsize ws;
    memset(&ws, 0, sizeof(ws));
    ws.ws_col = 80;
    ws.ws_row = 24;

    _childPID = forkpty(&_masterFD, NULL, NULL, &ws);
    if (_childPID < 0) {
        return NO;
    }

    if (_childPID == 0) {
        const char *shell = "/bin/sh";
        setenv("TERM", "xterm", 1);
        setenv("HOME", "/var/root", 1);
        setenv("PATH", "/usr/bin:/bin:/usr/sbin:/sbin", 1);
        execl(shell, shell, "-l", (char *)NULL);
        _exit(127);
    }

    _running = YES;
    [NSThread detachNewThreadSelector:@selector(readLoop) toTarget:self withObject:nil];
    return YES;
}

- (void)stop
{
    if (!_running) {
        return;
    }

    _running = NO;

    if (_childPID > 0) {
        kill(_childPID, SIGTERM);
        waitpid(_childPID, NULL, 0);
        _childPID = -1;
    }

    if (_masterFD >= 0) {
        close(_masterFD);
        _masterFD = -1;
    }
}

- (void)sendCommand:(NSString *)command
{
    if (!_running || _masterFD < 0 || command == nil) {
        return;
    }

    NSData *commandData = [command dataUsingEncoding:NSUTF8StringEncoding];
    if ([commandData length] > 0) {
        write(_masterFD, [commandData bytes], [commandData length]);
    }
}

- (void)readLoop
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    char buffer[512];
    while (_running) {
        ssize_t count = read(_masterFD, buffer, sizeof(buffer));
        if (count > 0) {
            [self dispatchOutputBytes:buffer length:count];
            continue;
        }

        if (count == 0) {
            break;
        }

        if (errno == EINTR) {
            continue;
        }

        break;
    }

    _running = NO;

    if (_masterFD >= 0) {
        close(_masterFD);
        _masterFD = -1;
    }

    if (_childPID > 0) {
        waitpid(_childPID, NULL, 0);
        _childPID = -1;
    }

    [self performSelectorOnMainThread:@selector(notifyExit) withObject:nil waitUntilDone:NO];

    [pool release];
}

- (void)dispatchOutputBytes:(const char *)bytes length:(ssize_t)length
{
    NSData *outputData = [NSData dataWithBytes:bytes length:(NSUInteger)length];
    NSString *output = [[[NSString alloc] initWithData:outputData encoding:NSUTF8StringEncoding] autorelease];
    if (output == nil) {
        output = [[[NSString alloc] initWithData:outputData encoding:NSISOLatin1StringEncoding] autorelease];
    }

    if (output != nil && [_delegate respondsToSelector:@selector(terminalSession:didReceiveOutput:)]) {
        NSDictionary *payload = [NSDictionary dictionaryWithObjectsAndKeys:self, @"session", output, @"output", nil];
        [self performSelectorOnMainThread:@selector(deliverOutput:) withObject:payload waitUntilDone:NO];
    }
}

- (void)deliverOutput:(NSDictionary *)payload
{
    MTTerminalSession *session = [payload objectForKey:@"session"];
    NSString *output = [payload objectForKey:@"output"];
    [_delegate terminalSession:session didReceiveOutput:output];
}

- (void)notifyExit
{
    if ([_delegate respondsToSelector:@selector(terminalSessionDidExit:)]) {
        [_delegate terminalSessionDidExit:self];
    }
}

- (void)dealloc
{
    [self stop];
    [super dealloc];
}

@end
