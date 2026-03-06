/*
 * SpringBoardAppDelegate.m
 * SpringBoard - PocketDarwin
 *
 * iPhoneOS 2.0 — Manual Retain/Release
 */

#import <UIKit/UIKit.h>
#import "SpringBoardAppDelegate.h"
#import "SBSpringBoardView.h"
#import "SBApplication.h"

/* PocketDarwin applications directory */
static NSString * const kPocketDarwinAppsDir   = @"/Applications";
static NSString * const kPocketDarwinSystemDir = @"/System/Library/CoreServices";

@implementation SpringBoardAppDelegate

@synthesize window          = _window;
@synthesize springBoardView = _springBoardView;

#pragma mark - UIApplicationDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    /* iPhoneOS 2.0: no launchOptions parameter */

    /* Hide the system status bar — we draw our own */
    [application setStatusBarHidden:YES animated:NO];

    /* Create the full-screen window */
    _window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    _window.backgroundColor = [UIColor blackColor];

    /* Create SpringBoard root view */
    _springBoardView = [[SBSpringBoardView alloc] initWithFrame:[UIScreen mainScreen].bounds];

    /* Discover and load applications */
    NSArray *mainApps = [self _buildDefaultApplicationList];
    NSArray *dockApps = [self _buildDefaultDockApplicationList];

    [_springBoardView loadApplications:mainApps];
    [_springBoardView loadDockApplications:dockApps];

    [_window addSubview:_springBoardView];
    [_window makeKeyAndVisible];

    /* Register for Home button notification */
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(_homeButtonPressed)
                                                 name:@"UIApplicationHomeButtonPressed"
                                               object:nil];

    NSLog(@"PocketDarwin SpringBoard: ready.");
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    /* SpringBoard should never actually terminate on a real device */
    NSLog(@"PocketDarwin SpringBoard: WARNING — SpringBoard is terminating!");
}

#pragma mark - Application discovery

- (NSArray *)_scanApplicationsDirectory
{
    NSMutableArray *apps = [NSMutableArray array];
    NSFileManager  *fm   = [NSFileManager defaultManager];
    NSError        *err  = nil;

    NSArray *entries = [fm contentsOfDirectoryAtPath:kPocketDarwinAppsDir error:&err];
    if (!entries) {
        NSLog(@"SpringBoard: Cannot read %@: %@", kPocketDarwinAppsDir, err);
        return apps;
    }

    for (NSString *name in entries) {
        if (![name hasSuffix:@".app"]) continue;
        NSString *fullPath = [kPocketDarwinAppsDir stringByAppendingPathComponent:name];
        BOOL isDir = NO;
        if ([fm fileExistsAtPath:fullPath isDirectory:&isDir] && isDir) {
            SBApplication *app = [SBApplication applicationWithBundlePath:fullPath];
            if (app && !app.isHidden) {
                [apps addObject:app];
            }
        }
    }

    return apps;
}

- (NSArray *)_buildDefaultApplicationList
{
    /*
     * PocketDarwin default home screen layout.
     * These mirror the iPhoneOS 2.0 stock application set,
     * re-implemented for PocketDarwin.
     *
     * In production, this comes from scanning /Applications
     * and reading each bundle's Info.plist.
     */

    /* First try scanning the real directory */
    NSArray *scanned = [self _scanApplicationsDirectory];
    if ([scanned count] > 0) {
        return scanned;
    }

    /* Fallback: synthesize built-in app stubs for the simulator/demo */
    NSArray *defaultBundleIDs = [NSArray arrayWithObjects:
        /* Row 1 */
        @"com.pocketdarwin.MobileSMS",
        @"com.pocketdarwin.MobileCalendar",
        @"com.pocketdarwin.MobileSlideShow",
        @"com.pocketdarwin.MobileCamera",
        /* Row 2 */
        @"com.pocketdarwin.MobileMail",
        @"com.pocketdarwin.MobileSafari",
        @"com.pocketdarwin.MobileIPod",
        @"com.pocketdarwin.MobileMaps",
        /* Row 3 */
        @"com.pocketdarwin.MobileWeather",
        @"com.pocketdarwin.MobileClock",
        @"com.pocketdarwin.MobileCalculator",
        @"com.pocketdarwin.MobileNotes",
        /* Row 4 */
        @"com.pocketdarwin.MobileStocks",
        @"com.pocketdarwin.MobileItunes",
        @"com.pocketdarwin.AppStore",
        @"com.pocketdarwin.MobileSettings",
        nil];

    NSArray *displayNames = [NSArray arrayWithObjects:
        @"Messages", @"Calendar",  @"Photos",    @"Camera",
        @"Mail",     @"Safari",    @"iPod",      @"Maps",
        @"Weather",  @"Clock",     @"Calculator",@"Notes",
        @"Stocks",   @"iTunes",    @"App Store", @"Settings",
        nil];

    NSMutableArray *apps = [NSMutableArray array];
    NSUInteger count = [defaultBundleIDs count];
    for (NSUInteger i = 0; i < count; i++) {
        SBApplication *app = [[[SBApplication alloc] init] autorelease];
        app.bundleIdentifier = [defaultBundleIDs objectAtIndex:i];
        app.displayName      = [displayNames objectAtIndex:i];
        app.isSystemApplication = YES;
        [app loadIconImages];  /* will use placeholder if Icon.png absent */
        [apps addObject:app];
    }

    return apps;
}

- (NSArray *)_buildDefaultDockApplicationList
{
    /*
     * iPhoneOS 2.0 default dock: Phone, Mail, Safari, iPod
     * On PocketDarwin there is no Phone, so: Mail, Safari, Settings, Terminal
     */
    NSArray *dockBundleIDs = [NSArray arrayWithObjects:
        @"com.pocketdarwin.MobileMail",
        @"com.pocketdarwin.MobileSafari",
        @"com.pocketdarwin.MobileSettings",
        @"com.pocketdarwin.Terminal",
        nil];

    NSArray *dockNames = [NSArray arrayWithObjects:
        @"Mail", @"Safari", @"Settings", @"Terminal", nil];

    /* Try to find matching apps from /Applications */
    NSArray *scanned = [self _scanApplicationsDirectory];
    if ([scanned count] > 0) {
        NSMutableArray *dockApps = [NSMutableArray array];
        for (NSString *bid in dockBundleIDs) {
            for (SBApplication *app in scanned) {
                if ([app.bundleIdentifier isEqualToString:bid]) {
                    [dockApps addObject:app];
                    break;
                }
            }
            if ([dockApps count] == 4) break;
        }
        return dockApps;
    }

    /* Fallback stubs */
    NSMutableArray *apps = [NSMutableArray array];
    NSUInteger dockCount = [dockBundleIDs count];
    for (NSUInteger i = 0; i < dockCount; i++) {
        SBApplication *app = [[[SBApplication alloc] init] autorelease];
        app.bundleIdentifier = [dockBundleIDs objectAtIndex:i];
        app.displayName      = [dockNames objectAtIndex:i];
        app.isSystemApplication = YES;
        [app loadIconImages];
        [apps addObject:app];
    }
    return apps;
}

#pragma mark - Home button

- (void)_homeButtonPressed
{
    /* Exit edit mode if active */
    [_springBoardView exitEditMode];

    /* Scroll back to page 0 */
    /* (SBSpringBoardView handles this internally via its UIScrollView) */
}

#pragma mark - Dealloc

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [_springBoardView release];
    [_window          release];
    [super dealloc];
}

@end