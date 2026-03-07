#import <Foundation/Foundation.h>

@protocol MTTerminalSessionDelegate;

@interface MTTerminalSession : NSObject {
    id<MTTerminalSessionDelegate> _delegate;
    int _masterFD;
    pid_t _childPID;
    BOOL _running;
}

- (id)initWithDelegate:(id<MTTerminalSessionDelegate>)delegate;
- (BOOL)start;
- (void)stop;
- (void)sendCommand:(NSString *)command;
@end

@protocol MTTerminalSessionDelegate <NSObject>
- (void)terminalSession:(MTTerminalSession *)session didReceiveOutput:(NSString *)output;
- (void)terminalSessionDidExit:(MTTerminalSession *)session;
@end
