//
//  UIView.m
//  pdUIKit - PocketDarwin UIKit Framework
//

#import "UIView.h"
#import "UIEvent.h"
#import "UITouch.h"
#import "UIGestureRecognizer.h"
#import "UIWindow.h"
#import "UIViewController.h"

@implementation UIView {
    NSMutableArray<UIView *> *_subviews;
    NSMutableArray<UIGestureRecognizer *> *_gestureRecognizers;
    CGRect _frame;
    CGRect _bounds;
    CGPoint _center;
    CGAffineTransform _transform;
    CGFloat _alpha;
    BOOL _opaque;
    BOOL _hidden;
    BOOL _userInteractionEnabled;
    BOOL _clipsToBounds;
    BOOL _multipleTouchEnabled;
    BOOL _exclusiveTouch;
    UIColor *_backgroundColor;
    UIViewContentMode _contentMode;
    CGFloat _contentScaleFactor;
    NSInteger _tag;
    __weak UIViewController *_viewController;
}

#pragma mark - Initialization

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:NSRectFromCGRect(frame)]) {
        [self _commonInit];
        _frame = frame;
        _bounds = CGRectMake(0, 0, frame.size.width, frame.size.height);
        _center = CGPointMake(CGRectGetMidX(frame), CGRectGetMidY(frame));
    }
    return self;
}

- (instancetype)init {
    return [self initWithFrame:CGRectZero];
}

- (void)_commonInit {
    _subviews = [[NSMutableArray alloc] init];
    _gestureRecognizers = [[NSMutableArray alloc] init];
    _transform = CGAffineTransformIdentity;
    _alpha = 1.0;
    _opaque = YES;
    _hidden = NO;
    _userInteractionEnabled = YES;
    _clipsToBounds = NO;
    _multipleTouchEnabled = NO;
    _exclusiveTouch = NO;
    _backgroundColor = nil;
    _contentMode = UIViewContentModeScaleToFill;
    _contentScaleFactor = 1.0;
    _tag = 0;
}

#pragma mark - Properties

- (void)setFrame:(CGRect)frame {
    _frame = frame;
    [super setFrame:NSRectFromCGRect(frame)];
    _bounds = CGRectMake(0, 0, frame.size.width, frame.size.height);
    _center = CGPointMake(CGRectGetMidX(frame), CGRectGetMidY(frame));
}

- (CGRect)frame {
    return _frame;
}

- (void)setBounds:(CGRect)bounds {
    _bounds = bounds;
    [super setBounds:NSRectFromCGRect(bounds)];
}

- (CGRect)bounds {
    return _bounds;
}

- (void)setCenter:(CGPoint)center {
    _center = center;
    CGRect frame = self.frame;
    frame.origin.x = center.x - frame.size.width / 2.0;
    frame.origin.y = center.y - frame.size.height / 2.0;
    self.frame = frame;
}

- (CGPoint)center {
    return _center;
}

- (void)setTransform:(CGAffineTransform)transform {
    _transform = transform;
    // Apply transform to NSView
    NSAffineTransform *nsTransform = [NSAffineTransform transform];
    [nsTransform setTransformStruct:*((NSAffineTransformStruct *)&transform)];
    [self setWantsLayer:YES];
    [self.layer setAffineTransform:nsTransform];
}

- (CGAffineTransform)transform {
    return _transform;
}

- (void)setAlpha:(CGFloat)alpha {
    _alpha = alpha;
    self.alphaValue = alpha;
}

- (CGFloat)alpha {
    return _alpha;
}

- (void)setOpaque:(BOOL)opaque {
    _opaque = opaque;
    self.opaque = opaque;
}

- (BOOL)isOpaque {
    return _opaque;
}

- (void)setHidden:(BOOL)hidden {
    _hidden = hidden;
    self.hidden = hidden;
}

- (BOOL)isHidden {
    return _hidden;
}

- (void)setUserInteractionEnabled:(BOOL)userInteractionEnabled {
    _userInteractionEnabled = userInteractionEnabled;
}

- (BOOL)isUserInteractionEnabled {
    return _userInteractionEnabled;
}

