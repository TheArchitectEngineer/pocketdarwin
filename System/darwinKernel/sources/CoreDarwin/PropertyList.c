/*
 * CoreDarwin - Property List Implementation
 * 
 * CFPropertyList implementation for PocketDarwin launchd
 */

#include "CoreDarwin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Property List Parser Implementation */
typedef struct _PListParser {
    const char *data;
    size_t length;
    size_t position;
} PListParser;

/* XML Property List Parsing */
static void skipWhitespace(PListParser *parser) {
    while (parser->position < parser->length && 
           isspace(parser->data[parser->position])) {
        parser->position++;
    }
}

static bool expectString(PListParser *parser, const char *expected) {
    skipWhitespace(parser);
    size_t len = strlen(expected);
    
    if (parser->position + len > parser->length) return false;
    
    if (strncmp(&parser->data[parser->position], expected, len) == 0) {
        parser->position += len;
        return true;
    }
    
    return false;
}

static char *parseString(PListParser *parser) {
    skipWhitespace(parser);
    
    if (parser->position >= parser->length || parser->data[parser->position] != '"') {
        return NULL;
    }
    
    parser->position++; // Skip opening quote
    
    char *result = malloc(1024); // Initial buffer
    size_t pos = 0;
    
    while (parser->position < parser->length && parser->data[parser->position] != '"') {
        if (parser->data[parser->position] == '\\' && parser->position + 1 < parser->length) {
            // Handle escape sequences
            parser->position++;
            char escape = parser->data[parser->position++];
            switch (escape) {
                case 'n': result[pos++] = '\n'; break;
                case 't': result[pos++] = '\t'; break;
                case 'r': result[pos++] = '\r'; break;
                case '\\': result[pos++] = '\\'; break;
                case '"': result[pos++] = '"'; break;
                default: result[pos++] = escape; break;
            }
        } else {
            result[pos++] = parser->data[parser->position++];
        }
        
        if (pos >= 1023) break; // Prevent buffer overflow
    }
    
    result[pos] = '\0';
    
    if (parser->position < parser->length && parser->data[parser->position] == '"') {
        parser->position++; // Skip closing quote
    }
    
    return result;
}

static CFTypeRef parseValue(PListParser *parser);

static CFDictionaryRef parseDictionary(PListParser *parser) {
    if (!expectString(parser, "<dict>")) return NULL;
    
    CFDictionary *dict = malloc(sizeof(CFDictionary));
    if (!dict) return NULL;
    
    dict->base.base._cfTypeID = kCFDictionaryTypeID;
    dict->base.retainCount = 1;
    pthread_mutex_init(&dict->base.lock, NULL);
    
    dict->capacity = 16;
    dict->count = 0;
    dict->keys = malloc(sizeof(void *) * 16);
    dict->values = malloc(sizeof(void *) * 16);
    
    while (parser->position < parser->length) {
        skipWhitespace(parser);
        
        if (expectString(parser, "</dict>")) {
            break;
        }
        
        if (expectString(parser, "<key>")) {
            char *key = parseString(parser);
            if (key) {
                CFTypeRef value = parseValue(parser);
                if (value && dict->count < dict->capacity) {
                    dict->keys[dict->count] = CFStringCreateWithCString(key);
                    dict->values[dict->count] = value;
                    dict->count++;
                }
                free(key);
            }
        } else {
            // Skip unknown element
            parser->position++;
        }
    }
    
    return (CFDictionaryRef)dict;
}

static CFArrayRef parseArray(PListParser *parser) {
    if (!expectString(parser, "<array>")) return NULL;
    
    CFArray *array = malloc(sizeof(CFArray));
    if (!array) return NULL;
    
    array->base.base._cfTypeID = kCFArrayTypeID;
    array->base.retainCount = 1;
    pthread_mutex_init(&array->base.lock, NULL);
    
    array->capacity = 16;
    array->count = 0;
    array->values = malloc(sizeof(void *) * 16);
    
    while (parser->position < parser->length) {
        skipWhitespace(parser);
        
        if (expectString(parser, "</array>")) {
            break;
        }
        
        CFTypeRef value = parseValue(parser);
        if (value && array->count < array->capacity) {
            array->values[array->count] = value;
            array->count++;
        }
    }
    
    return (CFArrayRef)array;
}

static CFTypeRef parseValue(PListParser *parser) {
    skipWhitespace(parser);
    
    if (expectString(parser, "<string>")) {
        char *str = parseString(parser);
        CFStringRef result = CFStringCreateWithCString(str);
        free(str);
        return (CFTypeRef)result;
    }
    
    if (expectString(parser, "<dict>")) {
        parser->position -= 6; // Back up to <dict>
        return (CFTypeRef)parseDictionary(parser);
    }
    
    if (expectString(parser, "<array>")) {
        parser->position -= 6; // Back up to <array>
        return (CFTypeRef)parseArray(parser);
    }
    
    if (expectString(parser, "<true/>")) {
        bool trueValue = true;
        return (CFTypeRef)CFNumberCreate(NULL, kCFNumberSInt8Type, &trueValue);
    }
    
    if (expectString(parser, "<false/>")) {
        bool falseValue = false;
        return (CFTypeRef)CFNumberCreate(NULL, kCFNumberSInt8Type, &falseValue);
    }
    
    // Skip unknown content
    parser->position++;
    return NULL;
}

