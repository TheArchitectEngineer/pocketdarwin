/*
 * SettingsAppDelegate.m
 * Settings.app - PocketDarwin
 *
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import <UIKit/UIKit.h>
#import "SettingsAppDelegate.h"
#import "PSSettingsRootController.h"

@implementation SettingsAppDelegate

@synthesize window               = _window;
@synthesize navigationController = _navigationController;

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    /* iPhoneOS 2.0: no launchOptions parameter */

    _window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

    /* Root controller */
    _rootController = [[PSSettingsRootController alloc] init];

    /* Navigation controller wrapping the root */
    _navigationController = [[UINavigationController alloc]
                              initWithRootViewController:_rootController];

    /*
     * iPhoneOS 2.0 Settings navigation bar style:
     * Light grey gradient with black text.
     * UINavigationBar.tintColor controls the bar colour.
     */
    _navigationController.navigationBar.barStyle = UIBarStyleDefault;
    _navigationController.navigationBar.tintColor =
        [UIColor colorWithRed:0.55f green:0.60f blue:0.68f alpha:1.0f];

    /* iPhoneOS 2.0: addSubview instead of rootViewController */
    [_window addSubview:_navigationController.view];
    [_window makeKeyAndVisible];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    NSLog(@"PocketDarwin Settings: terminating.");
}

- (void)dealloc
{
    [_rootController       release];
    [_navigationController release];
    [_window               release];
    [super dealloc];
}

@end
