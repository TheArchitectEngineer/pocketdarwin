/*
 * SBStatusBar.h
 * SpringBoard - PocketDarwin
 */

#import <UIKit/UIKit.h>

typedef enum {
    SBStatusBarStyleDefault = 0,
    SBStatusBarStyleBlack   = 1
} SBStatusBarStyle;

extern const CGFloat kSBStatusBarHeight;

@interface SBStatusBar : UIView {
    NSInteger        _signalStrength;
    CGFloat          _batteryLevel;
    BOOL             _batteryCharging;
    BOOL             _wifiConnected;
    NSString        *_carrierName;
    SBStatusBarStyle _style;

    UILabel         *_carrierLabel;
    UILabel         *_timeLabel;
    UIImageView     *_signalView;
    UIImageView     *_wifiView;
    UIImageView     *_batteryView;
    NSTimer         *_clockTimer;
}

@property (nonatomic, assign) NSInteger        signalStrength;
@property (nonatomic, assign) CGFloat          batteryLevel;
@property (nonatomic, assign) BOOL             batteryCharging;
@property (nonatomic, assign) BOOL             wifiConnected;
@property (nonatomic, copy)   NSString        *carrierName;
@property (nonatomic, assign) SBStatusBarStyle style;

- (id)initWithFrame:(CGRect)frame style:(SBStatusBarStyle)style;
- (void)refresh;

@end
