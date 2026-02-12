//
//  UITouch.m
//  pdUIKit - PocketDarwin UIKit Framework
//

#import "UITouch.h"
#import "UIEvent.h"
#import "UIWindow.h"

@implementation UITouch {
    UITouchPhase _phase;
    NSTimeInterval _timestamp;
    NSUInteger _tapCount;
    UIView *_view;
    UIWindow *_window;
    CGPoint _location;
    CGPoint _previousLocation;
    CGFloat _force;
    CGFloat _maximumPossibleForce;
    UITouchType _type;
    NSInteger _identifier;
}

+ (UITouch *)touchWithNSEvent:(NSEvent *)event {
    UITouch *touch = [[UITouch alloc] init];
    touch.timestamp = event.timestamp;
    touch.tapCount = event.clickCount;
    
    // Convert NSEvent location to touch location
    NSPoint location = event.locationInWindow;
    touch._location = CGPointFromNSPoint(location);
    touch._previousLocation = touch._location;
    
    // Determine phase from NSEvent type
    switch (event.type) {
        case NSEventTypeLeftMouseDown:
        case NSEventTypeRightMouseDown:
            touch.phase = UITouchPhaseBegan;
            break;
        case NSEventTypeLeftMouseDragged:
        case NSEventTypeRightMouseDragged:
            touch.phase = UITouchPhaseMoved;
            break;
        case NSEventTypeLeftMouseUp:
        case NSEventTypeRightMouseUp:
            touch.phase = UITouchPhaseEnded;
            break;
        default:
            touch.phase = UITouchPhaseStationary;
            break;
    }
    
    return touch;
}

- (CGPoint)locationInView:(UIView *)view {
    if (!view) return _location;
    return [view convertPoint:_location fromView:nil];
}

- (CGPoint)previousLocationInView:(UIView *)view {
    if (!view) return _previousLocation;
    return [view convertPoint:_previousLocation fromView:nil];
}

@end
