#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <stdio.h>
#include <string.h>

static io_service_t find_adgc_service(void)
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
    service = IOServiceGetMatchingService(kIOMasterPortDefault, match);
    return service;
}

static void print_cf_string(const char *label, CFTypeRef value)
{
    if (!value || CFGetTypeID(value) != CFStringGetTypeID()) {
        return;
    }
    char buf[256];
    if (CFStringGetCString((CFStringRef)value, buf, sizeof(buf), kCFStringEncodingUTF8)) {
        printf("%s: %s\n", label, buf);
    }
}

static void print_cf_number(const char *label, CFTypeRef value)
{
    if (!value || CFGetTypeID(value) != CFNumberGetTypeID()) {
        return;
    }
    long long v = 0;
    if (CFNumberGetValue((CFNumberRef)value, kCFNumberLongLongType, &v)) {
        printf("%s: %lld\n", label, v);
    }
}

static void print_cf_bool(const char *label, CFTypeRef value)
{
    if (!value || CFGetTypeID(value) != CFBooleanGetTypeID()) {
        return;
    }
    Boolean b = CFBooleanGetValue((CFBooleanRef)value);
    printf("%s: %s\n", label, b ? "true" : "false");
}

static void print_json_string(const char *key, const char *value)
{
    if (!value) {
        return;
    }
    printf("\"%s\":\"", key);
    for (const char *p = value; *p; ++p) {
        if (*p == '\"' || *p == '\\\\') {
            putchar('\\\\');
        }
        putchar(*p);
    }
    printf("\"");
}

static void print_json_number(const char *key, long long value)
{
    printf("\"%s\":%lld", key, value);
}

static void print_json_bool(const char *key, int value)
{
    printf("\"%s\":%s", key, value ? "true" : "false");
}

