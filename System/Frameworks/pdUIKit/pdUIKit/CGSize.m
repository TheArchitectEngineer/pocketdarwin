//
//  CGSize.m
//  pdUIKit - PocketDarwin UIKit Framework
//

#import "CGSize.h"

// UIKit-specific CGSize function implementations
NSString *NSStringFromCGSize(CGSize size) {
    return [NSString stringWithFormat:@"{%.1f, %.1f}", size.width, size.height];
}

// NSValue category implementation
@implementation NSValue (NSValueCGSizeExtensions)

+ (NSValue *)valueWithCGSize:(CGSize)size {
    return [NSValue valueWithBytes:&size objCType:@encode(CGSize)];
}

- (CGSize)CGSizeValue {
    CGSize size;
    [self getValue:&size];
    return size;
}

@end
