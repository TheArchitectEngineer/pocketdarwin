/*
 * SBStatusBar.m
 * SpringBoard - PocketDarwin
 *
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import <UIKit/UIKit.h>
#import "SBStatusBar.h"

const CGFloat kSBStatusBarHeight = 20.0f;

@implementation SBStatusBar

@synthesize signalStrength   = _signalStrength;
@synthesize batteryLevel     = _batteryLevel;
@synthesize batteryCharging  = _batteryCharging;
@synthesize wifiConnected    = _wifiConnected;
@synthesize carrierName      = _carrierName;
@synthesize style            = _style;

#pragma mark - Init

- (id)initWithFrame:(CGRect)frame style:(SBStatusBarStyle)style
{
    if ((self = [super initWithFrame:frame])) {
        _style           = style;
        _signalStrength  = 5;
        _batteryLevel    = 1.0f;
        _batteryCharging = NO;
        _wifiConnected   = YES;
        _carrierName     = [@"PocketDarwin" copy];

        self.backgroundColor = [UIColor clearColor];
        self.clipsToBounds   = YES;

        [self _buildSubviews];
        [self _startClock];
        [self refresh];
    }
    return self;
}

- (void)_buildSubviews
{
    UIColor *textColor = (_style == SBStatusBarStyleDefault)
        ? [UIColor blackColor]
        : [UIColor whiteColor];

    /* --- Carrier label (left) --- */
    _carrierLabel = [[UILabel alloc] initWithFrame:CGRectMake(4.0f, 0, 80.0f, 20.0f)];
    _carrierLabel.font            = [UIFont boldSystemFontOfSize:11.0f];
    _carrierLabel.textColor       = textColor;
    _carrierLabel.backgroundColor = [UIColor clearColor];
    _carrierLabel.text            = _carrierName;
    [self addSubview:_carrierLabel];

    /* --- Clock label (center) --- */
    _timeLabel = [[UILabel alloc] initWithFrame:CGRectMake(90.0f, 0.0f, 140.0f, 20.0f)];
    _timeLabel.font            = [UIFont boldSystemFontOfSize:12.0f];
    _timeLabel.textColor       = textColor;
    _timeLabel.backgroundColor = [UIColor clearColor];
    _timeLabel.textAlignment   = UITextAlignmentCenter;
    [self addSubview:_timeLabel];

    /* --- Right-side custom UIView for signal + wifi + battery --- */
    /* Signal bars */
    _signalView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 22.0f, 14.0f)];
    _signalView.image = [self _drawSignalBarsImage:_signalStrength color:textColor];
    [self addSubview:_signalView];

    /* WiFi */
    _wifiView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 15.0f, 12.0f)];
    _wifiView.image = [self _drawWiFiImage:_wifiConnected color:textColor];
    [self addSubview:_wifiView];

    /* Battery */
    _batteryView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 25.0f, 12.0f)];
    _batteryView.image = [self _drawBatteryImage:_batteryLevel charging:_batteryCharging color:textColor];
    [self addSubview:_batteryView];

    [self _layoutRightItems];
}

- (void)_layoutRightItems
{
    CGFloat totalW = CGRectGetWidth(self.bounds);
    CGFloat rightMargin = 4.0f;

    /* Battery — rightmost */
    CGFloat battX = totalW - rightMargin - 25.0f;
    _batteryView.frame = CGRectMake(battX, 4.0f, 25.0f, 12.0f);

    /* WiFi — left of battery */
    CGFloat wifiX = battX - 2.0f - 15.0f;
    _wifiView.frame = CGRectMake(wifiX, 4.0f, 15.0f, 12.0f);
    _wifiView.hidden = !_wifiConnected;

    /* Signal — left of wifi (or battery if no wifi) */
    CGFloat sigX = _wifiConnected ? (wifiX - 2.0f - 22.0f) : (battX - 2.0f - 22.0f);
    _signalView.frame = CGRectMake(sigX, 3.0f, 22.0f, 14.0f);
}

