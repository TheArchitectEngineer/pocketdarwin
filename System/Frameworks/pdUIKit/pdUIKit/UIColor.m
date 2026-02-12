//
//  UIColor.m
//  pdUIKit - PocketDarwin UIKit Framework
//

#import "UIColor.h"
#import "UIImage.h"

// Forward declaration for UIImage
@interface UIImage : NSObject
@end

@implementation UIColor {
    NSColor *_nsColor;
}

#pragma mark - Convenience Constructors

+ (UIColor *)colorWithRed:(CGFloat)red green:(CGFloat)green blue:(CGFloat)blue alpha:(CGFloat)alpha {
    return [[self alloc] initWithRed:red green:green blue:blue alpha:alpha];
}

+ (UIColor *)colorWithWhite:(CGFloat)white alpha:(CGFloat)alpha {
    return [[self alloc] initWithWhite:white alpha:alpha];
}

+ (UIColor *)colorWithHue:(CGFloat)hue saturation:(CGFloat)saturation brightness:(CGFloat)brightness alpha:(CGFloat)alpha {
    return [[self alloc] initWithHue:hue saturation:saturation brightness:brightness alpha:alpha];
}

+ (UIColor *)blackColor {
    return [self colorWithWhite:0.0 alpha:1.0];
}

+ (UIColor *)darkGrayColor {
    return [self colorWithWhite:0.333 alpha:1.0];
}

+ (UIColor *)lightGrayColor {
    return [self colorWithWhite:0.667 alpha:1.0];
}

+ (UIColor *)whiteColor {
    return [self colorWithWhite:1.0 alpha:1.0];
}

+ (UIColor *)grayColor {
    return [self colorWithWhite:0.5 alpha:1.0];
}

+ (UIColor *)redColor {
    return [self colorWithRed:1.0 green:0.0 blue:0.0 alpha:1.0];
}

+ (UIColor *)greenColor {
    return [self colorWithRed:0.0 green:1.0 blue:0.0 alpha:1.0];
}

+ (UIColor *)blueColor {
    return [self colorWithRed:0.0 green:0.0 blue:1.0 alpha:1.0];
}

+ (UIColor *)cyanColor {
    return [self colorWithRed:0.0 green:1.0 blue:1.0 alpha:1.0];
}

+ (UIColor *)yellowColor {
    return [self colorWithRed:1.0 green:1.0 blue:0.0 alpha:1.0];
}

+ (UIColor *)magentaColor {
    return [self colorWithRed:1.0 green:0.0 blue:1.0 alpha:1.0];
}

+ (UIColor *)orangeColor {
    return [self colorWithRed:1.0 green:0.5 blue:0.0 alpha:1.0];
}

+ (UIColor *)purpleColor {
    return [self colorWithRed:0.5 green:0.0 blue:0.5 alpha:1.0];
}

+ (UIColor *)brownColor {
    return [self colorWithRed:0.6 green:0.4 blue:0.2 alpha:1.0];
}

+ (UIColor *)clearColor {
    return [self colorWithWhite:0.0 alpha:0.0];
}

+ (UIColor *)colorWithPatternImage:(UIImage *)patternImage {
    // TODO: Implement pattern colors
    return [self colorWithWhite:0.0 alpha:1.0];
}

#pragma mark - Initialization

- (instancetype)initWithRed:(CGFloat)red green:(CGFloat)green blue:(CGFloat)blue alpha:(CGFloat)alpha {
    if (self = [super init]) {
        _nsColor = [NSColor colorWithCalibratedRed:red green:green blue:blue alpha:alpha];
    }
    return self;
}

- (instancetype)initWithWhite:(CGFloat)white alpha:(CGFloat)alpha {
    if (self = [super init]) {
        _nsColor = [NSColor colorWithCalibratedWhite:white alpha:alpha];
    }
    return self;
}

- (instancetype)initWithHue:(CGFloat)hue saturation:(CGFloat)saturation brightness:(CGFloat)brightness alpha:(CGFloat)alpha {
    if (self = [super init]) {
        _nsColor = [NSColor colorWithCalibratedHue:hue saturation:saturation brightness:brightness alpha:alpha];
    }
    return self;
}

- (instancetype)initWithCGColor:(CGColorRef)cgColor {
    if (self = [super init]) {
        _nsColor = [NSColor colorWithCGColor:cgColor];
    }
    return self;
}

#pragma mark - Getting Color Components

- (void)getRed:(nullable CGFloat *)red green:(nullable CGFloat *)green blue:(nullable CGFloat *)blue alpha:(nullable CGFloat *)alpha {
    NSColor *rgbColor = [_nsColor colorUsingColorSpace:[NSColorSpace genericRGBColorSpace]];
    if (rgbColor) {
        [rgbColor getRed:red green:green blue:blue alpha:alpha];
    }
}

- (void)getWhite:(nullable CGFloat *)white alpha:(nullable CGFloat *)alpha {
    NSColor *grayColor = [_nsColor colorUsingColorSpace:[NSColorSpace genericGrayColorSpace]];
    if (grayColor) {
        [grayColor getWhite:white alpha:alpha];
    }
}

- (void)getHue:(nullable CGFloat *)hue saturation:(nullable CGFloat *)saturation brightness:(nullable CGFloat *)brightness alpha:(nullable CGFloat *)alpha {
    NSColor *hsbColor = [_nsColor colorUsingColorSpace:[NSColorSpace genericRGBColorSpace]];
    if (hsbColor) {
        CGFloat r, g, b, a;
        [hsbColor getRed:&r green:&g blue:&b alpha:&a];
        
        // Convert RGB to HSB (simplified)
        CGFloat maxVal = MAX(r, MAX(g, b));
        CGFloat minVal = MIN(r, MIN(g, b));
        CGFloat delta = maxVal - minVal;
        
        if (delta == 0) {
            *hue = 0;
        } else if (maxVal == r) {
            *hue = 60 * ((g - b) / delta);
        } else if (maxVal == g) {
            *hue = 60 * ((b - r) / delta) + 120;
        } else {
            *hue = 60 * ((r - g) / delta) + 240;
        }
        
        *saturation = maxVal == 0 ? 0 : delta / maxVal;
        *brightness = maxVal;
        *alpha = a;
    }
}

#pragma mark - Properties

- (CGFloat)alphaComponent {
    return [_nsColor alphaComponent];
}

- (CGColorRef)CGColor {
    return [_nsColor CGColor];
}

#pragma mark - GNUStep Compatibility

- (NSColor *)NSColorValue {
    return _nsColor;
}

@end
