#import "CoreMobile.h"
#import <IOKit/IOKitLib.h>

static io_service_t MCFindService(void)
{
    io_service_t service = IOServiceGetMatchingService(kIOMasterPortDefault,
                                                       IOServiceMatching("AndroidDeviceGeneralController"));
    if (service != MACH_PORT_NULL) {
        return service;
    }

    CFMutableDictionaryRef match = IOServiceMatching("IOUserService");
    if (!match) {
        return MACH_PORT_NULL;
    }
    CFDictionarySetValue(match, CFSTR("IOUserClass"), CFSTR("AndroidDeviceGeneralController"));
    return IOServiceGetMatchingService(kIOMasterPortDefault, match);
}

static NSString *MCReadStringProperty(CFStringRef key, NSString *fallback)
{
    io_service_t service = MCFindService();
    if (service == MACH_PORT_NULL) {
        return fallback;
    }

    CFTypeRef value = IORegistryEntryCreateCFProperty(service, key, kCFAllocatorDefault, 0);
    IOObjectRelease(service);
    if (!value) {
        return fallback;
    }

    NSString *result = fallback;
    if (CFGetTypeID(value) == CFStringGetTypeID()) {
        result = [(__bridge NSString *)value copy];
    }
    CFRelease(value);
    return result;
}

static BOOL MCReadBoolProperty(CFStringRef key, BOOL fallback)
{
    io_service_t service = MCFindService();
    if (service == MACH_PORT_NULL) {
        return fallback;
    }

    CFTypeRef value = IORegistryEntryCreateCFProperty(service, key, kCFAllocatorDefault, 0);
    IOObjectRelease(service);
    if (!value) {
        return fallback;
    }

    BOOL result = fallback;
    if (CFGetTypeID(value) == CFBooleanGetTypeID()) {
        result = CFBooleanGetValue((CFBooleanRef)value);
    }
    CFRelease(value);
    return result;
}

static uint64_t MCReadUInt64Property(CFStringRef key, uint64_t fallback)
{
    io_service_t service = MCFindService();
    if (service == MACH_PORT_NULL) {
        return fallback;
    }

    CFTypeRef value = IORegistryEntryCreateCFProperty(service, key, kCFAllocatorDefault, 0);
    IOObjectRelease(service);
    if (!value) {
        return fallback;
    }

    uint64_t result = fallback;
    if (CFGetTypeID(value) == CFNumberGetTypeID()) {
        CFNumberGetValue((CFNumberRef)value, kCFNumberSInt64Type, &result);
    }
    CFRelease(value);
    return result;
}

static int MCReadIntProperty(CFStringRef key, int fallback)
{
    io_service_t service = MCFindService();
    if (service == MACH_PORT_NULL) {
        return fallback;
    }

    CFTypeRef value = IORegistryEntryCreateCFProperty(service, key, kCFAllocatorDefault, 0);
    IOObjectRelease(service);
    if (!value) {
        return fallback;
    }

    int result = fallback;
    if (CFGetTypeID(value) == CFNumberGetTypeID()) {
        CFNumberGetValue((CFNumberRef)value, kCFNumberIntType, &result);
    }
    CFRelease(value);
    return result;
}

NSString *MCExternalStorageRoot(void)
{
    return MCReadStringProperty(CFSTR("PDExternalStorageRoot"), @"/sdcard");
}

NSString *MCMobileEnvironmentRoot(void)
{
    return MCReadStringProperty(CFSTR("PDMobileEnvironmentRoot"), @"/System/Mobile Enviroment");
}

BOOL MCInternalStorageAllowed(void)
{
    return MCReadBoolProperty(CFSTR("PDInternalStorageAllowed"), NO);
}

uint64_t MCUartBase(void)
{
    return MCReadUInt64Property(CFSTR("PDUartBase"), 0);
}

uint64_t MCUartSize(void)
{
    return MCReadUInt64Property(CFSTR("PDUartSize"), 0);
}

int MCUartIRQ(void)
{
    return MCReadIntProperty(CFSTR("PDUartIRQ"), -1);
}

uint64_t MCFbBase(void)
{
    return MCReadUInt64Property(CFSTR("PDFbBase"), 0);
}

uint64_t MCFbSize(void)
{
    return MCReadUInt64Property(CFSTR("PDFbSize"), 0);
}