#pragma mark - Clock

- (void)_startClock
{
    /* Fire every 30 seconds; the exact second alignment isn't needed */
    _clockTimer = [[NSTimer scheduledTimerWithTimeInterval:30.0
                                                    target:self
                                                  selector:@selector(_updateTime)
                                                  userInfo:nil
                                                   repeats:YES] retain];
    [self _updateTime];
}

- (void)_updateTime
{
    NSDate *now           = [NSDate date];
    NSCalendar *cal       = [NSCalendar currentCalendar];
    NSDateComponents *comps = [cal components:(NSHourCalendarUnit | NSMinuteCalendarUnit)
                                     fromDate:now];
    NSInteger hour   = [comps hour];
    NSInteger minute = [comps minute];

    /* iPhoneOS 2.0 uses 12-hour by default in en_US locale */
    NSString *ampm  = (hour >= 12) ? @"PM" : @"AM";
    NSInteger hour12 = hour % 12;
    if (hour12 == 0) hour12 = 12;

    _timeLabel.text = [NSString stringWithFormat:@"%d:%02d %@", (int)hour12, (int)minute, ampm];
}

#pragma mark - Drawing helpers

- (UIImage *)_drawSignalBarsImage:(NSInteger)strength color:(UIColor *)color
{
    /* 5 bars, each 3px wide, separated by 1px gap; tallest is 12px */
    CGSize size = CGSizeMake(22.0f, 14.0f);
    UIGraphicsBeginImageContext(size);
    CGContextRef ctx = UIGraphicsGetCurrentContext();

    CGContextSetFillColorWithColor(ctx, [color CGColor]);

    CGFloat barW   = 3.0f;
    CGFloat gap    = 1.5f;
    CGFloat maxH   = 12.0f;
    CGFloat baseY  = 13.0f;

    for (NSInteger i = 0; i < 5; i++) {
        CGFloat h = (i + 1) * (maxH / 5.0f);
        CGFloat x = i * (barW + gap);
        CGFloat y = baseY - h;

        CGRect barRect = CGRectMake(x, y, barW, h);

        if (i < strength) {
            CGContextFillRect(ctx, barRect);
        } else {
            /* Empty bar: just outline */
            CGContextSetStrokeColorWithColor(ctx, [color CGColor]);
            CGContextSetLineWidth(ctx, 1.0f);
            CGContextSetAlpha(ctx, 0.4f);
            CGContextStrokeRect(ctx, CGRectInset(barRect, 0.5f, 0.5f));
            CGContextSetAlpha(ctx, 1.0f);
        }
    }

    UIImage *img = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return img;
}

- (UIImage *)_drawWiFiImage:(BOOL)connected color:(UIColor *)color
{
    CGSize size = CGSizeMake(15.0f, 12.0f);
    UIGraphicsBeginImageContext(size);
    CGContextRef ctx = UIGraphicsGetCurrentContext();

    CGContextSetStrokeColorWithColor(ctx, [color CGColor]);
    CGContextSetLineWidth(ctx, 1.5f);
    CGContextSetLineCap(ctx, kCGLineCapRound);

    if (!connected) {
        CGContextSetAlpha(ctx, 0.3f);
    }

    /* Draw 3 concentric arcs and a dot */
    CGFloat cx = 7.5f, cy = 11.5f;
    CGFloat radii[3] = { 10.0f, 7.0f, 4.0f };
    CGFloat startAngle = (CGFloat)M_PI + 0.4f;
    CGFloat endAngle   = -(CGFloat)0.4f;

    for (NSInteger i = 0; i < 3; i++) {
        CGContextBeginPath(ctx);
        CGContextAddArc(ctx, cx, cy, radii[i], startAngle, endAngle, 0);
        CGContextStrokePath(ctx);
    }

    /* Dot at base */
    CGContextFillEllipseInRect(ctx, CGRectMake(cx - 1.0f, cy - 1.0f, 2.0f, 2.0f));

    UIImage *img = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return img;
}

