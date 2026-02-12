/*
 * CoreDarwin - Core Foundation Implementation
 * 
 * Memory management and type system implementation
 */

#include "CoreDarwin.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/* Type ID Management */
static CFTypeID nextTypeID = 1;
static pthread_mutex_t typeIDMutex = PTHREAD_MUTEX_INITIALIZER;

/* CoreDarwin Type IDs */
const CFTypeID kCFStringTypeID = 1;
const CFTypeID kCFArrayTypeID = 2;
const CFTypeID kCFDictionaryTypeID = 3;
const CFTypeID kCFDataTypeID = 4;
const CFTypeID kCFNumberTypeID = 5;
const CFTypeID kCFRunLoopTypeID = 6;

/* Base Type Implementation */
typedef struct _CFObject {
    CFBase base;
    int32_t retainCount;
    pthread_mutex_t lock;
} CFObject;

/* Memory Management Implementation */
CFTypeRef CFRetain(CFTypeRef cf) {
    if (!cf) return NULL;
    
    CFObject *obj = (CFObject *)cf;
    pthread_mutex_lock(&obj->lock);
    obj->retainCount++;
    pthread_mutex_unlock(&obj->lock);
    
    return cf;
}

void CFRelease(CFTypeRef cf) {
    if (!cf) return;
    
    CFObject *obj = (CFObject *)cf;
    pthread_mutex_lock(&obj->lock);
    obj->retainCount--;
    bool shouldFree = (obj->retainCount <= 0);
    pthread_mutex_unlock(&obj->lock);
    
    if (shouldFree) {
        free(obj);
    }
}

CFTypeID CFGetTypeID(CFTypeRef cf) {
    if (!cf) return 0;
    return ((CFBase *)cf)->_cfTypeID;
}

bool CFEqual(CFTypeRef cf1, CFTypeRef cf2) {
    if (cf1 == cf2) return true;
    if (!cf1 || !cf2) return false;
    
    CFTypeID type1 = CFGetTypeID(cf1);
    CFTypeID type2 = CFGetTypeID(cf2);
    
    return (type1 == type2);
}

CFHashCode CFHash(CFTypeRef cf) {
    return (CFHashCode)(uintptr_t)cf;
}

/* String Implementation */
typedef struct _CFString {
    CFObject base;
    char *cString;
    CFIndex length;
} CFString;

CFStringRef CFStringCreateWithCString(const char *cStr) {
    if (!cStr) return NULL;
    
    CFString *str = malloc(sizeof(CFString));
    if (!str) return NULL;
    
    str->base.base._cfTypeID = kCFStringTypeID;
    str->base.retainCount = 1;
    pthread_mutex_init(&str->base.lock, NULL);
    
    str->length = strlen(cStr);
    str->cString = strdup(cStr);
    
    return (CFStringRef)str;
}

CFStringRef CFStringCreateWithBytes(const uint8_t *bytes, CFIndex length, CFStringEncoding encoding, bool isExternalRepresentation) {
    if (!bytes || length <= 0) return NULL;
    
    CFString *str = malloc(sizeof(CFString));
    if (!str) return NULL;
    
    str->base.base._cfTypeID = kCFStringTypeID;
    str->base.retainCount = 1;
    pthread_mutex_init(&str->base.lock, NULL);
    
    str->length = length;
    str->cString = malloc(length + 1);
    if (str->cString) {
        memcpy(str->cString, bytes, length);
        str->cString[length] = '\0';
    }
    
    return (CFStringRef)str;
}

const char *CFStringGetCStringPtr(CFStringRef str, CFStringEncoding encoding) {
    if (!str) return NULL;
    return ((CFString *)str)->cString;
}

CFIndex CFStringGetLength(CFStringRef str) {
    if (!str) return 0;
    return ((CFString *)str)->length;
}

bool CFStringGetCString(CFStringRef str, char *buffer, CFIndex bufferSize, CFStringEncoding encoding) {
    if (!str || !buffer || bufferSize <= 0) return false;
    
    const char *cStr = CFStringGetCStringPtr(str, encoding);
    if (!cStr) return false;
    
    strncpy(buffer, cStr, bufferSize - 1);
    buffer[bufferSize - 1] = '\0';
    return true;
}

/* Array Implementation */
typedef struct _CFArray {
    CFObject base;
    void **values;
    CFIndex count;
    CFIndex capacity;
} CFArray;

CFArrayRef CFArrayCreate(CFAllocatorRef allocator, const void **values, CFIndex numValues, const CFArrayCallBacks *callBacks) {
    if (!values || numValues <= 0) return NULL;
    
    CFArray *array = malloc(sizeof(CFArray));
    if (!array) return NULL;
    
    array->base.base._cfTypeID = kCFArrayTypeID;
    array->base.retainCount = 1;
    pthread_mutex_init(&array->base.lock, NULL);
    
    array->capacity = numValues;
    array->count = numValues;
    array->values = malloc(sizeof(void *) * numValues);
    
    if (array->values) {
        for (CFIndex i = 0; i < numValues; i++) {
            array->values[i] = (void *)values[i];
        }
    }
    
    return (CFArrayRef)array;
}

CFIndex CFArrayGetCount(CFArrayRef array) {
    if (!array) return 0;
    return ((CFArray *)array)->count;
}

const void *CFArrayGetValueAtIndex(CFArrayRef array, CFIndex idx) {
    if (!array || idx < 0 || idx >= ((CFArray *)array)->count) return NULL;
    return ((CFArray *)array)->values[idx];
}

