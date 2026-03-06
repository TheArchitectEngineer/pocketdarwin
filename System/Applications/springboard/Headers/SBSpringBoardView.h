/*
 * SBSpringBoardView.h
 * SpringBoard - PocketDarwin
 */

#import <UIKit/UIKit.h>
#import "SBIconList.h"

@class SBStatusBar;
@class SBDock;
@class SBApplication;

@interface SBSpringBoardView : UIView <UIScrollViewDelegate, SBIconListDelegate> {
    SBStatusBar    *_statusBar;
    UIScrollView   *_pageScrollView;
    SBDock         *_dock;
    UIPageControl  *_pageControl;
    NSMutableArray *_pages;
    NSMutableArray *_allApps;
    UIImageView    *_wallpaperView;
    NSInteger       _currentPage;
    BOOL            _editMode;
}

- (void)loadApplications:(NSArray *)applications;
- (void)loadDockApplications:(NSArray *)applications;
- (void)exitEditMode;

@end
