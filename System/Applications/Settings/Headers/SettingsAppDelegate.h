/*
 * SettingsAppDelegate.h
 * Settings.app - PocketDarwin
 *
 * Application delegate for Settings.app.
 * Sets up a UINavigationController with PSSettingsRootController
 * as the root view, matching iPhoneOS 2.0 Settings.app structure.
 *
 * iPhoneOS 2.0 compatible.
 */

#import <UIKit/UIKit.h>

@class PSSettingsRootController;

@interface SettingsAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow                *_window;
    UINavigationController  *_navigationController;
    PSSettingsRootController *_rootController;
}

@property (nonatomic, retain) UIWindow               *window;
@property (nonatomic, retain) UINavigationController *navigationController;

@end
