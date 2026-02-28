/*
 * SBSpringBoardView.m
 * SpringBoard - PocketDarwin
 *
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import "SBSpringBoardView.h"
#import "SBStatusBar.h"
#import "SBDock.h"
#import "SBIconList.h"
#import "SBApplication.h"

/* Screen constants for iPhoneOS 2.0 */
static const CGFloat kScreenWidth  = 320.0f;
static const CGFloat kScreenHeight = 480.0f;

@implementation SBSpringBoardView

#pragma mark - Init

- (id)initWithFrame:(CGRect)frame
{
    if ((self = [super initWithFrame:frame])) {
        _pages       = [[NSMutableArray alloc] init];
        _allApps     = [[NSMutableArray alloc] init];
        _currentPage = 0;
        _editMode    = NO;

        [self _buildWallpaper];
        [self _buildStatusBar];
        [self _buildScrollView];
        [self _buildDock];
        [self _buildPageControl];
    }
    return self;
}

#pragma mark - Subview construction

- (void)_buildWallpaper
{
    /* 
     * Default iPhoneOS 2.0 wallpaper: black on SpringBoard,
     * but we draw a classic blue gradient (iPhone original default).
     */
    UIGraphicsBeginImageContext(CGSizeMake(kScreenWidth, kScreenHeight));
    CGContextRef ctx = UIGraphicsGetCurrentContext();

    CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
    CGFloat comps[16] = {
        0.06f, 0.12f, 0.22f, 1.0f,
        0.10f, 0.20f, 0.38f, 1.0f,
        0.05f, 0.10f, 0.20f, 1.0f,
        0.02f, 0.04f, 0.08f, 1.0f
    };
    CGFloat locs[4] = { 0.0f, 0.35f, 0.70f, 1.0f };
    CGGradientRef gradient = CGGradientCreateWithColorComponents(space, comps, locs, 4);

    CGContextDrawLinearGradient(ctx, gradient,
                                CGPointMake(0, 0),
                                CGPointMake(0, kScreenHeight),
                                0);

    CGGradientRelease(gradient);
    CGColorSpaceRelease(space);

    UIImage *wpImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();

    _wallpaperView = [[UIImageView alloc] initWithFrame:self.bounds];
    _wallpaperView.image = wpImage;
    [self addSubview:_wallpaperView];
}

- (void)_buildStatusBar
{
    _statusBar = [[SBStatusBar alloc] initWithFrame:CGRectMake(0, 0, kScreenWidth, kSBStatusBarHeight)
                                              style:SBStatusBarStyleBlack];
    [self addSubview:_statusBar];
}

- (void)_buildScrollView
{
    CGFloat scrollY = kSBStatusBarHeight;
    CGFloat scrollH = kScreenHeight - kSBStatusBarHeight - kSBDockHeight;

    _pageScrollView = [[UIScrollView alloc] initWithFrame:
                       CGRectMake(0, scrollY, kScreenWidth, scrollH)];
    _pageScrollView.pagingEnabled          = YES;
    _pageScrollView.showsHorizontalScrollIndicator = NO;
    _pageScrollView.showsVerticalScrollIndicator   = NO;
    _pageScrollView.scrollsToTop           = NO;
    _pageScrollView.bounces                = NO;
    _pageScrollView.backgroundColor        = [UIColor clearColor];
    _pageScrollView.delegate               = self;
    [self addSubview:_pageScrollView];
}

- (void)_buildDock
{
    CGFloat dockY = kScreenHeight - kSBDockHeight;
    _dock = [[SBDock alloc] initWithFrame:CGRectMake(0, dockY, kScreenWidth, kSBDockHeight)];
    [self addSubview:_dock];
}

- (void)_buildPageControl
{
    /* Page dots sit just above the dock */
    CGFloat dotY = kScreenHeight - kSBDockHeight - 20.0f;
    _pageControl = [[UIPageControl alloc] initWithFrame:
                    CGRectMake(0, dotY, kScreenWidth, 14.0f)];
    _pageControl.currentPage                = 0;
    _pageControl.pageIndicatorTintColor     = [UIColor colorWithWhite:1.0f alpha:0.3f];
    _pageControl.currentPageIndicatorTintColor = [UIColor whiteColor];
    [self addSubview:_pageControl];
}

#pragma mark - App loading

- (void)loadApplications:(NSArray *)applications
{
    [_allApps removeAllObjects];
    [_allApps addObjectsFromArray:applications];

    /* Remove existing pages */
    for (SBIconList *page in _pages) {
        [page removeFromSuperview];
    }
    [_pages removeAllObjects];

    /* Paginate: 16 icons per page (4×4) */
    NSInteger totalApps = [applications count];
    NSInteger appsPerPage = kSBIconListMaxIcons;
    NSInteger totalPages = (totalApps + appsPerPage - 1) / appsPerPage;
    if (totalPages == 0) totalPages = 1;

    CGFloat scrollH = CGRectGetHeight(_pageScrollView.bounds);

    for (NSInteger p = 0; p < totalPages; p++) {
        NSRange range = NSMakeRange(p * appsPerPage,
                                    MIN(appsPerPage, totalApps - p * appsPerPage));
        NSArray *pageApps = [applications subarrayWithRange:range];

        SBIconList *page = [[SBIconList alloc] initWithFrame:
                            CGRectMake(p * kScreenWidth, 0, kScreenWidth, scrollH)];
        page.delegate = self;
        [page reloadApplications:pageApps];
        [_pageScrollView addSubview:page];
        [_pages addObject:page];
        [page release];
    }

    _pageScrollView.contentSize = CGSizeMake(kScreenWidth * totalPages, scrollH);

    _pageControl.numberOfPages = totalPages;
    _pageControl.currentPage   = 0;
}

- (void)loadDockApplications:(NSArray *)applications
{
    [_dock setApplications:applications];
}

#pragma mark - Edit mode

- (void)exitEditMode
{
    if (!_editMode) return;
    _editMode = NO;
    for (SBIconList *page in _pages) {
        [page endEditMode];
    }
}

#pragma mark - SBIconListDelegate

- (void)iconListWantsEditMode:(id)iconList
{
    if (_editMode) return;
    _editMode = YES;
    for (SBIconList *page in _pages) {
        [page beginEditMode];
    }
}

- (void)iconList:(id)iconList didSelectApplication:(SBApplication *)application
{
    /* SpringBoard launches the app; the app takes over the screen */
    [application launch];
}

#pragma mark - UIScrollViewDelegate

- (void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView
{
    CGFloat pageWidth = CGRectGetWidth(scrollView.bounds);
    NSInteger page    = (NSInteger)(scrollView.contentOffset.x / pageWidth + 0.5f);
    _currentPage      = page;
    _pageControl.currentPage = page;
}

#pragma mark - Dealloc

- (void)dealloc
{
    [_statusBar      release];
    [_pageScrollView release];
    [_dock           release];
    [_pageControl    release];
    [_pages          release];
    [_allApps        release];
    [_wallpaperView  release];
    [super dealloc];
}

@end