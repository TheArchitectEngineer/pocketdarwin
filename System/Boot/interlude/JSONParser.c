#include "JSONParser.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *src;
    size_t pos;
    size_t len;
} JSONCursor;

static void set_error(char **error, const char *message) {
    if (!error) {
        return;
    }
    if (*error) {
        return;
    }
    *error = strdup(message);
}

static void skip_ws(JSONCursor *cur) {
    while (cur->pos < cur->len && isspace((unsigned char)cur->src[cur->pos])) {
        cur->pos++;
    }
}

static int peek(JSONCursor *cur) {
    if (cur->pos >= cur->len) {
        return -1;
    }
    return (unsigned char)cur->src[cur->pos];
}

static int advance(JSONCursor *cur) {
    if (cur->pos >= cur->len) {
        return -1;
    }
    return (unsigned char)cur->src[cur->pos++];
}

static JSONValue *parse_value(JSONCursor *cur, char **error);

static JSONValue *alloc_value(JSONType type) {
    JSONValue *val = (JSONValue *)calloc(1, sizeof(JSONValue));
    if (!val) {
        return NULL;
    }
    val->type = type;
    return val;
}

static JSONValue *parse_string(JSONCursor *cur, char **error) {
    if (advance(cur) != '"') {
        set_error(error, "Expected string");
        return NULL;
    }

    size_t cap = 32;
    size_t len = 0;
    char *buf = (char *)malloc(cap);
    if (!buf) {
        set_error(error, "Out of memory");
        return NULL;
    }

    while (cur->pos < cur->len) {
        int ch = advance(cur);
        if (ch == '"') {
            break;
        }
        if (ch == '\\') {
            int esc = advance(cur);
            switch (esc) {
                case '"': ch = '"'; break;
                case '\\': ch = '\\'; break;
                case '/': ch = '/'; break;
                case 'b': ch = '\b'; break;
                case 'f': ch = '\f'; break;
                case 'n': ch = '\n'; break;
                case 'r': ch = '\r'; break;
                case 't': ch = '\t'; break;
                case 'u':
                    // Minimal unicode handling: consume 4 hex digits and replace with '?' for now
                    for (int i = 0; i < 4 && cur->pos < cur->len; ++i) {
                        advance(cur);
                    }
                    ch = '?';
                    break;
                default:
                    set_error(error, "Invalid escape sequence");
                    free(buf);
                    return NULL;
            }
        }

        if (len + 1 >= cap) {
            cap *= 2;
            char *tmp = (char *)realloc(buf, cap);
            if (!tmp) {
                set_error(error, "Out of memory");
                free(buf);
                return NULL;
            }
            buf = tmp;
        }
        buf[len++] = (char)ch;
    }

    buf[len] = '\0';
    JSONValue *val = alloc_value(JSON_STRING);
    if (!val) {
        set_error(error, "Out of memory");
        free(buf);
        return NULL;
    }
    val->as.string = buf;
    return val;
}

static JSONValue *parse_number(JSONCursor *cur, char **error) {
    size_t start = cur->pos;
    int ch = peek(cur);
    if (ch == '-') {
        advance(cur);
    }
    while (isdigit(peek(cur))) {
        advance(cur);
    }
    if (peek(cur) == '.') {
        advance(cur);
        while (isdigit(peek(cur))) {
            advance(cur);
        }
    }
    ch = peek(cur);
    if (ch == 'e' || ch == 'E') {
        advance(cur);
        ch = peek(cur);
        if (ch == '+' || ch == '-') {
            advance(cur);
        }
        while (isdigit(peek(cur))) {
            advance(cur);
        }
    }

    size_t end = cur->pos;
    char tmp[64];
    size_t len = end - start;
    if (len >= sizeof(tmp)) {
        set_error(error, "Number too long");
        return NULL;
    }
    memcpy(tmp, cur->src + start, len);
    tmp[len] = '\0';

    char *endptr = NULL;
    errno = 0;
    double val = strtod(tmp, &endptr);
    if (errno != 0 || endptr == tmp) {
        set_error(error, "Invalid number");
        return NULL;
    }

    JSONValue *out = alloc_value(JSON_NUMBER);
    if (!out) {
        set_error(error, "Out of memory");
        return NULL;
    }
    out->as.number = val;
    return out;
}

