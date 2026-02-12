/*
 * CoreDarwin - Open Source CoreFoundation Replacement
 * 
 * A fully open and hackable replacement for CoreFoundation
 * providing CFRunLoop, CFPropertyList, container types, 
 * and memory management utilities for PocketDarwin.
 */

#ifndef COREDARWIN_COREDARWIN_H
#define COREDARWIN_COREDARWIN_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Basic Types */
typedef void * CFTypeRef;
typedef void * CFAllocatorRef;
typedef struct _CFString * CFStringRef;
typedef struct _CFArray * CFArrayRef;
typedef struct _CFDictionary * CFDictionaryRef;
typedef struct _CFData * CFDataRef;
typedef struct _CFNumber * CFNumberRef;
typedef struct _CFRunLoop * CFRunLoopRef;
typedef struct _CFRunLoopTimer * CFRunLoopTimerRef;
typedef struct _CFRunLoopSource * CFRunLoopSourceRef;
typedef struct _CFRunLoopObserver * CFRunLoopObserverRef;
typedef struct _CFPropertyList * CFPropertyListRef;
typedef struct _CFError * CFErrorRef;

/* Type IDs */
typedef uint32_t CFTypeID;
typedef uint32_t CFHashCode;
typedef int32_t CFIndex;
typedef uint64_t CFOptionFlags;
typedef double CFAbsoluteTime;
typedef double CFTimeInterval;
typedef bool Boolean;
typedef CFOptionFlags CFRunLoopActivity;

/* CoreDarwin Base Type */
typedef struct _CFBase {
    void *isa;
    CFTypeID _cfTypeID;
} CFBase;

/* Callback Structures */
typedef struct {
    CFIndex version;
    void *info;
} CFRunLoopTimerContext;

typedef struct {
    CFIndex version;
    void *perform;
    void *info;
} CFRunLoopSourceCallBacks;

typedef struct {
    CFIndex version;
    void *info;
} CFRunLoopObserverContext;

typedef struct {
    CFIndex version;
    void *retain;
    void *release;
    void *copyDescription;
    void *equal;
    void *hash;
} CFArrayCallBacks;

typedef struct {
    CFIndex version;
    void *retain;
    void *release;
    void *copyDescription;
    void *equal;
    void *hash;
} CFDictionaryKeyCallBacks;

typedef struct {
    CFIndex version;
    void *retain;
    void *release;
    void *copyDescription;
    void *equal;
} CFDictionaryValueCallBacks;

/* Constants */
#define kCFAllocatorDefault NULL
#define kCFTypeArrayCallBacks NULL
#define kCFTypeDictionaryKeyCallBacks NULL
#define kCFTypeDictionaryValueCallBacks NULL
#define kCFRunLoopDefaultMode "kCFRunLoopDefaultMode"
#define kCFRunLoopCommonModes "kCFRunLoopCommonModes"

/* Number Types */
typedef uint32_t CFNumberType;
typedef uint32_t CFStringEncoding;
typedef uint32_t CFPropertyListMutabilityOptions;
typedef uint32_t CFPropertyListFormat;
typedef uint32_t CFPropertyListWriteOptions;
typedef const char * CFRunLoopMode;

enum {
    kCFNumberSInt8Type = 1,
    kCFNumberSInt16Type = 2,
    kCFNumberSInt32Type = 3,
    kCFNumberSInt64Type = 4,
    kCFNumberFloatType = 5,
    kCFNumberDoubleType = 6
};

enum {
    kCFStringEncodingUTF8 = 134217984,
    kCFStringEncodingASCII = 134217984
};

enum {
    kCFPropertyListImmutable = 0,
    kCFPropertyListMutableContainers = 1,
    kCFPropertyListMutableContainersAndLeaves = 2
};

enum {
    kCFPropertyListOpenStepFormat = 1,
    kCFPropertyListXMLFormat_v1_0 = 100,
    kCFPropertyListBinaryFormat_v1_0 = 200
};

enum {
    kCFRunLoopEntry = 1 << 0,
    kCFRunLoopBeforeTimers = 1 << 1,
    kCFRunLoopBeforeSources = 1 << 2,
    kCFRunLoopBeforeWaiting = 1 << 3,
    kCFRunLoopAfterWaiting = 1 << 4,
    kCFRunLoopExit = 1 << 5,
    kCFRunLoopAllActivities = 0x0FFFFFFF
};

/* Memory Management */
CFTypeRef CFRetain(CFTypeRef cf);
void CFRelease(CFTypeRef cf);
CFTypeID CFGetTypeID(CFTypeRef cf);
bool CFEqual(CFTypeRef cf1, CFTypeRef cf2);
CFHashCode CFHash(CFTypeRef cf);

