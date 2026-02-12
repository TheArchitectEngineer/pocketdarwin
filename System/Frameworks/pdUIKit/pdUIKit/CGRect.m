//
//  CGRect.m
//  pdUIKit - PocketDarwin UIKit Framework
//

#import "CGRect.h"

// UIKit-specific CGRect function implementations
NSString *NSStringFromCGRect(CGRect rect) {
    return [NSString stringWithFormat:@"{{%.1f, %.1f}, {%.1f, %.1f}}", 
            rect.origin.x, rect.origin.y, rect.size.width, rect.size.height];
}

// NSValue category implementation
@implementation NSValue (NSValueCGRectExtensions)

+ (NSValue *)valueWithCGRect:(CGRect)rect {
    return [NSValue valueWithBytes:&rect objCType:@encode(CGRect)];
}

- (CGRect)CGRectValue {
    CGRect rect;
    [self getValue:&rect];
    return rect;
}

@end