static JSONValue *parse_array(JSONCursor *cur, char **error) {
    if (advance(cur) != '[') {
        set_error(error, "Expected array");
        return NULL;
    }

    JSONValue *val = alloc_value(JSON_ARRAY);
    if (!val) {
        set_error(error, "Out of memory");
        return NULL;
    }

    skip_ws(cur);
    if (peek(cur) == ']') {
        advance(cur);
        return val;
    }

    while (cur->pos < cur->len) {
        skip_ws(cur);
        JSONValue *item = parse_value(cur, error);
        if (!item) {
            json_free(val);
            return NULL;
        }

        JSONValue **new_items = (JSONValue **)realloc(
            val->as.array.items,
            sizeof(JSONValue *) * (val->as.array.count + 1));
        if (!new_items) {
            set_error(error, "Out of memory");
            json_free(item);
            json_free(val);
            return NULL;
        }
        val->as.array.items = new_items;
        val->as.array.items[val->as.array.count++] = item;

        skip_ws(cur);
        int ch = advance(cur);
        if (ch == ']') {
            return val;
        }
        if (ch != ',') {
            set_error(error, "Expected ',' or ']' in array");
            json_free(val);
            return NULL;
        }
    }

    set_error(error, "Unterminated array");
    json_free(val);
    return NULL;
}

static JSONValue *parse_object(JSONCursor *cur, char **error) {
    if (advance(cur) != '{') {
        set_error(error, "Expected object");
        return NULL;
    }

    JSONValue *val = alloc_value(JSON_OBJECT);
    if (!val) {
        set_error(error, "Out of memory");
        return NULL;
    }

    skip_ws(cur);
    if (peek(cur) == '}') {
        advance(cur);
        return val;
    }

    while (cur->pos < cur->len) {
        skip_ws(cur);
        if (peek(cur) != '"') {
            set_error(error, "Expected string key");
            json_free(val);
            return NULL;
        }
        JSONValue *key_val = parse_string(cur, error);
        if (!key_val) {
            json_free(val);
            return NULL;
        }
        skip_ws(cur);
        if (advance(cur) != ':') {
            set_error(error, "Expected ':' after key");
            json_free(key_val);
            json_free(val);
            return NULL;
        }

        skip_ws(cur);
        JSONValue *item = parse_value(cur, error);
        if (!item) {
            json_free(key_val);
            json_free(val);
            return NULL;
        }

        JSONPair *new_pairs = (JSONPair *)realloc(
            val->as.object.pairs,
            sizeof(JSONPair) * (val->as.object.count + 1));
        if (!new_pairs) {
            set_error(error, "Out of memory");
            json_free(key_val);
            json_free(item);
            json_free(val);
            return NULL;
        }
        val->as.object.pairs = new_pairs;
        JSONPair *pair = &val->as.object.pairs[val->as.object.count++];
        pair->key = key_val->as.string;
        pair->value = item;
        free(key_val);

        skip_ws(cur);
        int ch = advance(cur);
        if (ch == '}') {
            return val;
        }
        if (ch != ',') {
            set_error(error, "Expected ',' or '}' in object");
            json_free(val);
            return NULL;
        }
    }

    set_error(error, "Unterminated object");
    json_free(val);
    return NULL;
}

static JSONValue *parse_keyword(JSONCursor *cur, char **error, const char *keyword, JSONType type) {
    size_t len = strlen(keyword);
    if (cur->pos + len > cur->len) {
        set_error(error, "Unexpected end of input");
        return NULL;
    }
    if (strncmp(cur->src + cur->pos, keyword, len) != 0) {
        set_error(error, "Unexpected token");
        return NULL;
    }
    cur->pos += len;
    JSONValue *val = alloc_value(type);
    if (!val) {
        set_error(error, "Out of memory");
        return NULL;
    }
    if (type == JSON_BOOL) {
        val->as.boolean = (keyword[0] == 't');
    }
    return val;
}

