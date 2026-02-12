//
//  UIApplication.h
//  pdUIKit - PocketDarwin UIKit Framework
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "UIWindow.h"

NS_ASSUME_NONNULL_BEGIN

// Application states
typedef NS_ENUM(NSInteger, UIApplicationState) {
    UIApplicationStateActive,
    UIApplicationStateInactive,
    UIApplicationStateBackground,
};

// Launch options keys
UIKIT_EXTERN NSString * const UIApplicationLaunchOptionsURLKey;
UIKIT_EXTERN NSString * const UIApplicationLaunchOptionsSourceApplicationKey;
UIKIT_EXTERN NSString * const UIApplicationLaunchOptionsRemoteNotificationKey;

// Notification names
UIKIT_EXTERN NSNotificationName const UIApplicationDidBecomeActiveNotification;
UIKIT_EXTERN NSNotificationName const UIApplicationWillResignActiveNotification;
UIKIT_EXTERN NSNotificationName const UIApplicationDidEnterBackgroundNotification;
UIKIT_EXTERN NSNotificationName const UIApplicationWillEnterForegroundNotification;

// Forward declarations
@class UIApplication, UIEvent, UITouch;

@protocol UIApplicationDelegate <NSObject>

@optional
// Application lifecycle
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;
- (void)applicationDidBecomeActive:(UIApplication *)application;
- (void)applicationWillResignActive:(UIApplication *)application;
- (void)applicationDidEnterBackground:(UIApplication *)application;
- (void)applicationWillEnterForeground:(UIApplication *)application;

// URL handling
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary *)options;

// Memory warnings
- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application;

// Termination
- (void)applicationWillTerminate:(UIApplication *)application;

@end

// UIApplication class - adaptation of NSApplication for UIKit
@interface UIApplication : NSObject

// Singleton access
+ (UIApplication *)sharedApplication;

// Application state
@property (nonatomic, readonly) UIApplicationState applicationState;
@property (nonatomic, readonly) BOOL isNetworkActivityIndicatorVisible;

// Application configuration
@property (nonatomic, weak, nullable) id<UIApplicationDelegate> delegate;
@property (nonatomic, strong, nullable) UIWindow *window;
@property (nonatomic, readonly) NSTimeInterval backgroundTimeRemaining;

// Status bar control
@property (nonatomic, readonly, getter=isStatusBarHidden) BOOL statusBarHidden;
@property (nonatomic, readonly) UIStatusBarStyle statusBarStyle;

// Background tasks
- (UIBackgroundTaskIdentifier)beginBackgroundTaskWithName:(nullable NSString *)taskName 
                                      expirationHandler:(void (^ __nullable)(void))handler;
- (UIBackgroundTaskIdentifier)beginBackgroundTaskWithExpirationHandler:(void (^ __nullable)(void))handler;
- (void)endBackgroundTask:(UIBackgroundTaskIdentifier)identifier;

// URL handling
- (BOOL)canOpenURL:(NSURL *)url;
- (BOOL)openURL:(NSURL *)url options:(NSDictionary *)options completionHandler:(void (^ __nullable)(BOOL success))completion;

// Application lifecycle
- (void)performSelector:(SEL)aSelector withObject:(nullable id)anObject afterDelay:(NSTimeInterval)delay;
- (void)performSelector:(SEL)aSelector withObject:(nullable id)anObject afterDelay:(NSTimeInterval)delay inModes:(NSArray<NSRunLoopMode> *)modes;
- (void)cancelPreviousPerformRequestsWithTarget:(id)aTarget selector:(SEL)aSelector object:(nullable id)anObject;

// GNUStep compatibility
- (NSApplication *)NSApplicationValue;

@end

NS_ASSUME_NONNULL_END
