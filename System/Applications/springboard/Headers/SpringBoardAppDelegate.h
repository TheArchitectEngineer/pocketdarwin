/*
 * SpringBoardAppDelegate.h
 * SpringBoard - PocketDarwin
 */

#import <UIKit/UIKit.h>

@class SBSpringBoardView;

@interface SpringBoardAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow         *_window;
    SBSpringBoardView *_springBoardView;
}

@property (nonatomic, retain) UIWindow          *window;
@property (nonatomic, retain) SBSpringBoardView *springBoardView;

@end
