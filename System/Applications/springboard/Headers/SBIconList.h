/*
 * SBIconList.h
 * SpringBoard - PocketDarwin
 */

#import <UIKit/UIKit.h>

@class SBApplication;
@class SBApplicationIcon;

extern const NSInteger kSBIconListMaxIcons;

@protocol SBIconListDelegate <NSObject>
- (void)iconListWantsEditMode:(id)iconList;
- (void)iconList:(id)iconList didSelectApplication:(SBApplication *)application;
@end

@interface SBIconList : UIView {
    NSMutableArray *_applications;
    NSMutableArray *_iconViews;
    id<SBIconListDelegate> _delegate;
    BOOL _editMode;
}

@property (nonatomic, assign) id<SBIconListDelegate> delegate;

- (void)reloadApplications:(NSArray *)applications;
- (void)beginEditMode;
- (void)endEditMode;

@end
