//
//  AndroidDeviceGeneralControllerUserClient.cpp
//  AndroidDeviceGeneralController
//
//  Created by Oaky on 06/02/26.
//

#include <os/log.h>

#include <DriverKit/IOLib.h>
#include <DriverKit/OSDictionary.h>
#include <DriverKit/OSString.h>

#include "AndroidDeviceGeneralControllerUserClient.h"

static const char *kPDKeys[] = {
    "PDExternalStorageRoot",
    "PDMobileEnvironmentRoot",
    "PDInternalStorageAllowed",
    "PDUartBase",
    "PDUartSize",
    "PDUartIRQ",
    "PDFbBase",
    "PDFbSize",
    "PDFbWidth",
    "PDFbHeight",
    "PDFbStride",
    nullptr
};

kern_return_t
IMPL(AndroidDeviceGeneralControllerUserClient, Start)
{
    kern_return_t ret = Start(provider, SUPERDISPATCH);
    os_log(OS_LOG_DEFAULT, "[AndroidDeviceGeneralControllerUserClient] Start: %d", ret);
    return ret;
}

kern_return_t
IMPL(AndroidDeviceGeneralControllerUserClient, CopyProperties)
{
    if (!provider) {
        return kIOReturnNotReady;
    }

    OSDictionary *dict = OSDictionary::withCapacity(12);
    if (!dict) {
        return kIOReturnNoMemory;
    }

    for (int i = 0; kPDKeys[i]; ++i) {
        const char *key = kPDKeys[i];
        OSObject *value = provider->CopyProperty(key);
        if (value) {
            dict->setObject(key, value);
            value->release();
        }
    }

    *properties = dict;
    return kIOReturnSuccess;
}
