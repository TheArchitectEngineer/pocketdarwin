/*
 * SBDock.h
 * SpringBoard - PocketDarwin
 *
 * The dock at the bottom of the home screen.
 * iPhoneOS 2.0: holds exactly 4 icons, always visible.
 * Rendered with the frosted glass / shelf background.
 *
 * iPhoneOS 2.0 compatible.
 */

#import <UIKit/UIKit.h>

@class SBApplication;

extern const NSInteger kSBDockMaxIcons;  /* 4 */
extern const CGFloat   kSBDockHeight;   /* 90 */

@interface SBDock : UIView {
    NSMutableArray  *_dockIcons;
    UIImageView     *_backgroundView;
}

- (id)initWithFrame:(CGRect)frame;

/* Set dock applications (max 4) */
- (void)setApplications:(NSArray *)applications;

/* Layout icon frames inside dock */
- (void)layoutDockIcons;

@end