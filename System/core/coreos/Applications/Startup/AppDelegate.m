#import "AppDelegate.h"
#import "FDStartup.h"
#import <stdlib.h>

@interface FDStartupChooserController : UITableViewController {
    AppDelegate *_startupDelegate;
    NSArray *_bundles;
}

- (id)initWithStartupDelegate:(AppDelegate *)delegate bundles:(NSArray *)bundles;

@end

@implementation FDStartupChooserController

- (id)initWithStartupDelegate:(AppDelegate *)delegate bundles:(NSArray *)bundles {
    self = [super initWithStyle:UITableViewStylePlain];
    if (self != nil) {
        _startupDelegate = delegate;
        _bundles = [bundles retain];
        self.title = @"Startup Bundle";
    }
    return self;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [_bundles count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString * const kCellIdentifier = @"StartupBundleCell";

    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:kCellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle
                                       reuseIdentifier:kCellIdentifier] autorelease];
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    }

    FDStartup *bundle = [_bundles objectAtIndex:indexPath.row];
    cell.textLabel.text = bundle.displayName;
    if (bundle.bundleIdentifier != nil) {
        cell.detailTextLabel.text = bundle.bundleIdentifier;
    } else {
        cell.detailTextLabel.text = [bundle.bundlePath lastPathComponent];
    }
    cell.imageView.image = bundle.icon;

    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    FDStartup *bundle = [_bundles objectAtIndex:indexPath.row];
    [_startupDelegate launchStartupBundle:bundle];
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
}

- (void)dealloc {
    [_bundles release];
    [super dealloc];
}

@end

@implementation AppDelegate

@synthesize window = _window;
@synthesize navigationController = _navigationController;
@synthesize startupBundles = _startupBundles;

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    [application setStatusBarHidden:NO animated:NO];

    _window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    _window.backgroundColor = [UIColor whiteColor];

    NSString *configurationPath = [[NSBundle mainBundle] pathForResource:@"BootMk" ofType:@"plist"];
    _startupBundles = [[FDStartup startupBundlesFromConfiguration:configurationPath] retain];

    FDStartupChooserController *chooser = [[[FDStartupChooserController alloc]
                                            initWithStartupDelegate:self
                                                           bundles:_startupBundles] autorelease];
    _navigationController = [[UINavigationController alloc] initWithRootViewController:chooser];
    _navigationController.navigationBar.barStyle = UIBarStyleDefault;
    _navigationController.navigationBar.tintColor =
        [UIColor colorWithRed:0.35f green:0.40f blue:0.47f alpha:1.0f];

    [_window addSubview:_navigationController.view];
    [_window makeKeyAndVisible];

    if ([_startupBundles count] == 0) {
        UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"No Startup Bundles"
                                                          message:@"No launchable .app bundles were found."
                                                         delegate:nil
                                                cancelButtonTitle:@"OK"
                                                otherButtonTitles:nil] autorelease];
        [alert show];
    }
}

- (void)launchStartupBundle:(FDStartup *)bundle {
    NSError *error = nil;
    if ([bundle launchAndReturnError:&error]) {
        exit(0);
        return;
    }

    NSString *message = @"Unable to launch the selected startup bundle.";
    if (error != nil) {
        message = [error localizedDescription];
    }

    UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Launch Failed"
                                                      message:message
                                                     delegate:nil
                                            cancelButtonTitle:@"OK"
                                            otherButtonTitles:nil] autorelease];
    [alert show];
}

- (void)dealloc {
    [_startupBundles release];
    [_navigationController release];
    [_window release];
    [super dealloc];
}

@end
