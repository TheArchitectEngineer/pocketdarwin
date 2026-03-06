#import "MobileTerminalAppDelegate.h"
#import "MTTerminalViewController.h"

@implementation MobileTerminalAppDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    _window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    _terminalViewController = [[MTTerminalViewController alloc] init];
    [_window addSubview:[_terminalViewController view]];
    [_window makeKeyAndVisible];
    (void)application;
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    (void)application;
}

- (void)dealloc
{
    [_terminalViewController release];
    [_window release];
    [super dealloc];
}

@end