static JSONValue *parse_value(JSONCursor *cur, char **error) {
    skip_ws(cur);
    int ch = peek(cur);
    if (ch == -1) {
        set_error(error, "Unexpected end of input");
        return NULL;
    }
    if (ch == '"') {
        return parse_string(cur, error);
    }
    if (ch == '{') {
        return parse_object(cur, error);
    }
    if (ch == '[') {
        return parse_array(cur, error);
    }
    if (ch == 't') {
        return parse_keyword(cur, error, "true", JSON_BOOL);
    }
    if (ch == 'f') {
        return parse_keyword(cur, error, "false", JSON_BOOL);
    }
    if (ch == 'n') {
        return parse_keyword(cur, error, "null", JSON_NULL);
    }
    if (ch == '-' || isdigit(ch)) {
        return parse_number(cur, error);
    }
    set_error(error, "Unexpected token");
    return NULL;
}

JSONValue *json_parse_string(const char *input, char **error) {
    if (!input) {
        set_error(error, "Input is null");
        return NULL;
    }
    JSONCursor cur = { input, 0, strlen(input) };
    JSONValue *root = parse_value(&cur, error);
    if (!root) {
        return NULL;
    }
    skip_ws(&cur);
    if (cur.pos != cur.len) {
        json_free(root);
        set_error(error, "Trailing data after JSON value");
        return NULL;
    }
    return root;
}

JSONValue *json_parse_file(const char *path, char **error) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        set_error(error, "Failed to open file");
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        set_error(error, "Failed to seek file");
        return NULL;
    }
    long len = ftell(f);
    if (len < 0) {
        fclose(f);
        set_error(error, "Failed to read file size");
        return NULL;
    }
    if (fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        set_error(error, "Failed to seek file");
        return NULL;
    }

    char *buf = (char *)malloc((size_t)len + 1);
    if (!buf) {
        fclose(f);
        set_error(error, "Out of memory");
        return NULL;
    }
    size_t read_len = fread(buf, 1, (size_t)len, f);
    fclose(f);
    buf[read_len] = '\0';

    JSONValue *root = json_parse_string(buf, error);
    free(buf);
    return root;
}

void json_free(JSONValue *value) {
    if (!value) {
        return;
    }
    switch (value->type) {
        case JSON_STRING:
            free(value->as.string);
            break;
        case JSON_ARRAY:
            for (size_t i = 0; i < value->as.array.count; ++i) {
                json_free(value->as.array.items[i]);
            }
            free(value->as.array.items);
            break;
        case JSON_OBJECT:
            for (size_t i = 0; i < value->as.object.count; ++i) {
                free(value->as.object.pairs[i].key);
                json_free(value->as.object.pairs[i].value);
            }
            free(value->as.object.pairs);
            break;
        default:
            break;
    }
    free(value);
}

JSONValue *json_object_get(JSONValue *object, const char *key) {
    if (!object || object->type != JSON_OBJECT || !key) {
        return NULL;
    }
    for (size_t i = 0; i < object->as.object.count; ++i) {
        if (strcmp(object->as.object.pairs[i].key, key) == 0) {
            return object->as.object.pairs[i].value;
        }
    }
    return NULL;
}

static JSONValue *json_array_get(JSONValue *array, size_t index) {
    if (!array || array->type != JSON_ARRAY) {
        return NULL;
    }
    if (index >= array->as.array.count) {
        return NULL;
    }
    return array->as.array.items[index];
}

JSONValue *json_get_path(JSONValue *root, const char *path) {
    if (!root || !path) {
        return NULL;
    }

    JSONValue *current = root;
    const char *p = path;
    while (*p) {
        if (*p == '.') {
            p++;
            continue;
        }

        if (*p == '[') {
            p++;
            char *endptr = NULL;
            long idx = strtol(p, &endptr, 10);
            if (endptr == p || *endptr != ']') {
                return NULL;
            }
            current = json_array_get(current, (size_t)idx);
            if (!current) {
                return NULL;
            }
            p = endptr + 1;
            continue;
        }

        const char *start = p;
        while (*p && *p != '.' && *p != '[') {
            p++;
        }
        size_t len = (size_t)(p - start);
        if (len == 0) {
            return NULL;
        }
        char key[128];
        if (len >= sizeof(key)) {
            return NULL;
        }
        memcpy(key, start, len);
        key[len] = '\0';
        current = json_object_get(current, key);
        if (!current) {
            return NULL;
        }
    }

    return current;
}

const char *json_get_string(JSONValue *value) {
    if (!value || value->type != JSON_STRING) {
        return NULL;
    }
    return value->as.string;
}

