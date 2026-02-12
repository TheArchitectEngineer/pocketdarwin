/*
 * AppScanner.mm
 * PocketDarwin freeboard - scan /Applications for .app bundles (Objective-C)
 */

#import <Foundation/Foundation.h>
#include "AppBundle.h"

namespace freeboard {

static std::string nsstringToStdString(NSString* s) {
    if (!s) return "";
    const char* c = [s UTF8String];
    return c ? std::string(c) : "";
}

std::vector<AppInfo> scanApplications(const std::string& applicationsPath) {
    std::vector<AppInfo> result;
    @autoreleasepool {
        NSString* path = [NSString stringWithUTF8String: applicationsPath.c_str()];
        NSFileManager* fm = [NSFileManager defaultManager];
        NSError* err = nil;
        NSArray* contents = [fm contentsOfDirectoryAtPath: path error: &err];
        if (!contents || err) return result;

        for (NSString* name in contents) {
            if (![name hasSuffix: @".app"]) continue;
            NSString* bundlePath = [path stringByAppendingPathComponent: name];
            BOOL isDir = NO;
            if (![fm fileExistsAtPath: bundlePath isDirectory: &isDir] || !isDir)
                continue;

            NSString* contentsPath = [bundlePath stringByAppendingPathComponent: @"Contents"];
            NSString* plistPath = [contentsPath stringByAppendingPathComponent: @"Info.plist"];
            NSString* macOSPath = [contentsPath stringByAppendingPathComponent: @"MacOS"];
            NSString* resourcesPath = [contentsPath stringByAppendingPathComponent: @"Resources"];

            NSString* displayName = [name stringByDeletingPathExtension];
            NSString* execName = nil;
            if ([fm fileExistsAtPath: plistPath]) {
                NSDictionary* plist = [NSDictionary dictionaryWithContentsOfFile: plistPath];
                if (plist) {
                    NSString* dn = plist[@"CFBundleDisplayName"];
                    if (dn && [dn length] > 0) displayName = dn;
                    else {
                        dn = plist[@"CFBundleName"];
                        if (dn && [dn length] > 0) displayName = dn;
                    }
                    execName = plist[@"CFBundleExecutable"];
                }
            }
            if (!execName || [execName length] == 0) execName = [name stringByDeletingPathExtension];

            NSString* executablePath = [macOSPath stringByAppendingPathComponent: execName];
            if (![fm isExecutableFileAtPath: executablePath]) {
                /* Try legacy .app with executable at root */
                executablePath = [bundlePath stringByAppendingPathComponent: execName];
                if (![fm isExecutableFileAtPath: executablePath])
                    continue;
            }

            // Look for PNG icons in Resources directory
            NSString* iconPath = nil;
            BOOL hasCustomIcon = NO;
            
            if ([fm fileExistsAtPath: resourcesPath isDirectory: &isDir] && isDir) {
                NSArray* resources = [fm contentsOfDirectoryAtPath: resourcesPath error: &err];
                if (resources) {
                    // Look for icon files with common naming patterns
                    NSArray* iconNames = @[@"icon.png", @"Icon.png", @"AppIcon.png", @"app-icon.png", 
                                         [NSString stringWithFormat: @"%@.png", [name stringByDeletingPathExtension]]];
                    
                    for (NSString* iconName in iconNames) {
                        NSString* possibleIcon = [resourcesPath stringByAppendingPathComponent: iconName];
                        if ([fm fileExistsAtPath: possibleIcon]) {
                            iconPath = possibleIcon;
                            hasCustomIcon = YES;
                            break;
                        }
                    }
                    
                    // If no standard icon found, look for any PNG file
                    if (!iconPath) {
                        for (NSString* resource in resources) {
                            if ([resource hasSuffix: @".png"]) {
                                iconPath = [resourcesPath stringByAppendingPathComponent: resource];
                                hasCustomIcon = YES;
                                break;
                            }
                        }
                    }
                }
            }

            AppInfo info;
            info.displayName = nsstringToStdString(displayName);
            info.bundlePath = nsstringToStdString(bundlePath);
            info.executablePath = nsstringToStdString(executablePath);
            info.iconPath = iconPath ? nsstringToStdString(iconPath) : "";
            info.hasCustomIcon = hasCustomIcon;
            result.push_back(info);
        }
    }
    return result;
}

} // namespace freeboard
