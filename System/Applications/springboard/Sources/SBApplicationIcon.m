/*
 * SBApplicationIcon.m
 * SpringBoard - PocketDarwin
 *
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import "SBApplicationIcon.h"
#import "SBApplication.h"

const CGFloat kSBIconCellWidth    = 76.0f;
const CGFloat kSBIconCellHeight   = 88.0f;
const CGFloat kSBIconImageSize    = 57.0f;
const CGFloat kSBIconCornerRadius = 11.5f;

/* Wiggle animation key & parameters */
static NSString *const kWiggleAnimationKey = @"sbWiggle";
static const CFTimeInterval kWiggleSpeed   = 0.13;
static const CGFloat        kWiggleAngle   = 0.05f; /* ~3 degrees */

@implementation SBApplicationIcon

@synthesize application = _application;
@synthesize isWiggling  = _isWiggling;

#pragma mark - Init

- (id)initWithApplication:(SBApplication *)application
{
    CGRect frame = CGRectMake(0.0f, 0.0f, kSBIconCellWidth, kSBIconCellHeight);
    if ((self = [super initWithFrame:frame])) {
        self.backgroundColor = [UIColor clearColor];
        self.application = application;
        [self _buildSubviews];
        [self reloadBadge];
    }
    return self;
}

- (void)_buildSubviews
{
    CGFloat iconX = (kSBIconCellWidth  - kSBIconImageSize) / 2.0f;
    CGFloat iconY = 3.0f;

    /* --- Icon image --- */
    _iconImageView = [[UIImageView alloc] initWithFrame:
                      CGRectMake(iconX, iconY, kSBIconImageSize, kSBIconImageSize)];
    _iconImageView.image = _application.icon;

    /* Rounded corners via CALayer */
    _iconImageView.layer.cornerRadius  = kSBIconCornerRadius;
    _iconImageView.layer.masksToBounds = YES;

    /* Subtle drop shadow on the icon */
    _iconImageView.layer.shadowColor   = [[UIColor blackColor] CGColor];
    _iconImageView.layer.shadowOpacity = 0.45f;
    _iconImageView.layer.shadowOffset  = CGSizeMake(0.0f, 2.0f);
    _iconImageView.layer.shadowRadius  = 3.0f;

    [self addSubview:_iconImageView];

    /* --- Glass shine overlay (top ~40% of icon, white→clear gradient) --- */
    _glassOverlay = [[UIImageView alloc] initWithFrame:
                     CGRectMake(iconX, iconY, kSBIconImageSize, kSBIconImageSize * 0.55f)];
    _glassOverlay.image = [self _buildGlossImage];
    _glassOverlay.layer.cornerRadius  = kSBIconCornerRadius;
    _glassOverlay.layer.masksToBounds = YES;
    [self addSubview:_glassOverlay];

    /* --- Name label --- */
    CGFloat labelY = iconY + kSBIconImageSize + 2.0f;
    _nameLabel = [[UILabel alloc] initWithFrame:
                  CGRectMake(0.0f, labelY, kSBIconCellWidth, 18.0f)];
    _nameLabel.text            = _application.displayName;
    _nameLabel.font            = [UIFont boldSystemFontOfSize:11.0f];
    _nameLabel.textColor       = [UIColor whiteColor];
    _nameLabel.textAlignment   = UITextAlignmentCenter;
    _nameLabel.backgroundColor = [UIColor clearColor];
    _nameLabel.shadowColor     = [UIColor colorWithWhite:0.0f alpha:0.7f];
    _nameLabel.shadowOffset    = CGSizeMake(0.0f, 1.0f);
    _nameLabel.lineBreakMode   = UILineBreakModeTailTruncation;
    [self addSubview:_nameLabel];

    /* --- Badge --- */
    _badgeView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 20, 20)];
    _badgeView.backgroundColor    = [UIColor redColor];
    _badgeView.layer.cornerRadius = 10.0f;
    _badgeView.layer.masksToBounds = YES;
    _badgeView.hidden             = YES;

    /* Position badge at top-right of icon */
    CGFloat badgeX = iconX + kSBIconImageSize - 10.0f;
    CGFloat badgeY = iconY - 10.0f;
    _badgeView.frame = CGRectMake(badgeX, badgeY, 20.0f, 20.0f);

    _badgeLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 20, 20)];
    _badgeLabel.font            = [UIFont boldSystemFontOfSize:10.0f];
    _badgeLabel.textColor       = [UIColor whiteColor];
    _badgeLabel.textAlignment   = UITextAlignmentCenter;
    _badgeLabel.backgroundColor = [UIColor clearColor];
    [_badgeView addSubview:_badgeLabel];
    [self addSubview:_badgeView];
}

