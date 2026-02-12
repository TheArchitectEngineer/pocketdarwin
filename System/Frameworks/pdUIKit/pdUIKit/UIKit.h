//
//  UIKit.h
//  pdUIKit - PocketDarwin UIKit Framework
//  Touch-oriented adaptation of GNUStep AppKit/Cocoa
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

// Core UIKit Classes
#import <pdUIKit/UIApplication.h>
#import <pdUIKit/UIWindow.h>
#import <pdUIKit/UIView.h>
#import <pdUIKit/UIViewController.h>
#import <pdUIKit/UIScreen.h>

// Geometry and Utilities
#import <pdUIKit/CGPoint.h>
#import <pdUIKit/CGSize.h>
#import <pdUIKit/CGRect.h>
#import <pdUIKit/UIColor.h>
#import <pdUIKit/UIFont.h>
#import <pdUIKit/UIImage.h>

// Touch and Event Handling
#import <pdUIKit/UITouch.h>
#import <pdUIKit/UIEvent.h>
#import <pdUIKit/UIGestureRecognizer.h>
#import <pdUIKit/UITapGestureRecognizer.h>
#import <pdUIKit/UIPanGestureRecognizer.h>
#import <pdUIKit/UIPinchGestureRecognizer.h>

// UI Controls
#import <pdUIKit/UILabel.h>
#import <pdUIKit/UIButton.h>
#import <pdUIKit/UITextField.h>
#import <pdUIKit/UITextView.h>
#import <pdUIKit/UIImageView.h>

// View Controllers
#import <pdUIKit/UINavigationController.h>
#import <pdUIKit/UITabBarController.h>
#import <pdUIKit/UITableViewController.h>
#import <pdUIKit/UICollectionViewController.h>

// Container Views
#import <pdUIKit/UIScrollView.h>
#import <pdUIKit/UITableView.h>
#import <pdUIKit/UICollectionView.h>
#import <pdUIKit/UITableViewCell.h>

// Bars and Navigation
#import <pdUIKit/UINavigationBar.h>
#import <pdUIKit/UITabBar.h>
#import <pdUIKit/UIToolbar.h>
#import <pdUIKit/UINavigationItem.h>
#import <pdUIKit/UITabBarItem.h>

// Alerts and Actions
#import <pdUIKit/UIAlertController.h>
#import <pdUIKit/UIActionSheet.h>

// Constants and Enums
#import <pdUIKit/UIApplication.h>
#import <pdUIKit/UIView.h>
#import <pdUIKit/UIViewController.h>

//! pdUIKit version information
FOUNDATION_EXPORT double pdUIKitVersionNumber;
FOUNDATION_EXPORT const unsigned char pdUIKitVersionString[];

// Platform identification macro
#define UIKIT_STRINGIFY(x) #x
#define UIKIT_STRINGIFY2(x) UIKIT_STRINGIFY(x)
#define UIKIT_VERSION UIKIT_STRINGIFY2(UIKIT_VERSION_MAJOR) "." UIKIT_STRINGIFY2(UIKIT_VERSION_MINOR) "." UIKIT_STRINGIFY2(UIKIT_VERSION_PATCH)

// Touch adaptation macros for GNUStep compatibility
#define UI_TOUCH_EVENT_FROM_NSEVENT(nsevent) [UITouch touchWithNSEvent:nsevent]
#define UI_GESTURE_FROM_NSEVENT(nsevent) [[UIGestureRecognizer alloc] initWithNSEvent:nsevent]