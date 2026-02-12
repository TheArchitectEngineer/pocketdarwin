//
//  UIView.h
//  pdUIKit - PocketDarwin UIKit Framework
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "UIColor.h"
#import "CGPoint.h"
#import "CGSize.h"
#import "CGRect.h"

NS_ASSUME_NONNULL_BEGIN

// UIView content modes
typedef NS_ENUM(NSInteger, UIViewContentMode) {
    UIViewContentModeScaleToFill,
    UIViewContentModeScaleAspectFit,
    UIViewContentModeScaleAspectFill,
    UIViewContentModeRedraw,
    UIViewContentModeCenter,
    UIViewContentModeTop,
    UIViewContentModeBottom,
    UIViewContentModeLeft,
    UIViewContentModeRight,
    UIViewContentModeTopLeft,
    UIViewContentModeTopRight,
    UIViewContentModeBottomLeft,
    UIViewContentModeBottomRight,
};

// UIView animation curves
typedef NS_ENUM(NSInteger, UIViewAnimationCurve) {
    UIViewAnimationCurveEaseInOut,
    UIViewAnimationCurveEaseIn,
    UIViewAnimationCurveEaseOut,
    UIViewAnimationCurveLinear,
};

// Forward declarations
@class UIEvent, UITouch, UIGestureRecognizer, UIViewController;

// UIView class - adaptation of NSView for UIKit
@interface UIView : NSView

// Geometry and frame
@property (nonatomic) CGRect frame;
@property (nonatomic) CGRect bounds;
@property (nonatomic) CGPoint center;
@property (nonatomic) CGAffineTransform transform;
@property (nonatomic) CGFloat alpha;
@property (nonatomic, getter=isOpaque) BOOL opaque;
@property (nonatomic, getter=isHidden) BOOL hidden;
@property (nonatomic, getter=isUserInteractionEnabled) BOOL userInteractionEnabled;
@property (nonatomic, getter=isClipsToBounds) BOOL clipsToBounds;
@property (nonatomic, getter=isMultipleTouchEnabled) BOOL multipleTouchEnabled;
@property (nonatomic, getter=isExclusiveTouch) BOOL exclusiveTouch;

// View hierarchy
@property (nonatomic, readonly, nullable) UIView *superview;
@property (nonatomic, readonly, copy) NSArray<UIView *> *subviews;
@property (nonatomic, readonly, nullable) UIWindow *window;
@property (nonatomic, weak, nullable) UIViewController *viewController;

// Appearance and behavior
@property (nonatomic, strong) UIColor *backgroundColor;
@property (nonatomic) UIViewContentMode contentMode;
@property (nonatomic) CGFloat contentScaleFactor;

// Gesture recognizers
@property (nonatomic, readonly, copy) NSArray<UIGestureRecognizer *> *gestureRecognizers;

// Adding and removing subviews
- (void)addSubview:(UIView *)view;
- (void)insertSubview:(UIView *)view atIndex:(NSInteger)index;
- (void)insertSubview:(UIView *)view aboveSubview:(UIView *)siblingSubview;
- (void)insertSubview:(UIView *)view belowSubview:(UIView *)siblingSubview;
- (void)bringSubviewToFront:(UIView *)view;
- (void)sendSubviewToBack:(UIView *)view;
- (void)removeFromSuperview;

// View hierarchy inspection
- (nullable UIView *)viewWithTag:(NSInteger)tag;
- (nullable __kindof UIView *)viewWithTag:(NSInteger)tag;

// Converting coordinates
- (CGPoint)convertPoint:(CGPoint)point toView:(nullable UIView *)view;
- (CGPoint)convertPoint:(CGPoint)point fromView:(nullable UIView *)view;
- (CGRect)convertRect:(CGRect)rect toView:(nullable UIView *)view;
- (CGRect)convertRect:(CGRect)rect fromView:(nullable UIView *)view;

// Hit testing
- (nullable UIView *)hitTest:(CGPoint)point withEvent:(nullable UIEvent *)event;
- (BOOL)pointInside:(CGPoint)point withEvent:(nullable UIEvent *)event;

// Gesture recognizer methods
- (void)addGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer;
- (void)removeGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer;

// Drawing and animation
- (void)setNeedsDisplay;
- (void)setNeedsDisplayInRect:(CGRect)rect;
- (void)layoutIfNeeded;
- (void)layoutSubviews;

// Class methods for animation
+ (void)animateWithDuration:(NSTimeInterval)duration 
                 animations:(void (^)(void))animations;
+ (void)animateWithDuration:(NSTimeInterval)duration 
                 animations:(void (^)(void))animations 
                 completion:(void (^ __nullable)(BOOL finished))completion;

+ (void)animateWithDuration:(NSTimeInterval)duration 
                      delay:(NSTimeInterval)delay 
                    options:(UIViewAnimationOptions)options 
                 animations:(void (^)(void))animations 
                 completion:(void (^ __nullable)(BOOL finished))completion;

// Touch event handling (adapted from NSView)
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event;
- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event;
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event;
- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event;

// GNUStep compatibility
- (NSView *)NSViewValue;

@end

NS_ASSUME_NONNULL_END