/* Property List Creation Functions */
CFPropertyListRef CFPropertyListCreateWithData(CFAllocatorRef allocator, CFDataRef data, CFPropertyListMutabilityOptions options, CFPropertyListFormat *format, CFErrorRef *error) {
    if (!data) return NULL;
    
    const uint8_t *bytes = CFDataGetBytePtr(data);
    CFIndex length = CFDataGetLength(data);
    
    if (!bytes || length <= 0) return NULL;
    
    PListParser parser;
    parser.data = (const char *)bytes;
    parser.length = length;
    parser.position = 0;
    
    skipWhitespace(&parser);
    
    if (expectString(&parser, "<?xml") || expectString(&parser, "<!DOCTYPE")) {
        // XML Property List
        if (format) *format = kCFPropertyListXMLFormat_v1_0;
        
        // Skip to <plist>
        while (parser.position < parser.length && 
               !expectString(&parser, "<plist")) {
            parser.position++;
        }
        
        // Skip version attribute
        while (parser.position < parser.length && 
               parser.data[parser.position] != '>') {
            parser.position++;
        }
        parser.position++; // Skip '>'
        
        return parseValue(&parser);
    }
    
    return NULL;
}

CFPropertyListRef CFPropertyListCreateFromXMLData(CFAllocatorRef allocator, CFDataRef xmlData, CFPropertyListMutabilityOptions options, CFPropertyListFormat *format, CFErrorRef *error) {
    return CFPropertyListCreateWithData(allocator, xmlData, options, format, error);
}

/* Property List Serialization */
static void escapeString(const char *input, char *output, size_t outputSize) {
    size_t pos = 0;
    while (*input && pos < outputSize - 1) {
        switch (*input) {
            case '&':
                if (pos < outputSize - 6) {
                    strcpy(&output[pos], "&amp;");
                    pos += 5;
                }
                break;
            case '<':
                if (pos < outputSize - 5) {
                    strcpy(&output[pos], "&lt;");
                    pos += 4;
                }
                break;
            case '>':
                if (pos < outputSize - 5) {
                    strcpy(&output[pos], "&gt;");
                    pos += 4;
                }
                break;
            case '"':
                if (pos < outputSize - 7) {
                    strcpy(&output[pos], "&quot;");
                    pos += 6;
                }
                break;
            default:
                output[pos++] = *input;
                break;
        }
        input++;
    }
    output[pos] = '\0';
}

static void serializeValue(CFTypeRef value, char *output, size_t outputSize, int indent);

static void serializeDictionary(CFDictionaryRef dict, char *output, size_t outputSize, int indent) {
    size_t pos = 0;
    pos += snprintf(&output[pos], outputSize - pos, "%*s<dict>\n", indent, "");
    
    for (CFIndex i = 0; i < CFDictionaryGetCount(dict); i++) {
        CFStringRef key = (CFStringRef)CFArrayGetValueAtIndex((CFArrayRef)dict->keys, i);
        CFTypeRef value = CFDictionaryGetValue(dict, key);
        
        char keyStr[1024];
        CFStringGetCString(key, keyStr, sizeof(keyStr), kCFStringEncodingUTF8);
        
        char escapedKey[2048];
        escapeString(keyStr, escapedKey, sizeof(escapedKey));
        
        pos += snprintf(&output[pos], outputSize - pos, "%*s<key>%s</key>\n", indent + 2, "", escapedKey);
        
        serializeValue(value, &output[pos], outputSize - pos, indent + 2);
        
        // Move position forward (this is a simplified approach)
        pos += strlen(&output[pos]);
    }
    
    pos += snprintf(&output[pos], outputSize - pos, "%*s</dict>\n", indent, "");
}

static void serializeArray(CFArrayRef array, char *output, size_t outputSize, int indent) {
    size_t pos = 0;
    pos += snprintf(&output[pos], outputSize - pos, "%*s<array>\n", indent, "");
    
    for (CFIndex i = 0; i < CFArrayGetCount(array); i++) {
        CFTypeRef value = CFArrayGetValueAtIndex(array, i);
        serializeValue(value, &output[pos], outputSize - pos, indent + 2);
        pos += strlen(&output[pos]);
    }
    
    pos += snprintf(&output[pos], outputSize - pos, "%*s</array>\n", indent, "");
}

static void serializeValue(CFTypeRef value, char *output, size_t outputSize, int indent) {
    if (!value) return;
    
    CFTypeID typeID = CFGetTypeID(value);
    
    if (typeID == kCFStringTypeID) {
        char str[1024];
        CFStringGetCString((CFStringRef)value, str, sizeof(str), kCFStringEncodingUTF8);
        
        char escaped[2048];
        escapeString(str, escaped, sizeof(escaped));
        
        snprintf(output, outputSize, "%*s<string>%s</string>\n", indent, "", escaped);
    }
    else if (typeID == kCFDictionaryTypeID) {
        serializeDictionary((CFDictionaryRef)value, output, outputSize, indent);
    }
    else if (typeID == kCFArrayTypeID) {
        serializeArray((CFArrayRef)value, output, outputSize, indent);
    }
    else if (typeID == kCFNumberTypeID) {
        bool boolValue;
        if (CFNumberGetValue((CFNumberRef)value, kCFNumberSInt8Type, &boolValue)) {
            snprintf(output, outputSize, "%*s<%s/>\n", indent, "", boolValue ? "true" : "false");
        }
    }
}

CFDataRef CFPropertyListCreateData(CFAllocatorRef allocator, CFPropertyListRef propertyList, CFPropertyListFormat format, CFPropertyListWriteOptions options, CFErrorRef *error) {
    if (!propertyList) return NULL;
    
    char buffer[65536];
    size_t pos = 0;
    
    // XML header
    pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n");
    pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "<plist version=\"1.0\">\n");
    
    // Serialize the property list
    serializeValue(propertyList, &buffer[pos], sizeof(buffer) - pos, 0);
    
    // Close plist
    pos += strlen(&buffer[pos]);
    pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "</plist>\n");
    
    return CFDataCreate(allocator, (const uint8_t *)buffer, strlen(buffer));
}
