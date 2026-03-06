/*
 * SBApplication.h
 * SpringBoard - PocketDarwin
 *
 * Represents a launchable application on the home screen.
 * Compatible with iPhoneOS 2.0 SDK.
 */

#import <Foundation/Foundation.h>

@interface SBApplication : NSObject {
    NSString        *_displayName;
    NSString        *_bundleIdentifier;
    NSString        *_bundlePath;
    NSString        *_executablePath;
    UIImage         *_icon;
    UIImage         *_smallIcon;
    BOOL             _isSystemApplication;
    BOOL             _isHidden;
    NSInteger        _badgeCount;
}

@property (nonatomic, copy)   NSString  *displayName;
@property (nonatomic, copy)   NSString  *bundleIdentifier;
@property (nonatomic, copy)   NSString  *bundlePath;
@property (nonatomic, copy)   NSString  *executablePath;
@property (nonatomic, retain) UIImage   *icon;
@property (nonatomic, retain) UIImage   *smallIcon;
@property (nonatomic, assign) BOOL       isSystemApplication;
@property (nonatomic, assign) BOOL       isHidden;
@property (nonatomic, assign) NSInteger  badgeCount;

/* Designated initializer — loads Info.plist from bundlePath */
- (id)initWithBundlePath:(NSString *)bundlePath;

/* Convenience factory */
+ (SBApplication *)applicationWithBundlePath:(NSString *)bundlePath;

/* Attempt to launch via SpringBoard's UIApplicationLaunchingDelegate */
- (void)launch;

/* Load icon images from bundle */
- (void)loadIconImages;

/* Returns the localised display name from CFBundleDisplayName / CFBundleName */
- (NSString *)localizedDisplayName;

@end