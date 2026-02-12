//
//  CGSize.h
//  pdUIKit - PocketDarwin UIKit Framework
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

// CGSize is already defined in AppKit, so we just add UIKit-specific functions
NS_ASSUME_NONNULL_BEGIN

// UIKit-specific CGSize functions
NSString *NSStringFromCGSize(CGSize size);

// Extension for NSValue (if not already available)
#if !defined(NSValue_UIKIT_SIZE_EXTENSIONS)
#define NSValue_UIKIT_SIZE_EXTENSIONS

@interface NSValue (NSValueCGSizeExtensions)

+ (NSValue *)valueWithCGSize:(CGSize)size;
- (CGSize)CGSizeValue;

@end

#endif

NS_ASSUME_NONNULL_END
