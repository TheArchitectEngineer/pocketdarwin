#import "MobileEnvironmentInfo.h"
#import "CoreMobile.h"

@implementation MCMobileEnvironmentInfo

+ (instancetype)currentInfo
{
    MCMobileInfo info;
    if (!MCGetMobileInfo(&info)) {
        return nil;
    }

    MCMobileEnvironmentInfo *obj = [[MCMobileEnvironmentInfo alloc] init];
    obj->_externalStorageRoot = info.externalStorageRoot ?: @"/sdcard";
    obj->_mobileEnvironmentRoot = info.mobileEnvironmentRoot ?: @"/System/Mobile Enviroment";
    obj->_internalStorageAllowed = info.internalStorageAllowed;

    obj->_uartBase = info.uartBase;
    obj->_uartSize = info.uartSize;
    obj->_uartIRQ = info.uartIRQ;

    obj->_fbBase = info.fbBase;
    obj->_fbSize = info.fbSize;
    obj->_fbWidth = info.fbWidth;
    obj->_fbHeight = info.fbHeight;
    obj->_fbStride = info.fbStride;

    return obj;
}

@end
