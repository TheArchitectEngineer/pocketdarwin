/*
 * main.m
 * SpringBoard - PocketDarwin
 *
 * Entry point for the SpringBoard process.
 * On real iPhoneOS, SpringBoard is launched by launchd immediately
 * after the kernel and dyld finish bootstrapping.
 *
 * iPhoneOS 2.0 compatible.
 */

#import <UIKit/UIKit.h>
#import "SpringBoardAppDelegate.h"

int main(int argc, char *argv[])
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    int retVal = UIApplicationMain(argc, argv,
                                   nil,  /* use UIApplication as principal class */
                                   @"SpringBoardAppDelegate");
    [pool release];
    return retVal;
}