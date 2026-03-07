#import <UIKit/UIKit.h>

@class FDStartup;

@interface AppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *_window;
    UINavigationController *_navigationController;
    NSArray *_startupBundles;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) UINavigationController *navigationController;
@property (nonatomic, retain) NSArray *startupBundles;

- (void)launchStartupBundle:(FDStartup *)bundle;

@end