- (UIImage *)_buildGlossImage
{
    /*
     * Draw the classic iPhone icon gloss:
     * a semi-ellipse filled with a white→transparent gradient
     * covering roughly the top 55% of the icon.
     */
    CGSize size = CGSizeMake(kSBIconImageSize, kSBIconImageSize * 0.55f);
    UIGraphicsBeginImageContext(size);
    CGContextRef ctx = UIGraphicsGetCurrentContext();

    CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
    CGFloat components[8] = {
        1.0f, 1.0f, 1.0f, 0.55f,   /* top: semi-opaque white */
        1.0f, 1.0f, 1.0f, 0.05f    /* bottom: nearly transparent */
    };
    CGGradientRef gradient = CGGradientCreateWithColorComponents(space, components, NULL, 2);

    /* Elliptical clip */
    CGContextBeginPath(ctx);
    CGContextAddEllipseInRect(ctx, CGRectMake(-4.0f, -4.0f,
                                             size.width + 8.0f,
                                             size.height * 1.6f));
    CGContextClip(ctx);

    CGContextDrawLinearGradient(ctx, gradient,
                                CGPointMake(0, 0),
                                CGPointMake(0, size.height),
                                0);

    CGGradientRelease(gradient);
    CGColorSpaceRelease(space);

    UIImage *result = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return result;
}

#pragma mark - Badge

- (void)reloadBadge
{
    NSInteger count = _application.badgeCount;
    if (count > 0) {
        _badgeLabel.text = [NSString stringWithFormat:@"%d", (int)count];

        /* Widen badge pill for numbers > 9 */
        CGFloat w = (count > 9) ? 28.0f : 20.0f;
        CGFloat iconX = (kSBIconCellWidth - kSBIconImageSize) / 2.0f;
        _badgeView.frame = CGRectMake(iconX + kSBIconImageSize - w + 6.0f,
                                      3.0f - 10.0f,
                                      w, 20.0f);
        _badgeView.layer.cornerRadius = 10.0f;
        _badgeView.hidden = NO;
    } else {
        _badgeView.hidden = YES;
    }
}

#pragma mark - Wiggle (edit mode)

- (void)startWiggle
{
    if (_isWiggling) return;
    _isWiggling = YES;

    CAKeyframeAnimation *rotation = [CAKeyframeAnimation animationWithKeyPath:@"transform.rotation.z"];
    rotation.values = [NSArray arrayWithObjects:
                       [NSNumber numberWithFloat: kWiggleAngle],
                       [NSNumber numberWithFloat:-kWiggleAngle],
                       [NSNumber numberWithFloat: kWiggleAngle],
                       nil];
    rotation.duration       = kWiggleSpeed * 2.0;
    rotation.repeatCount    = HUGE_VALF;
    rotation.autoreverses   = NO;
    rotation.calculationMode = kCAAnimationLinear;

    CAKeyframeAnimation *translation = [CAKeyframeAnimation animationWithKeyPath:@"transform.translation.y"];
    translation.values = [NSArray arrayWithObjects:
                          [NSNumber numberWithFloat:-0.8f],
                          [NSNumber numberWithFloat: 0.8f],
                          [NSNumber numberWithFloat:-0.8f],
                          nil];
    translation.duration     = kWiggleSpeed * 2.0;
    translation.repeatCount  = HUGE_VALF;
    translation.calculationMode = kCAAnimationLinear;

    CAAnimationGroup *group = [CAAnimationGroup animation];
    group.animations  = [NSArray arrayWithObjects:rotation, translation, nil];
    group.duration    = kWiggleSpeed * 2.0;
    group.repeatCount = HUGE_VALF;

    [self.layer addAnimation:group forKey:kWiggleAnimationKey];
}

- (void)stopWiggle
{
    if (!_isWiggling) return;
    _isWiggling = NO;
    [self.layer removeAnimationForKey:kWiggleAnimationKey];
    /* Snap transform back to identity */
    self.layer.transform = CATransform3DIdentity;
}

#pragma mark - Highlight (tap state)

- (void)setHighlighted:(BOOL)highlighted
{
    _isHighlighted = highlighted;
    _iconImageView.alpha = highlighted ? 0.65f : 1.0f;
}

#pragma mark - Touch handling

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self setHighlighted:YES];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self setHighlighted:NO];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self setHighlighted:NO];

    if (!_isWiggling) {
        [_application launch];
    }
}

#pragma mark - Memory management

- (void)dealloc
{
    [_application    release];
    [_iconImageView  release];
    [_glassOverlay   release];
    [_nameLabel      release];
    [_badgeLabel     release];
    [_badgeView      release];
    [super dealloc];
}

@end