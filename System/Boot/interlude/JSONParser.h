#ifndef POCKETDARWIN_JSONPARSER_H
#define POCKETDARWIN_JSONPARSER_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JSONType;

typedef struct JSONValue JSONValue;

typedef struct {
    char *key;
    JSONValue *value;
} JSONPair;

typedef struct {
    JSONValue **items;
    size_t count;
} JSONArray;

typedef struct {
    JSONPair *pairs;
    size_t count;
} JSONObject;

struct JSONValue {
    JSONType type;
    union {
        bool boolean;
        double number;
        char *string;
        JSONArray array;
        JSONObject object;
    } as;
};

JSONValue *json_parse_string(const char *input, char **error);
JSONValue *json_parse_file(const char *path, char **error);
void json_free(JSONValue *value);

JSONValue *json_object_get(JSONValue *object, const char *key);
JSONValue *json_get_path(JSONValue *root, const char *path);

const char *json_get_string(JSONValue *value);
bool json_get_bool(JSONValue *value, bool default_value);
double json_get_number(JSONValue *value, double default_value);

// OCMobileConfig helpers
char *oc_mobile_get_kernel_path(JSONValue *root);
char *oc_mobile_get_external_storage_root(JSONValue *root);
char *oc_mobile_get_mobile_environment_root(JSONValue *root);
int oc_mobile_get_internal_storage_allowed(JSONValue *root, int default_value);

#ifdef __cplusplus
}
#endif

#endif
