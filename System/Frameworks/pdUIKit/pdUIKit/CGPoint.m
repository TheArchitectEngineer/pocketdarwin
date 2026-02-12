//
//  CGPoint.m
//  pdUIKit - PocketDarwin UIKit Framework
//

#import "CGPoint.h"

// UIKit-specific CGPoint function implementations
NSString *NSStringFromCGPoint(CGPoint point) {
    return [NSString stringWithFormat:@"{%.1f, %.1f}", point.x, point.y];
}

// NSValue category implementation
@implementation NSValue (NSValueCGPointExtensions)

+ (NSValue *)valueWithCGPoint:(CGPoint)point {
    return [NSValue valueWithBytes:&point objCType:@encode(CGPoint)];
}

- (CGPoint)CGPointValue {
    CGPoint point;
    [self getValue:&point];
    return point;
}

@end
