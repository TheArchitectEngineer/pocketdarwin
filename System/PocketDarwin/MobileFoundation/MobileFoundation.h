//
//  MobileFoundation.h
//  MobileFoundation
//
//  Created by Oaky on 07/03/26.
//

#import <Foundation/Foundation.h>

//! Project version number for MobileFoundation.
FOUNDATION_EXPORT double MobileFoundationVersionNumber;

//! Project version string for MobileFoundation.
FOUNDATION_EXPORT const unsigned char MobileFoundationVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <MobileFoundation/PublicHeader.h>

//! Check the reported iPhoneOS version (needs to be greater or equal than 2.0)
//!

#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)

if (SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(@"2.0")) {
    #import <MobileFoundation/Mobile_Controller.h>
    #import <MobileFoundation/NSObject+OpenBundleObject.h>
}

#import <CoreFoundation/CoreFoundation.h>
#import <CoreFoundation/CFBundle.h>

#define MobileFoundationBundle CFBundleGetBundleWithIdentifier(CFSTR("dev.oaky.darwin.MobileFoundation"))

@interface MobileBundle : NSBundle
+ (NSBundle *)mainBundle;
@end

@property (nonatomic, readonly) NSString *mobileFoundationVersion;
@property (nonatomic, readonly) NSString *mobileFoundationBuild;

@interface MobileFoundation : NSObject
//
@end

#define MobileFoundationVersion [NSBundle.mainBundle mobileFoundationVersion]
#define MobileFoundationBuild [NSBundle.mainBundle mobileFoundationBuild]

@protocol iPhoneSystemInfoForiTunes <NSObject>

//

@end

const unsigned char *iPhoneSystemInfoForiTunesGetSerialNumber(void);

void iPhoneSystemInfoForiTunesGetDeviceInfo(uint64_t *outDeviceType, uint64_t *outDeviceSubtype, uint64_t *outDeviceVariant);

void GenerateSerialNumber(char *buffer, size_t bufferSize);
