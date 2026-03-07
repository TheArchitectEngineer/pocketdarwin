#import <UIKit/UIKit.h>
#import "MobileTerminalAppDelegate.h"

int main(int argc, char *argv[])
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, @"MobileTerminalAppDelegate");
    [pool release];
    return retVal;
}
