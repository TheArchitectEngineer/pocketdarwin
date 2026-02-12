//
//  CGPoint.h
//  pdUIKit - PocketDarwin UIKit Framework
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

// CGPoint is already defined in AppKit, so we just add UIKit-specific functions
NS_ASSUME_NONNULL_BEGIN

// UIKit-specific CGPoint functions
NSString *NSStringFromCGPoint(CGPoint point);

// Extension for NSValue (if not already available)
#if !defined(NSValue_UIKIT_POINT_EXTENSIONS)
#define NSValue_UIKIT_POINT_EXTENSIONS

@interface NSValue (NSValueCGPointExtensions)

+ (NSValue *)valueWithCGPoint:(CGPoint)point;
- (CGPoint)CGPointValue;

@end

#endif

NS_ASSUME_NONNULL_END
