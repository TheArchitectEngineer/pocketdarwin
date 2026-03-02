/*
 * SBIconList.m
 * SpringBoard - PocketDarwin
 */

#import <UIKit/UIKit.h>
#import "SBIconList.h"
#import "SBApplicationIcon.h"
#import "SBApplication.h"

const NSInteger kSBIconListMaxIcons = 16;

@implementation SBIconList

@synthesize delegate = _delegate;

- (id)initWithFrame:(CGRect)frame
{
    if ((self = [super initWithFrame:frame])) {
        _applications = [[NSMutableArray alloc] init];
        _iconViews    = [[NSMutableArray alloc] init];
        _editMode     = NO;
        self.backgroundColor = [UIColor clearColor];
    }
    return self;
}

- (void)_clearIcons
{
    for (UIView *v in _iconViews) {
        [v removeFromSuperview];
    }
    [_iconViews removeAllObjects];
}

- (void)reloadApplications:(NSArray *)applications
{
    [_applications removeAllObjects];
    if (applications) {
        [_applications addObjectsFromArray:applications];
    }

    [self _clearIcons];

    const CGFloat colStep = 76.0f;
    const CGFloat rowStep = 92.0f;
    const CGFloat leftPad = 8.0f;
    const CGFloat topPad  = 12.0f;

    NSInteger idx = 0;
    for (SBApplication *app in _applications) {
        SBApplicationIcon *icon = [[SBApplicationIcon alloc] initWithApplication:app];
        NSInteger row = idx / 4;
        NSInteger col = idx % 4;
        CGRect f = icon.frame;
        f.origin.x = leftPad + (col * colStep);
        f.origin.y = topPad + (row * rowStep);
        icon.frame = f;
        [self addSubview:icon];
        [_iconViews addObject:icon];
        [icon release];
        idx++;
        if (idx >= kSBIconListMaxIcons) break;
    }
}

- (void)beginEditMode
{
    if (_editMode) return;
    _editMode = YES;
    for (SBApplicationIcon *icon in _iconViews) {
        [icon startWiggle];
    }
}

- (void)endEditMode
{
    if (!_editMode) return;
    _editMode = NO;
    for (SBApplicationIcon *icon in _iconViews) {
        [icon stopWiggle];
    }
}

- (void)dealloc
{
    [_applications release];
    [_iconViews release];
    [super dealloc];
}

@end