bool json_get_bool(JSONValue *value, bool default_value) {
    if (!value || value->type != JSON_BOOL) {
        return default_value;
    }
    return value->as.boolean;
}

double json_get_number(JSONValue *value, double default_value) {
    if (!value || value->type != JSON_NUMBER) {
        return default_value;
    }
    return value->as.number;
}

static char *dup_string(const char *value) {
    if (!value) {
        return NULL;
    }
    size_t len = strlen(value);
    char *out = (char *)malloc(len + 1);
    if (!out) {
        return NULL;
    }
    memcpy(out, value, len);
    out[len] = '\0';
    return out;
}

char *oc_mobile_get_kernel_path(JSONValue *root) {
    if (!root) {
        return NULL;
    }

    const char *keys[] = {
        "darwinKernelPath",
        "darwin_kernel_path",
        "darwinKernel",
        "darwin_kernel",
        NULL
    };

    for (int i = 0; keys[i]; ++i) {
        JSONValue *val = json_object_get(root, keys[i]);
        const char *str = json_get_string(val);
        if (str) {
            return dup_string(str);
        }
    }

    JSONValue *boot = json_object_get(root, "Boot");
    if (boot && boot->type == JSON_OBJECT) {
        for (int i = 0; keys[i]; ++i) {
            JSONValue *val = json_object_get(boot, keys[i]);
            const char *str = json_get_string(val);
            if (str) {
                return dup_string(str);
            }
        }
        JSONValue *kernel = json_object_get(boot, "Kernel");
        const char *str = json_get_string(kernel);
        if (str) {
            return dup_string(str);
        }
    }

    JSONValue *kernel = json_object_get(root, "Kernel");
    if (kernel && kernel->type == JSON_OBJECT) {
        for (int i = 0; keys[i]; ++i) {
            JSONValue *val = json_object_get(kernel, keys[i]);
            const char *str = json_get_string(val);
            if (str) {
                return dup_string(str);
            }
        }
        JSONValue *path = json_object_get(kernel, "Path");
        const char *str = json_get_string(path);
        if (str) {
            return dup_string(str);
        }
    }

    return NULL;
}

static char *get_string_key(JSONValue *root, const char *primary, const char *alt, const char *section) {
    if (!root) {
        return NULL;
    }
    if (primary) {
        JSONValue *val = json_object_get(root, primary);
        const char *str = json_get_string(val);
        if (str) {
            return dup_string(str);
        }
    }
    if (alt) {
        JSONValue *val = json_object_get(root, alt);
        const char *str = json_get_string(val);
        if (str) {
            return dup_string(str);
        }
    }
    if (section) {
        JSONValue *sec = json_object_get(root, section);
        if (sec && sec->type == JSON_OBJECT) {
            if (primary) {
                JSONValue *val = json_object_get(sec, primary);
                const char *str = json_get_string(val);
                if (str) {
                    return dup_string(str);
                }
            }
            if (alt) {
                JSONValue *val = json_object_get(sec, alt);
                const char *str = json_get_string(val);
                if (str) {
                    return dup_string(str);
                }
            }
        }
    }
    return NULL;
}

char *oc_mobile_get_external_storage_root(JSONValue *root) {
    return get_string_key(root, "ExternalStorageRoot", "external_storage_root", "Platform");
}

char *oc_mobile_get_mobile_environment_root(JSONValue *root) {
    return get_string_key(root, "MobileEnvironmentRoot", "mobile_environment_root", "Platform");
}

int oc_mobile_get_internal_storage_allowed(JSONValue *root, int default_value) {
    if (!root) {
        return default_value;
    }
    JSONValue *val = json_object_get(root, "InternalStorageAllowed");
    if (val && val->type == JSON_BOOL) {
        return val->as.boolean ? 1 : 0;
    }
    if (val && val->type == JSON_NUMBER) {
        return val->as.number != 0.0 ? 1 : 0;
    }
    JSONValue *sec = json_object_get(root, "Platform");
    if (sec && sec->type == JSON_OBJECT) {
        val = json_object_get(sec, "InternalStorageAllowed");
        if (val && val->type == JSON_BOOL) {
            return val->as.boolean ? 1 : 0;
        }
        if (val && val->type == JSON_NUMBER) {
            return val->as.number != 0.0 ? 1 : 0;
        }
    }
    return default_value;
}