/* Dictionary Implementation */
typedef struct _CFDictionary {
    CFObject base;
    void **keys;
    void **values;
    CFIndex count;
    CFIndex capacity;
} CFDictionary;

CFDictionaryRef CFDictionaryCreate(CFAllocatorRef allocator, const void **keys, const void **values, CFIndex numValues, const CFDictionaryKeyCallBacks *keyCallBacks, const CFDictionaryValueCallBacks *valueCallBacks) {
    if (!keys || !values || numValues <= 0) return NULL;
    
    CFDictionary *dict = malloc(sizeof(CFDictionary));
    if (!dict) return NULL;
    
    dict->base.base._cfTypeID = kCFDictionaryTypeID;
    dict->base.retainCount = 1;
    pthread_mutex_init(&dict->base.lock, NULL);
    
    dict->capacity = numValues;
    dict->count = numValues;
    dict->keys = malloc(sizeof(void *) * numValues);
    dict->values = malloc(sizeof(void *) * numValues);
    
    if (dict->keys && dict->values) {
        for (CFIndex i = 0; i < numValues; i++) {
            dict->keys[i] = (void *)keys[i];
            dict->values[i] = (void *)values[i];
        }
    }
    
    return (CFDictionaryRef)dict;
}

CFIndex CFDictionaryGetCount(CFDictionaryRef dict) {
    if (!dict) return 0;
    return ((CFDictionary *)dict)->count;
}

const void *CFDictionaryGetValue(CFDictionaryRef dict, const void *key) {
    if (!dict || !key) return NULL;
    
    CFDictionary *d = (CFDictionary *)dict;
    for (CFIndex i = 0; i < d->count; i++) {
        if (d->keys[i] == key) {
            return d->values[i];
        }
    }
    return NULL;
}

bool CFDictionaryGetValueIfPresent(CFDictionaryRef dict, const void *key, const void **value) {
    const void *val = CFDictionaryGetValue(dict, key);
    if (value) *value = val;
    return (val != NULL);
}

/* Data Implementation */
typedef struct _CFData {
    CFObject base;
    uint8_t *bytes;
    CFIndex length;
} CFData;

CFDataRef CFDataCreate(CFAllocatorRef allocator, const uint8_t *bytes, CFIndex length) {
    if (!bytes || length <= 0) return NULL;
    
    CFData *data = malloc(sizeof(CFData));
    if (!data) return NULL;
    
    data->base.base._cfTypeID = kCFDataTypeID;
    data->base.retainCount = 1;
    pthread_mutex_init(&data->base.lock, NULL);
    
    data->length = length;
    data->bytes = malloc(length);
    if (data->bytes) {
        memcpy(data->bytes, bytes, length);
    }
    
    return (CFDataRef)data;
}

CFIndex CFDataGetLength(CFDataRef data) {
    if (!data) return 0;
    return ((CFData *)data)->length;
}

const uint8_t *CFDataGetBytePtr(CFDataRef data) {
    if (!data) return NULL;
    return ((CFData *)data)->bytes;
}

/* Number Implementation */
typedef struct _CFNumber {
    CFObject base;
    CFNumberType type;
    union {
        int8_t sInt8Value;
        int16_t sInt16Value;
        int32_t sInt32Value;
        int64_t sInt64Value;
        float floatValue;
        double doubleValue;
    } value;
} CFNumber;

CFNumberRef CFNumberCreate(CFAllocatorRef allocator, CFNumberType theType, const void *valuePtr) {
    if (!valuePtr) return NULL;
    
    CFNumber *number = malloc(sizeof(CFNumber));
    if (!number) return NULL;
    
    number->base.base._cfTypeID = kCFNumberTypeID;
    number->base.retainCount = 1;
    pthread_mutex_init(&number->base.lock, NULL);
    
    number->type = theType;
    
    switch (theType) {
        case kCFNumberSInt8Type:
            number->value.sInt8Value = *(int8_t *)valuePtr;
            break;
        case kCFNumberSInt16Type:
            number->value.sInt16Value = *(int16_t *)valuePtr;
            break;
        case kCFNumberSInt32Type:
            number->value.sInt32Value = *(int32_t *)valuePtr;
            break;
        case kCFNumberSInt64Type:
            number->value.sInt64Value = *(int64_t *)valuePtr;
            break;
        case kCFNumberFloatType:
            number->value.floatValue = *(float *)valuePtr;
            break;
        case kCFNumberDoubleType:
            number->value.doubleValue = *(double *)valuePtr;
            break;
        default:
            number->value.sInt32Value = *(int32_t *)valuePtr;
            break;
    }
    
    return (CFNumberRef)number;
}

bool CFNumberGetValue(CFNumberRef number, CFNumberType theType, void *valuePtr) {
    if (!number || !valuePtr) return false;
    
    CFNumber *num = (CFNumber *)number;
    
    switch (theType) {
        case kCFNumberSInt8Type:
            *(int8_t *)valuePtr = num->value.sInt8Value;
            break;
        case kCFNumberSInt16Type:
            *(int16_t *)valuePtr = num->value.sInt16Value;
            break;
        case kCFNumberSInt32Type:
            *(int32_t *)valuePtr = num->value.sInt32Value;
            break;
        case kCFNumberSInt64Type:
            *(int64_t *)valuePtr = num->value.sInt64Value;
            break;
        case kCFNumberFloatType:
            *(float *)valuePtr = num->value.floatValue;
            break;
        case kCFNumberDoubleType:
            *(double *)valuePtr = num->value.doubleValue;
            break;
        default:
            return false;
    }
    
    return true;
}