int main(int argc, char **argv)
{
    int json = 0;
    if (argc > 1 && strcmp(argv[1], "--json") == 0) {
        json = 1;
    }

    io_service_t service = find_adgc_service();
    if (service == MACH_PORT_NULL) {
        fprintf(stderr, "AndroidDeviceGeneralController service not found.\n");
        return 1;
    }

    CFTypeRef sd = IORegistryEntryCreateCFProperty(service, CFSTR("PDExternalStorageRoot"), kCFAllocatorDefault, 0);
    CFTypeRef mobile = IORegistryEntryCreateCFProperty(service, CFSTR("PDMobileEnvironmentRoot"), kCFAllocatorDefault, 0);
    CFTypeRef allow = IORegistryEntryCreateCFProperty(service, CFSTR("PDInternalStorageAllowed"), kCFAllocatorDefault, 0);

    CFTypeRef uart_base = IORegistryEntryCreateCFProperty(service, CFSTR("PDUartBase"), kCFAllocatorDefault, 0);
    CFTypeRef uart_size = IORegistryEntryCreateCFProperty(service, CFSTR("PDUartSize"), kCFAllocatorDefault, 0);
    CFTypeRef uart_irq = IORegistryEntryCreateCFProperty(service, CFSTR("PDUartIRQ"), kCFAllocatorDefault, 0);

    CFTypeRef fb_base = IORegistryEntryCreateCFProperty(service, CFSTR("PDFbBase"), kCFAllocatorDefault, 0);
    CFTypeRef fb_size = IORegistryEntryCreateCFProperty(service, CFSTR("PDFbSize"), kCFAllocatorDefault, 0);
    CFTypeRef fb_w = IORegistryEntryCreateCFProperty(service, CFSTR("PDFbWidth"), kCFAllocatorDefault, 0);
    CFTypeRef fb_h = IORegistryEntryCreateCFProperty(service, CFSTR("PDFbHeight"), kCFAllocatorDefault, 0);
    CFTypeRef fb_stride = IORegistryEntryCreateCFProperty(service, CFSTR("PDFbStride"), kCFAllocatorDefault, 0);

    if (!json) {
        print_cf_string("ExternalStorageRoot", sd);
        print_cf_string("MobileEnvironmentRoot", mobile);
        print_cf_bool("InternalStorageAllowed", allow);

        print_cf_number("UartBase", uart_base);
        print_cf_number("UartSize", uart_size);
        print_cf_number("UartIRQ", uart_irq);

        print_cf_number("FbBase", fb_base);
        print_cf_number("FbSize", fb_size);
        print_cf_number("FbWidth", fb_w);
        print_cf_number("FbHeight", fb_h);
        print_cf_number("FbStride", fb_stride);
    } else {
        printf("{");
        int first = 1;

        if (sd && CFGetTypeID(sd) == CFStringGetTypeID()) {
            char buf[256];
            if (CFStringGetCString((CFStringRef)sd, buf, sizeof(buf), kCFStringEncodingUTF8)) {
                if (!first) printf(",");
                print_json_string("ExternalStorageRoot", buf);
                first = 0;
            }
        }
        if (mobile && CFGetTypeID(mobile) == CFStringGetTypeID()) {
            char buf[256];
            if (CFStringGetCString((CFStringRef)mobile, buf, sizeof(buf), kCFStringEncodingUTF8)) {
                if (!first) printf(",");
                print_json_string("MobileEnvironmentRoot", buf);
                first = 0;
            }
        }
        if (allow && CFGetTypeID(allow) == CFBooleanGetTypeID()) {
            if (!first) printf(",");
            print_json_bool("InternalStorageAllowed", CFBooleanGetValue((CFBooleanRef)allow));
            first = 0;
        }

        if (uart_base && CFGetTypeID(uart_base) == CFNumberGetTypeID()) {
            long long v = 0;
            CFNumberGetValue((CFNumberRef)uart_base, kCFNumberLongLongType, &v);
            if (!first) printf(",");
            print_json_number("UartBase", v);
            first = 0;
        }
        if (uart_size && CFGetTypeID(uart_size) == CFNumberGetTypeID()) {
            long long v = 0;
            CFNumberGetValue((CFNumberRef)uart_size, kCFNumberLongLongType, &v);
            if (!first) printf(",");
            print_json_number("UartSize", v);
            first = 0;
        }
        if (uart_irq && CFGetTypeID(uart_irq) == CFNumberGetTypeID()) {
            long long v = 0;
            CFNumberGetValue((CFNumberRef)uart_irq, kCFNumberLongLongType, &v);
            if (!first) printf(",");
            print_json_number("UartIRQ", v);
            first = 0;
        }

        if (fb_base && CFGetTypeID(fb_base) == CFNumberGetTypeID()) {
            long long v = 0;
            CFNumberGetValue((CFNumberRef)fb_base, kCFNumberLongLongType, &v);
            if (!first) printf(",");
            print_json_number("FbBase", v);
            first = 0;
        }
        if (fb_size && CFGetTypeID(fb_size) == CFNumberGetTypeID()) {
            long long v = 0;
            CFNumberGetValue((CFNumberRef)fb_size, kCFNumberLongLongType, &v);
            if (!first) printf(",");
            print_json_number("FbSize", v);
            first = 0;
        }
        if (fb_w && CFGetTypeID(fb_w) == CFNumberGetTypeID()) {
            long long v = 0;
            CFNumberGetValue((CFNumberRef)fb_w, kCFNumberLongLongType, &v);
            if (!first) printf(",");
            print_json_number("FbWidth", v);
            first = 0;
        }
        if (fb_h && CFGetTypeID(fb_h) == CFNumberGetTypeID()) {
            long long v = 0;
            CFNumberGetValue((CFNumberRef)fb_h, kCFNumberLongLongType, &v);
            if (!first) printf(",");
            print_json_number("FbHeight", v);
            first = 0;
        }
        if (fb_stride && CFGetTypeID(fb_stride) == CFNumberGetTypeID()) {
            long long v = 0;
            CFNumberGetValue((CFNumberRef)fb_stride, kCFNumberLongLongType, &v);
            if (!first) printf(",");
            print_json_number("FbStride", v);
            first = 0;
        }

        printf("}\n");
    }

    if (sd) CFRelease(sd);
    if (mobile) CFRelease(mobile);
    if (allow) CFRelease(allow);
    if (uart_base) CFRelease(uart_base);
    if (uart_size) CFRelease(uart_size);
    if (uart_irq) CFRelease(uart_irq);
    if (fb_base) CFRelease(fb_base);
    if (fb_size) CFRelease(fb_size);
    if (fb_w) CFRelease(fb_w);
    if (fb_h) CFRelease(fb_h);
    if (fb_stride) CFRelease(fb_stride);

    IOObjectRelease(service);
    return 0;
}
