//
//  CoreMobile.h
//  Mobile Controller
//
//  Created by Oaky on 06/02/26.
//

#ifndef CoreMobile_h
#define CoreMobile_h

#import <Foundation/Foundation.h>

FOUNDATION_EXPORT NSString *MCExternalStorageRoot(void);
FOUNDATION_EXPORT NSString *MCMobileEnvironmentRoot(void);
FOUNDATION_EXPORT BOOL MCInternalStorageAllowed(void);
FOUNDATION_EXPORT uint64_t MCUartBase(void);
FOUNDATION_EXPORT uint64_t MCUartSize(void);
FOUNDATION_EXPORT int MCUartIRQ(void);
FOUNDATION_EXPORT uint64_t MCFbBase(void);
FOUNDATION_EXPORT uint64_t MCFbSize(void);
FOUNDATION_EXPORT int MCFbWidth(void);
FOUNDATION_EXPORT int MCFbHeight(void);
FOUNDATION_EXPORT int MCFbStride(void);

typedef struct {
    uint64_t uartBase;
    uint64_t uartSize;
    int uartIRQ;
    uint64_t fbBase;
    uint64_t fbSize;
    int fbWidth;
    int fbHeight;
    int fbStride;
    BOOL internalStorageAllowed;
    __unsafe_unretained NSString *externalStorageRoot;
    __unsafe_unretained NSString *mobileEnvironmentRoot;
} MCMobileInfo;

FOUNDATION_EXPORT BOOL MCGetMobileInfo(MCMobileInfo *outInfo);

#endif /* CoreMobile_h */
