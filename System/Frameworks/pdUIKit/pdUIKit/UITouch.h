//
//  UITouch.h
//  pdUIKit - PocketDarwin UIKit Framework
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "CGPoint.h"
#import "UIView.h"

NS_ASSUME_NONNULL_BEGIN

// Touch phases
typedef NS_ENUM(NSInteger, UITouchPhase) {
    UITouchPhaseBegan,
    UITouchPhaseMoved,
    UITouchPhaseStationary,
    UITouchPhaseEnded,
    UITouchPhaseCancelled,
};

// Forward declarations
@class UIEvent, UIView;

// UITouch class - adaptation of NSEvent for touch input
@interface UITouch : NSObject

// Touch properties
@property (nonatomic, readonly) UITouchPhase phase;
@property (nonatomic, readonly) NSTimeInterval timestamp;
@property (nonatomic, readonly) NSUInteger tapCount;
@property (nonatomic, readonly, nullable) UIView *view;
@property (nonatomic, readonly, nullable) UIWindow *window;
@property (nonatomic, readonly) CGPoint locationInView:(UIView *)view;
@property (nonatomic, readonly) CGPoint previousLocationInView:(UIView *)view;

// Force properties (for pressure-sensitive devices)
@property (nonatomic, readonly) CGFloat force;
@property (nonatomic, readonly) CGFloat maximumPossibleForce;

// Touch type
@property (nonatomic, readonly) UITouchType type;

// Touch identification
@property (nonatomic, readonly) NSInteger identifier;

// Creation from NSEvent (GNUStep adaptation)
+ (UITouch *)touchWithNSEvent:(NSEvent *)event;

@end

NS_ASSUME_NONNULL_END