/* String Functions */
CFStringRef CFStringCreateWithCString(const char *cStr);
CFStringRef CFStringCreateWithBytes(const uint8_t *bytes, CFIndex length, CFStringEncoding encoding, bool isExternalRepresentation);
const char *CFStringGetCStringPtr(CFStringRef str, CFStringEncoding encoding);
CFIndex CFStringGetLength(CFStringRef str);
bool CFStringGetCString(CFStringRef str, char *buffer, CFIndex bufferSize, CFStringEncoding encoding);

/* Array Functions */
CFArrayRef CFArrayCreate(CFAllocatorRef allocator, const void **values, CFIndex numValues, const CFArrayCallBacks *callBacks);
CFIndex CFArrayGetCount(CFArrayRef array);
const void *CFArrayGetValueAtIndex(CFArrayRef array, CFIndex idx);
void CFArrayAppendValue(CFArrayRef array, const void *value);
void CFArrayRemoveValueAtIndex(CFArrayRef array, CFIndex idx);

/* Dictionary Functions */
CFDictionaryRef CFDictionaryCreate(CFAllocatorRef allocator, const void **keys, const void **values, CFIndex numValues, const CFDictionaryKeyCallBacks *keyCallBacks, const CFDictionaryValueCallBacks *valueCallBacks);
CFIndex CFDictionaryGetCount(CFDictionaryRef dict);
const void *CFDictionaryGetValue(CFDictionaryRef dict, const void *key);
bool CFDictionaryGetValueIfPresent(CFDictionaryRef dict, const void *key, const void **value);
void CFDictionarySetValue(CFDictionaryRef dict, const void *key, const void *value);
void CFDictionaryRemoveValue(CFDictionaryRef dict, const void *key);

/* Data Functions */
CFDataRef CFDataCreate(CFAllocatorRef allocator, const uint8_t *bytes, CFIndex length);
CFIndex CFDataGetLength(CFDataRef data);
const uint8_t *CFDataGetBytePtr(CFDataRef data);

/* Number Functions */
CFNumberRef CFNumberCreate(CFAllocatorRef allocator, CFNumberType theType, const void *valuePtr);
bool CFNumberGetValue(CFNumberRef number, CFNumberType theType, void *valuePtr);

/* RunLoop Functions */
CFRunLoopRef CFRunLoopGetCurrent(void);
CFRunLoopRef CFRunLoopGetMain(void);
void CFRunLoopRun(void);
void CFRunLoopStop(CFRunLoopRef rl);
void CFRunLoopAddTimer(CFRunLoopRef rl, CFRunLoopTimerRef timer, CFRunLoopMode mode);
void CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source, CFRunLoopMode mode);
void CFRunLoopAddObserver(CFRunLoopRef rl, CFRunLoopObserverRef observer, CFRunLoopMode mode);

/* RunLoop Timer Functions */
CFRunLoopTimerRef CFRunLoopTimerCreate(CFAllocatorRef allocator, CFAbsoluteTime fireDate, CFTimeInterval interval, CFOptionFlags flags, CFIndex order, void (*callout)(CFRunLoopTimerRef timer, void *info), CFRunLoopTimerContext *context);

/* RunLoop Source Functions */
CFRunLoopSourceRef CFRunLoopSourceCreate(CFAllocatorRef allocator, CFIndex order, CFRunLoopSourceCallBacks *callBacks);

/* RunLoop Observer Functions */
CFRunLoopObserverRef CFRunLoopObserverCreate(CFAllocatorRef allocator, CFOptionFlags activities, Boolean repeats, CFIndex order, void (*callout)(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info), CFRunLoopObserverContext *context);

/* Property List Functions */
CFPropertyListRef CFPropertyListCreateWithData(CFAllocatorRef allocator, CFDataRef data, CFPropertyListMutabilityOptions options, CFPropertyListFormat *format, CFErrorRef *error);
CFDataRef CFPropertyListCreateData(CFAllocatorRef allocator, CFPropertyListRef propertyList, CFPropertyListFormat format, CFPropertyListWriteOptions options, CFErrorRef *error);
CFPropertyListRef CFPropertyListCreateFromXMLData(CFAllocatorRef allocator, CFDataRef xmlData, CFPropertyListMutabilityOptions options, CFPropertyListFormat *format, CFErrorRef *error);

/* Utility Functions */
CFAbsoluteTime CFAbsoluteTimeGetCurrent(void);

#ifdef __cplusplus
}
#endif

#endif /* COREDARWIN_COREDARWIN_H */
