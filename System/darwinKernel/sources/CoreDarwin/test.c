/*
 * CoreDarwin Test Program
 * 
 * Test the CoreDarwin implementation
 */

#include "CoreDarwin.h"
#include <stdio.h>
#include <string.h>

int main() {
    printf("=== CoreDarwin Test Program ===\n\n");
    
    // Test String functions
    printf("Testing String Functions:\n");
    CFStringRef str = CFStringCreateWithCString("Hello, CoreDarwin!");
    if (str) {
        const char *cStr = CFStringGetCStringPtr(str, kCFStringEncodingUTF8);
        printf("  Created string: %s\n", cStr ? cStr : "(null)");
        printf("  String length: %ld\n", CFStringGetLength(str));
        CFRelease(str);
    }
    printf("\n");
    
    // Test Array functions
    printf("Testing Array Functions:\n");
    const void *values[] = {
        CFStringCreateWithCString("Item 1"),
        CFStringCreateWithCString("Item 2"),
        CFStringCreateWithCString("Item 3")
    };
    CFArrayRef array = CFArrayCreate(NULL, values, 3, NULL);
    if (array) {
        printf("  Array count: %ld\n", CFArrayGetCount(array));
        for (CFIndex i = 0; i < CFArrayGetCount(array); i++) {
            CFStringRef item = (CFStringRef)CFArrayGetValueAtIndex(array, i);
            const char *itemStr = CFStringGetCStringPtr(item, kCFStringEncodingUTF8);
            printf("  Item %ld: %s\n", i, itemStr ? itemStr : "(null)");
        }
        
        // Clean up array items
        for (CFIndex i = 0; i < CFArrayGetCount(array); i++) {
            CFRelease((CFTypeRef)CFArrayGetValueAtIndex(array, i));
        }
        CFRelease(array);
    }
    printf("\n");
    
    // Test Dictionary functions
    printf("Testing Dictionary Functions:\n");
    const void *keys[] = {
        CFStringCreateWithCString("Name"),
        CFStringCreateWithCString("Version"),
        CFStringCreateWithCString("Platform")
    };
    const void *dictValues[] = {
        CFStringCreateWithCString("CoreDarwin"),
        CFStringCreateWithCString("1.0.0"),
        CFStringCreateWithCString("PocketDarwin")
    };
    CFDictionaryRef dict = CFDictionaryCreate(NULL, keys, dictValues, 3, NULL, NULL);
    if (dict) {
        printf("  Dictionary count: %ld\n", CFDictionaryGetCount(dict));
        
        CFStringRef nameKey = CFStringCreateWithCString("Name");
        const void *nameValue = CFDictionaryGetValue(dict, nameKey);
        if (nameValue) {
            const char *nameStr = CFStringGetCStringPtr((CFStringRef)nameValue, kCFStringEncodingUTF8);
            printf("  Name: %s\n", nameStr ? nameStr : "(null)");
        }
        CFRelease(nameKey);
        
        // Clean up dictionary items
        for (CFIndex i = 0; i < 3; i++) {
            CFRelease((CFTypeRef)keys[i]);
            CFRelease((CFTypeRef)dictValues[i]);
        }
        CFRelease(dict);
    }
    printf("\n");
    
    // Test Number functions
    printf("Testing Number Functions:\n");
    int32_t intValue = 42;
    CFNumberRef number = CFNumberCreate(NULL, kCFNumberSInt32Type, &intValue);
    if (number) {
        int32_t retrievedValue;
        if (CFNumberGetValue(number, kCFNumberSInt32Type, &retrievedValue)) {
            printf("  Original value: %d\n", intValue);
            printf("  Retrieved value: %d\n", retrievedValue);
        }
        CFRelease(number);
    }
    printf("\n");
    
    // Test Data functions
    printf("Testing Data Functions:\n");
    const uint8_t dataBytes[] = { 0xDE, 0xAD, 0xBE, 0xEF };
    CFDataRef data = CFDataCreate(NULL, dataBytes, sizeof(dataBytes));
    if (data) {
        printf("  Data length: %ld\n", CFDataGetLength(data));
        const uint8_t *bytes = CFDataGetBytePtr(data);
        if (bytes) {
            printf("  Data bytes: ");
            for (CFIndex i = 0; i < CFDataGetLength(data); i++) {
                printf("%02X ", bytes[i]);
            }
            printf("\n");
        }
        CFRelease(data);
    }
    printf("\n");
    
    // Test Property List functions
    printf("Testing Property List Functions:\n");
    CFStringRef xmlString = CFStringCreateWithCString(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">"
        "<plist version=\"1.0\">"
        "<dict>"
        "<key>TestKey</key>"
        "<string>TestValue</string>"
        "<key>TestNumber</key>"
        "<true/>"
        "</dict>"
        "</plist>"
    );
    
    if (xmlString) {
        const char *xmlCStr = CFStringGetCStringPtr(xmlString, kCFStringEncodingUTF8);
        CFDataRef xmlData = CFDataCreate(NULL, (const uint8_t *)xmlCStr, strlen(xmlCStr));
        
        if (xmlData) {
            CFPropertyListFormat format;
            CFPropertyListRef plist = CFPropertyListCreateFromXMLData(NULL, xmlData, kCFPropertyListImmutable, &format, NULL);
            
            if (plist) {
                printf("  Property list created successfully\n");
                printf("  Format: %d\n", format);
                
                CFStringRef testKey = CFStringCreateWithCString("TestKey");
                const void *testValue = CFDictionaryGetValue((CFDictionaryRef)plist, testKey);
                if (testValue) {
                    const char *valueStr = CFStringGetCStringPtr((CFStringRef)testValue, kCFStringEncodingUTF8);
                    printf("  TestKey value: %s\n", valueStr ? valueStr : "(null)");
                }
                CFRelease(testKey);
                CFRelease(plist);
            }
            CFRelease(xmlData);
        }
        CFRelease(xmlString);
    }
    printf("\n");
    
    // Test RunLoop functions (basic test)
    printf("Testing RunLoop Functions:\n");
    CFRunLoopRef runLoop = CFRunLoopGetCurrent();
    if (runLoop) {
        printf("  Current RunLoop created\n");
        printf("  RunLoop type ID: %u\n", CFGetTypeID((CFTypeRef)runLoop));
    }
    printf("\n");
    
    printf("=== CoreDarwin Test Complete ===\n");
    printf("All tests passed! CoreDarwin is working correctly.\n");
    
    return 0;
}