- (UIImage *)_drawBatteryImage:(CGFloat)level charging:(BOOL)charging color:(UIColor *)color
{
    CGSize size = CGSizeMake(25.0f, 12.0f);
    UIGraphicsBeginImageContext(size);
    CGContextRef ctx = UIGraphicsGetCurrentContext();

    /* Outer shell */
    CGContextSetStrokeColorWithColor(ctx, [color CGColor]);
    CGContextSetLineWidth(ctx, 1.0f);
    CGContextStrokeRect(ctx, CGRectMake(0.5f, 1.5f, 20.0f, 9.0f));

    /* Tip (positive terminal) */
    CGContextSetFillColorWithColor(ctx, [color CGColor]);
    CGContextFillRect(ctx, CGRectMake(21.0f, 4.0f, 3.0f, 4.0f));

    /* Fill */
    CGFloat fillW = (CGFloat)(level * 18.0f);
    UIColor *fillColor = (level < 0.2f)
        ? [UIColor colorWithRed:1.0f green:0.2f blue:0.2f alpha:1.0f]
        : color;
    CGContextSetFillColorWithColor(ctx, [fillColor CGColor]);
    CGContextFillRect(ctx, CGRectMake(1.5f, 2.5f, fillW, 7.0f));

    /* Charging bolt */
    if (charging) {
        CGContextSetFillColorWithColor(ctx, [[UIColor yellowColor] CGColor]);
        CGPoint bolt[6] = {
            CGPointMake(12.0f, 2.5f),
            CGPointMake(9.0f,  7.0f),
            CGPointMake(11.0f, 7.0f),
            CGPointMake(8.0f,  11.5f),
            CGPointMake(12.5f, 6.5f),
            CGPointMake(10.5f, 6.5f)
        };
        CGContextBeginPath(ctx);
        CGContextMoveToPoint(ctx, bolt[0].x, bolt[0].y);
        for (NSInteger i = 1; i < 6; i++) {
            CGContextAddLineToPoint(ctx, bolt[i].x, bolt[i].y);
        }
        CGContextClosePath(ctx);
        CGContextFillPath(ctx);
    }

    UIImage *img = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return img;
}

#pragma mark - Refresh

- (void)refresh
{
    UIColor *textColor = (_style == SBStatusBarStyleDefault)
        ? [UIColor blackColor]
        : [UIColor whiteColor];

    _carrierLabel.text  = _carrierName;
    _carrierLabel.textColor = textColor;
    _timeLabel.textColor    = textColor;

    _signalView.image  = [self _drawSignalBarsImage:_signalStrength color:textColor];
    _wifiView.image    = [self _drawWiFiImage:_wifiConnected color:textColor];
    _batteryView.image = [self _drawBatteryImage:_batteryLevel charging:_batteryCharging color:textColor];

    _wifiView.hidden = !_wifiConnected;

    [self _layoutRightItems];
    [self _updateTime];
}

#pragma mark - Property setters with auto-refresh

- (void)setSignalStrength:(NSInteger)v  { _signalStrength = v; [self refresh]; }
- (void)setBatteryLevel:(CGFloat)v      { _batteryLevel = v;   [self refresh]; }
- (void)setBatteryCharging:(BOOL)v      { _batteryCharging = v;[self refresh]; }
- (void)setWifiConnected:(BOOL)v        { _wifiConnected = v;  [self refresh]; }
- (void)setCarrierName:(NSString *)v    {
    [_carrierName release];
    _carrierName = [v copy];
    [self refresh];
}

#pragma mark - Dealloc

- (void)dealloc
{
    [_clockTimer invalidate];
    [_clockTimer     release];
    [_carrierLabel   release];
    [_timeLabel      release];
    [_signalView     release];
    [_wifiView       release];
    [_batteryView    release];
    [_carrierName    release];
    [super dealloc];
}

@end