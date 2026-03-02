/*
 * SBDock.m
 * SpringBoard - PocketDarwin
 */

#import <UIKit/UIKit.h>
#import "SBDock.h"
#import "SBApplication.h"
#import "SBApplicationIcon.h"

const NSInteger kSBDockMaxIcons = 4;
const CGFloat   kSBDockHeight   = 90.0f;

@implementation SBDock

- (id)initWithFrame:(CGRect)frame
{
    if ((self = [super initWithFrame:frame])) {
        _dockIcons = [[NSMutableArray alloc] init];
        self.backgroundColor = [UIColor colorWithWhite:0.10f alpha:0.45f];
    }
    return self;
}

- (void)setApplications:(NSArray *)applications
{
    for (UIView *v in _dockIcons) {
        [v removeFromSuperview];
    }
    [_dockIcons removeAllObjects];

    NSInteger count = MIN((NSInteger)[applications count], kSBDockMaxIcons);
    for (NSInteger i = 0; i < count; i++) {
        SBApplication *app = [applications objectAtIndex:i];
        SBApplicationIcon *icon = [[SBApplicationIcon alloc] initWithApplication:app];
        [_dockIcons addObject:icon];
        [self addSubview:icon];
        [icon release];
    }
    [self layoutDockIcons];
}

- (void)layoutDockIcons
{
    CGFloat spacing = 6.0f;
    CGFloat iconW   = 76.0f;
    CGFloat totalW  = (iconW * kSBDockMaxIcons) + (spacing * (kSBDockMaxIcons - 1));
    CGFloat startX  = (CGRectGetWidth(self.bounds) - totalW) / 2.0f;
    CGFloat y       = 2.0f;

    NSInteger i = 0;
    for (UIView *v in _dockIcons) {
        CGRect f = v.frame;
        f.origin.x = startX + i * (iconW + spacing);
        f.origin.y = y;
        v.frame = f;
        i++;
    }
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    [self layoutDockIcons];
}

- (void)dealloc
{
    [_dockIcons release];
    [_backgroundView release];
    [super dealloc];
}

@end
