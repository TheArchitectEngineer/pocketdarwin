/*
 * main.m
 * Settings.app - PocketDarwin
 *
 * iPhoneOS 2.0 compatible.
 */

#import <UIKit/UIKit.h>
#import "SettingsAppDelegate.h"

int main(int argc, char *argv[])
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    int retVal = UIApplicationMain(argc, argv,
                                   nil,
                                   @"SettingsAppDelegate");
    [pool release];
    return retVal;
}
