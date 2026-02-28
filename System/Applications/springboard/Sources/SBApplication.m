/*
 * SBApplication.m
 * SpringBoard - PocketDarwin
 *
 * iPhoneOS 2.0 — Manual Retain/Release (no ARC)
 */

#import <UIKit/UIKit.h>
#import "SBApplication.h"

@implementation SBApplication

@synthesize displayName        = _displayName;
@synthesize bundleIdentifier   = _bundleIdentifier;
@synthesize bundlePath         = _bundlePath;
@synthesize executablePath     = _executablePath;
@synthesize icon               = _icon;
@synthesize smallIcon          = _smallIcon;
@synthesize isSystemApplication = _isSystemApplication;
@synthesize isHidden           = _isHidden;
@synthesize badgeCount         = _badgeCount;

- (id)init
{
    if ((self = [super init])) {
        _displayName         = nil;
        _bundleIdentifier    = nil;
        _bundlePath          = nil;
        _executablePath      = nil;
        _icon                = nil;
        _smallIcon           = nil;
        _isSystemApplication = NO;
        _isHidden            = NO;
        _badgeCount          = 0;
    }
    return self;
}

- (id)initWithBundlePath:(NSString *)bundlePath
{
    if ((self = [self init])) {
        self.bundlePath = bundlePath;
        [self _loadInfoPlist];
        [self loadIconImages];
    }
    return self;
}

+ (SBApplication *)applicationWithBundlePath:(NSString *)bundlePath
{
    return [[[SBApplication alloc] initWithBundlePath:bundlePath] autorelease];
}

- (void)_loadInfoPlist
{
    NSString *plistPath = [_bundlePath stringByAppendingPathComponent:@"Info.plist"];
    NSDictionary *info  = [NSDictionary dictionaryWithContentsOfFile:plistPath];

    if (!info) {
        NSLog(@"SBApplication: Failed to load Info.plist at %@", plistPath);
        return;
    }

    /* CFBundleDisplayName preferred, fall back to CFBundleName */
    NSString *displayName = [info objectForKey:@"CFBundleDisplayName"];
    if (!displayName) {
        displayName = [info objectForKey:@"CFBundleName"];
    }
    self.displayName = displayName ? displayName : @"Unknown";

    self.bundleIdentifier = [info objectForKey:@"CFBundleIdentifier"];

    /* Build executable path */
    NSString *execName = [info objectForKey:@"CFBundleExecutable"];
    if (execName) {
        self.executablePath = [_bundlePath stringByAppendingPathComponent:execName];
    }
}

- (void)loadIconImages
{
    /* iPhoneOS 2.0 icon naming: Icon.png (57×57) and Icon-Small.png (29×29) */
    NSString *iconPath  = [_bundlePath stringByAppendingPathComponent:@"Icon.png"];
    NSString *smallPath = [_bundlePath stringByAppendingPathComponent:@"Icon-Small.png"];

    UIImage *icon = [UIImage imageWithContentsOfFile:iconPath];
    if (icon) {
        self.icon = icon;
    } else {
        /* Fall back to a generated placeholder */
        self.icon = [self _generatePlaceholderIcon];
    }

    UIImage *small = [UIImage imageWithContentsOfFile:smallPath];
    if (small) {
        self.smallIcon = small;
    } else {
        self.smallIcon = self.icon;
    }
}

- (UIImage *)_generatePlaceholderIcon
{
    /* Draw a rounded-rect placeholder with the app initial */
    CGSize size = CGSizeMake(57.0f, 57.0f);
    UIGraphicsBeginImageContext(size);

    CGContextRef ctx = UIGraphicsGetCurrentContext();

    /* Background gradient */
    CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
    CGFloat comps[8] = { 0.3f, 0.5f, 0.9f, 1.0f,
                         0.1f, 0.3f, 0.7f, 1.0f };
    CGGradientRef gradient = CGGradientCreateWithColorComponents(space, comps, NULL, 2);

    CGContextSaveGState(ctx);
    /* Clip to rounded rect */
    CGRect rect = CGRectMake(0, 0, 57, 57);
    [self _clipContextToRoundedRect:rect radius:10.0f context:ctx];
    CGContextDrawLinearGradient(ctx, gradient,
                                CGPointMake(0, 0), CGPointMake(0, 57),
                                0);
    CGContextRestoreGState(ctx);

    CGGradientRelease(gradient);
    CGColorSpaceRelease(space);

    UIImage *result = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return result;
}

- (void)_clipContextToRoundedRect:(CGRect)rect
                            radius:(CGFloat)radius
                           context:(CGContextRef)ctx
{
    CGFloat minX = CGRectGetMinX(rect);
    CGFloat minY = CGRectGetMinY(rect);
    CGFloat maxX = CGRectGetMaxX(rect);
    CGFloat maxY = CGRectGetMaxY(rect);

    CGContextBeginPath(ctx);
    CGContextMoveToPoint(ctx, minX + radius, minY);
    CGContextAddArcToPoint(ctx, maxX, minY, maxX, minY + radius, radius);
    CGContextAddArcToPoint(ctx, maxX, maxY, maxX - radius, maxY, radius);
    CGContextAddArcToPoint(ctx, minX, maxY, minX, maxY - radius, radius);
    CGContextAddArcToPoint(ctx, minX, minY, minX + radius, minY, radius);
    CGContextClosePath(ctx);
    CGContextClip(ctx);
}

- (NSString *)localizedDisplayName
{
    return _displayName ? _displayName : @"Unknown";
}

- (void)launch
{
    /*
     * On real iPhoneOS 2.0, SpringBoard uses a private IPC mechanism
     * (mach ports / uikit-app-launch) to exec the application binary.
     * On PocketDarwin we use posix_spawn or execve.
     */
    if (!_executablePath) {
        NSLog(@"SBApplication: Cannot launch — no executable path for %@", _bundleIdentifier);
        return;
    }

    NSLog(@"SBApplication: Launching %@ (%@)", _displayName, _executablePath);

    /* Set HOME and DYLD_ROOT_PATH for PocketDarwin */
    NSArray *args = [NSArray arrayWithObject:_executablePath];
    NSLog(@"SBApplication: Would exec: %@", args);

    /*
     * On a real device with PocketDarwin, replace the block below with:
     *
     *   const char *argv[] = { [_executablePath UTF8String], NULL };
     *   execve([_executablePath UTF8String], (char *const *)argv, environ);
     *
     * SpringBoard itself does not return after a successful launch —
     * the kernel re-parents the new process and SpringBoard waits via
     * a run-loop watchdog.
     */
}

- (void)dealloc
{
    [_displayName      release];
    [_bundleIdentifier release];
    [_bundlePath       release];
    [_executablePath   release];
    [_icon             release];
    [_smallIcon        release];
    [super dealloc];
}

@end