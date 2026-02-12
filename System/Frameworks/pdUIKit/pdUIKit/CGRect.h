//
//  CGRect.h
//  pdUIKit - PocketDarwin UIKit Framework
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

// CGRect is already defined in AppKit, so we just add UIKit-specific functions
NS_ASSUME_NONNULL_BEGIN

// UIKit-specific CGRect functions
NSString *NSStringFromCGRect(CGRect rect);

// Extension for NSValue (if not already available)
#if !defined(NSValue_UIKIT_RECT_EXTENSIONS)
#define NSValue_UIKIT_RECT_EXTENSIONS

@interface NSValue (NSValueCGRectExtensions)

+ (NSValue *)valueWithCGRect:(CGRect)rect;
- (CGRect)CGRectValue;

@end

#endif

NS_ASSUME_NONNULL_END