- (void)setClipsToBounds:(BOOL)clipsToBounds {
    _clipsToBounds = clipsToBounds;
    self.wantsLayer = YES;
    self.layer.masksToBounds = clipsToBounds;
}

- (BOOL)isClipsToBounds {
    return _clipsToBounds;
}

- (void)setMultipleTouchEnabled:(BOOL)multipleTouchEnabled {
    _multipleTouchEnabled = multipleTouchEnabled;
}

- (BOOL)isMultipleTouchEnabled {
    return _multipleTouchEnabled;
}

- (void)setExclusiveTouch:(BOOL)exclusiveTouch {
    _exclusiveTouch = exclusiveTouch;
}

- (BOOL)isExclusiveTouch {
    return _exclusiveTouch;
}

- (void)setBackgroundColor:(UIColor *)backgroundColor {
    _backgroundColor = backgroundColor;
    if (backgroundColor) {
        self.layer.backgroundColor = [backgroundColor CGColor];
    } else {
        self.layer.backgroundColor = nil;
    }
}

- (UIColor *)backgroundColor {
    return _backgroundColor;
}

- (void)setContentMode:(UIViewContentMode)contentMode {
    _contentMode = contentMode;
}

- (UIViewContentMode)contentMode {
    return _contentMode;
}

- (void)setContentScaleFactor:(CGFloat)contentScaleFactor {
    _contentScaleFactor = contentScaleFactor;
    self.layer.contentsScale = contentScaleFactor;
}

- (CGFloat)contentScaleFactor {
    return _contentScaleFactor;
}

- (NSArray<UIView *> *)subviews {
    return [_subviews copy];
}

- (UIView *)superview {
    return (UIView *)self.superview;
}

- (UIWindow *)window {
    NSWindow *nsWindow = self.window;
    if ([nsWindow isKindOfClass:[UIWindow class]]) {
        return (UIWindow *)nsWindow;
    }
    return nil;
}

- (NSArray<UIGestureRecognizer *> *)gestureRecognizers {
    return [_gestureRecognizers copy];
}

- (void)setViewController:(UIViewController *)viewController {
    _viewController = viewController;
}

- (UIViewController *)viewController {
    return _viewController;
}

#pragma mark - View Hierarchy

- (void)addSubview:(UIView *)view {
    if (view && ![_subviews containsObject:view]) {
        [_subviews addObject:view];
        [super addSubview:view];
        view.viewController = self.viewController;
    }
}

- (void)insertSubview:(UIView *)view atIndex:(NSInteger)index {
    if (view && ![_subviews containsObject:view]) {
        [_subviews insertObject:view atIndex:index];
        [super addSubview:view positioned:NSWindowBelow relativeTo:nil];
        view.viewController = self.viewController;
    }
}

- (void)insertSubview:(UIView *)view aboveSubview:(UIView *)siblingSubview {
    if (view && siblingSubview && [_subviews containsObject:siblingSubview]) {
        NSInteger index = [_subviews indexOfObject:siblingSubview];
        [self insertSubview:view atIndex:index + 1];
    }
}

- (void)insertSubview:(UIView *)view belowSubview:(UIView *)siblingSubview {
    if (view && siblingSubview && [_subviews containsObject:siblingSubview]) {
        NSInteger index = [_subviews indexOfObject:siblingSubview];
        [self insertSubview:view atIndex:index];
    }
}

- (void)bringSubviewToFront:(UIView *)view {
    if (view && [_subviews containsObject:view]) {
        [view removeFromSuperview];
        [self addSubview:view];
    }
}

- (void)sendSubviewToBack:(UIView *)view {
    if (view && [_subviews containsObject:view]) {
        [view removeFromSuperview];
        [self insertSubview:view atIndex:0];
    }
}

- (void)removeFromSuperview {
    [_subviews removeObject:self];
    [super removeFromSuperview];
    self.viewController = nil;
}

#pragma mark - Hit Testing

- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event {
    if (!self.isUserInteractionEnabled || self.isHidden || self.alpha < 0.01) {
        return nil;
    }
    
    if (![self pointInside:point withEvent:event]) {
        return nil;
    }
    
    // Check subviews from front to back
    for (UIView *subview in [_subviews reverseObjectEnumerator]) {
        CGPoint convertedPoint = [self convertPoint:point toView:subview];
        UIView *hitView = [subview hitTest:convertedPoint withEvent:event];
        if (hitView) {
            return hitView;
        }
    }
    
    return self;
}

- (BOOL)pointInside:(CGPoint)point withEvent:(UIEvent *)event {
    return CGRectContainsPoint(self.bounds, point);
}

#pragma mark - Coordinate Conversion

- (CGPoint)convertPoint:(CGPoint)point toView:(UIView *)view {
    if (!view) {
        // Convert to window coordinates
        return [self convertPoint:point toView:self.window];
    }
    
    NSPoint nsPoint = NSPointFromCGPoint(point);
    NSPoint convertedPoint = [self convertPoint:nsPoint toView:view];
    return CGPointFromNSPoint(convertedPoint);
}

- (CGPoint)convertPoint:(CGPoint)point fromView:(UIView *)view {
    if (!view) {
        return [self convertPoint:point fromView:self.window];
    }
    
    NSPoint nsPoint = NSPointFromCGPoint(point);
    NSPoint convertedPoint = [self convertPoint:nsPoint fromView:view];
    return CGPointFromNSPoint(convertedPoint);
}

- (CGRect)convertRect:(CGRect)rect toView:(UIView *)view {
    if (!view) {
        return [self convertRect:rect toView:self.window];
    }
    
    NSRect nsRect = NSRectFromCGRect(rect);
    NSRect convertedRect = [self convertRect:nsRect toView:view];
    return CGRectFromNSRect(convertedRect);
}

- (CGRect)convertRect:(CGRect)rect fromView:(UIView *)view {
    if (!view) {
        return [self convertRect:rect fromView:self.window];
    }
    
    NSRect nsRect = NSRectFromCGRect(rect);
    NSRect convertedRect = [self convertRect:nsRect fromView:view];
    return CGRectFromNSRect(convertedRect);
}

#pragma mark - Gesture Recognizers

- (void)addGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer {
    if (gestureRecognizer && ![_gestureRecognizers containsObject:gestureRecognizer]) {
        [_gestureRecognizers addObject:gestureRecognizer];
        gestureRecognizer.view = self;
    }
}

- (void)removeGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer {
    if (gestureRecognizer) {
        [_gestureRecognizers removeObject:gestureRecognizer];
        gestureRecognizer.view = nil;
    }
}

#pragma mark - Drawing

- (void)setNeedsDisplay {
    [self setNeedsDisplay:YES];
}

- (void)setNeedsDisplayInRect:(CGRect)rect {
    [self setNeedsDisplayInRect:NSRectFromCGRect(rect)];
}

- (void)layoutSubviews {
    // Override in subclasses
}

- (void)layoutIfNeeded {
    [self layoutSubviews];
}

#pragma mark - Touch Events (adapted from NSView)

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    // Override in subclasses
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    // Override in subclasses
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    // Override in subclasses
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    // Override in subclasses
}

// Override NSView mouse events to convert to UIKit touch events
- (void)mouseDown:(NSEvent *)event {
    UITouch *touch = [UITouch touchWithNSEvent:event];
    UIEvent *uiEvent = [UIEvent eventWithNSEvent:event];
    [self touchesBegan:[NSSet setWithObject:touch] withEvent:uiEvent];
}

- (void)mouseDragged:(NSEvent *)event {
    UITouch *touch = [UITouch touchWithNSEvent:event];
    UIEvent *uiEvent = [UIEvent eventWithNSEvent:event];
    [self touchesMoved:[NSSet setWithObject:touch] withEvent:uiEvent];
}

- (void)mouseUp:(NSEvent *)event {
    UITouch *touch = [UITouch touchWithNSEvent:event];
    UIEvent *uiEvent = [UIEvent eventWithNSEvent:event];
    [self touchesEnded:[NSSet setWithObject:touch] withEvent:uiEvent];
}

#pragma mark - GNUStep Compatibility

- (NSView *)NSViewValue {
    return self;
}

@end