int MCFbWidth(void)
{
    return MCReadIntProperty(CFSTR("PDFbWidth"), 0);
}

int MCFbHeight(void)
{
    return MCReadIntProperty(CFSTR("PDFbHeight"), 0);
}

int MCFbStride(void)
{
    return MCReadIntProperty(CFSTR("PDFbStride"), 0);
}

static NSString *MCReadStringPropertyFromService(io_service_t service, CFStringRef key, NSString *fallback)
{
    if (service == MACH_PORT_NULL) {
        return fallback;
    }
    CFTypeRef value = IORegistryEntryCreateCFProperty(service, key, kCFAllocatorDefault, 0);
    if (!value) {
        return fallback;
    }
    NSString *result = fallback;
    if (CFGetTypeID(value) == CFStringGetTypeID()) {
        result = [(__bridge NSString *)value copy];
    }
    CFRelease(value);
    return result;
}

static uint64_t MCReadUInt64PropertyFromService(io_service_t service, CFStringRef key, uint64_t fallback)
{
    if (service == MACH_PORT_NULL) {
        return fallback;
    }
    CFTypeRef value = IORegistryEntryCreateCFProperty(service, key, kCFAllocatorDefault, 0);
    if (!value) {
        return fallback;
    }
    uint64_t result = fallback;
    if (CFGetTypeID(value) == CFNumberGetTypeID()) {
        CFNumberGetValue((CFNumberRef)value, kCFNumberSInt64Type, &result);
    }
    CFRelease(value);
    return result;
}

static int MCReadIntPropertyFromService(io_service_t service, CFStringRef key, int fallback)
{
    if (service == MACH_PORT_NULL) {
        return fallback;
    }
    CFTypeRef value = IORegistryEntryCreateCFProperty(service, key, kCFAllocatorDefault, 0);
    if (!value) {
        return fallback;
    }
    int result = fallback;
    if (CFGetTypeID(value) == CFNumberGetTypeID()) {
        CFNumberGetValue((CFNumberRef)value, kCFNumberIntType, &result);
    }
    CFRelease(value);
    return result;
}

static BOOL MCReadBoolPropertyFromService(io_service_t service, CFStringRef key, BOOL fallback)
{
    if (service == MACH_PORT_NULL) {
        return fallback;
    }
    CFTypeRef value = IORegistryEntryCreateCFProperty(service, key, kCFAllocatorDefault, 0);
    if (!value) {
        return fallback;
    }
    BOOL result = fallback;
    if (CFGetTypeID(value) == CFBooleanGetTypeID()) {
        result = CFBooleanGetValue((CFBooleanRef)value);
    }
    CFRelease(value);
    return result;
}

BOOL MCGetMobileInfo(MCMobileInfo *outInfo)
{
    if (!outInfo) {
        return NO;
    }

    io_service_t service = MCFindService();
    if (service == MACH_PORT_NULL) {
        return NO;
    }

    outInfo->externalStorageRoot = MCReadStringPropertyFromService(service, CFSTR("PDExternalStorageRoot"), @"/sdcard");
    outInfo->mobileEnvironmentRoot = MCReadStringPropertyFromService(service, CFSTR("PDMobileEnvironmentRoot"), @"/System/Mobile Enviroment");
    outInfo->internalStorageAllowed = MCReadBoolPropertyFromService(service, CFSTR("PDInternalStorageAllowed"), NO);

    outInfo->uartBase = MCReadUInt64PropertyFromService(service, CFSTR("PDUartBase"), 0);
    outInfo->uartSize = MCReadUInt64PropertyFromService(service, CFSTR("PDUartSize"), 0);
    outInfo->uartIRQ = MCReadIntPropertyFromService(service, CFSTR("PDUartIRQ"), -1);

    outInfo->fbBase = MCReadUInt64PropertyFromService(service, CFSTR("PDFbBase"), 0);
    outInfo->fbSize = MCReadUInt64PropertyFromService(service, CFSTR("PDFbSize"), 0);
    outInfo->fbWidth = MCReadIntPropertyFromService(service, CFSTR("PDFbWidth"), 0);
    outInfo->fbHeight = MCReadIntPropertyFromService(service, CFSTR("PDFbHeight"), 0);
    outInfo->fbStride = MCReadIntPropertyFromService(service, CFSTR("PDFbStride"), 0);

    IOObjectRelease(service);
    return YES;
}
