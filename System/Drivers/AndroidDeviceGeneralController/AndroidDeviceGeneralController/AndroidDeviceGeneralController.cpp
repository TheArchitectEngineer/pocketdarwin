//
//  AndroidDeviceGeneralController.cpp
//  AndroidDeviceGeneralController
//
//  Created by Oaky on 06/02/26.
//

#include <os/log.h>

#include <DriverKit/IOUserServer.h>
#include <DriverKit/IOLib.h>
#include <DriverKit/OSBoolean.h>
#include <DriverKit/OSData.h>
#include <DriverKit/OSDictionary.h>
#include <DriverKit/OSNumber.h>
#include <DriverKit/OSString.h>

#include "AndroidDeviceGeneralController.h"
#include "DeviceTreeParsing.hpp"
#include "Framebuffer.hpp"

static char *copy_boot_args(IOService *service)
{
    if (!service) {
        return nullptr;
    }

    OSObject *bootObj = service->CopyProperty("boot-args");
    if (!bootObj) {
        return nullptr;
    }

    char *out = nullptr;
    if (OSString *str = OSDynamicCast(OSString, bootObj)) {
        const char *cstr = str->getCStringNoCopy();
        if (cstr) {
            size_t len = strlen(cstr);
            out = (char *)IOMalloc(len + 1);
            if (out) {
                memcpy(out, cstr, len);
                out[len] = '\0';
            }
        }
    } else if (OSData *data = OSDynamicCast(OSData, bootObj)) {
        const void *bytes = data->getBytesNoCopy();
        size_t len = data->getLength();
        if (bytes && len > 0) {
            out = (char *)IOMalloc(len + 1);
            if (out) {
                memcpy(out, bytes, len);
                out[len] = '\0';
            }
        }
    }

    bootObj->release();
    return out;
}

static void publish_properties(IOService *service, const ADGC_BootArgsInfo &info)
{
    if (!service) {
        return;
    }

    OSDictionary *props = OSDictionary::withCapacity(12);
    if (!props) {
        return;
    }

    const char *sd_root = info.external_storage_root[0] ? info.external_storage_root : ADGC_DefaultExternalStorageRoot();
    const char *mobile_root = info.mobile_environment_root[0] ? info.mobile_environment_root : ADGC_DefaultMobileEnvironmentRoot();
    props->setObject("PDExternalStorageRoot", OSString::withCString(sd_root));
    props->setObject("PDMobileEnvironmentRoot", OSString::withCString(mobile_root));
    props->setObject("PDInternalStorageAllowed", OSBoolean::withBoolean(info.internal_storage_allowed != 0));

    if (info.uart_base) {
        props->setObject("PDUartBase", OSNumber::withNumber(info.uart_base, 64));
    }
    if (info.uart_size) {
        props->setObject("PDUartSize", OSNumber::withNumber(info.uart_size, 64));
    }
    if (info.uart_irq >= 0) {
        props->setObject("PDUartIRQ", OSNumber::withNumber((uint64_t)info.uart_irq, 32));
    }

    if (info.fb_base) {
        props->setObject("PDFbBase", OSNumber::withNumber(info.fb_base, 64));
    }
    if (info.fb_size) {
        props->setObject("PDFbSize", OSNumber::withNumber(info.fb_size, 64));
    }
    if (info.fb_width) {
        props->setObject("PDFbWidth", OSNumber::withNumber((uint64_t)info.fb_width, 32));
    }
    if (info.fb_height) {
        props->setObject("PDFbHeight", OSNumber::withNumber((uint64_t)info.fb_height, 32));
    }
    if (info.fb_stride) {
        props->setObject("PDFbStride", OSNumber::withNumber((uint64_t)info.fb_stride, 32));
    }

    service->SetProperties(props);
    props->release();
}

kern_return_t
IMPL(AndroidDeviceGeneralController, Start)
{
    kern_return_t ret;
    ret = Start(provider, SUPERDISPATCH);
    os_log(OS_LOG_DEFAULT, "[AndroidDeviceGeneralController] Start: %d", ret);

    ADGC_BootArgsInfo info;
    char *bootArgs = copy_boot_args(provider);
    if (bootArgs) {
        ADGC_ParseBootArgs(bootArgs, &info);
        IOFree(bootArgs, strlen(bootArgs) + 1);
    } else {
        ADGC_ParseBootArgs(nullptr, &info);
    }

    publish_properties(this, info);

    return ret;
}
