/*
 * SBApplicationIcon.h
 * SpringBoard - PocketDarwin
 *
 * A UIView subclass that renders one icon cell:
 *   - Glossy rounded-rect icon image (57×57)
 *   - Shine overlay (top half gloss)
 *   - Label (Helvetica 11, white, 1pt shadow)
 *   - Badge (red pill, top-right)
 *   - Wiggle animation in edit mode
 *
 * iPhoneOS 2.0 compatible.
 */

#import <UIKit/UIKit.h>

@class SBApplication;

/* Icon cell total size is 80×90 (icon 57×57 + label 80×20 + padding) */
extern const CGFloat kSBIconCellWidth;
extern const CGFloat kSBIconCellHeight;
extern const CGFloat kSBIconImageSize;
extern const CGFloat kSBIconCornerRadius;

@interface SBApplicationIcon : UIView {
    SBApplication  *_application;
    UIImageView    *_iconImageView;
    UIImageView    *_glassOverlay;
    UILabel        *_nameLabel;
    UILabel        *_badgeLabel;
    UIView         *_badgeView;
    BOOL            _isWiggling;
    BOOL            _isHighlighted;
}

@property (nonatomic, retain) SBApplication *application;
@property (nonatomic, assign) BOOL           isWiggling;

- (id)initWithApplication:(SBApplication *)application;

/* Start / stop the wiggle (edit mode) */
- (void)startWiggle;
- (void)stopWiggle;

/* Update the badge display */
- (void)reloadBadge;

/* Highlight state (tap-down) */
- (void)setHighlighted:(BOOL)highlighted;

@end