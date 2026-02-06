//
//  DeviceTreeParsing.cpp
//  AndroidDeviceGeneralController
//
//  Created by Oaky on 06/02/26.
//

#include "DeviceTreeParsing.hpp"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static void init_info(ADGC_BootArgsInfo *info) {
    if (!info) {
        return;
    }
    memset(info, 0, sizeof(*info));
    info->uart_irq = -1;
    info->internal_storage_allowed = 0;
}

static const char *skip_spaces(const char *p) {
    while (p && *p && isspace((unsigned char)*p)) {
        ++p;
    }
    return p;
}

static const char *next_token(const char *p) {
    while (p && *p && !isspace((unsigned char)*p)) {
        ++p;
    }
    return p;
}

static bool parse_uint64(const char *value, uint64_t *out) {
    if (!value || !out) {
        return false;
    }
    char *end = NULL;
    uint64_t v = strtoull(value, &end, 0);
    if (end == value) {
        return false;
    }
    *out = v;
    return true;
}

static bool parse_int(const char *value, int *out) {
    if (!value || !out) {
        return false;
    }
    char *end = NULL;
    long v = strtol(value, &end, 0);
    if (end == value) {
        return false;
    }
    *out = (int)v;
    return true;
}

static void apply_kv(ADGC_BootArgsInfo *info, const char *key, const char *value) {
    if (!info || !key || !value) {
        return;
    }

    if (strcmp(key, "pd.uart.base") == 0) {
        parse_uint64(value, &info->uart_base);
    } else if (strcmp(key, "pd.uart.size") == 0) {
        parse_uint64(value, &info->uart_size);
    } else if (strcmp(key, "pd.uart.irq") == 0) {
        parse_int(value, &info->uart_irq);
    } else if (strcmp(key, "pd.fb.base") == 0) {
        parse_uint64(value, &info->fb_base);
    } else if (strcmp(key, "pd.fb.size") == 0) {
        parse_uint64(value, &info->fb_size);
    } else if (strcmp(key, "pd.fb.w") == 0) {
        parse_int(value, &info->fb_width);
    } else if (strcmp(key, "pd.fb.h") == 0) {
        parse_int(value, &info->fb_height);
    } else if (strcmp(key, "pd.fb.stride") == 0) {
        parse_int(value, &info->fb_stride);
    } else if (strcmp(key, "pd.sdroot") == 0) {
        strncpy(info->external_storage_root, value, sizeof(info->external_storage_root) - 1);
        info->external_storage_root[sizeof(info->external_storage_root) - 1] = '\0';
    } else if (strcmp(key, "pd.mobileenv") == 0) {
        strncpy(info->mobile_environment_root, value, sizeof(info->mobile_environment_root) - 1);
        info->mobile_environment_root[sizeof(info->mobile_environment_root) - 1] = '\0';
    } else if (strcmp(key, "pd.allow_internal") == 0) {
        parse_int(value, &info->internal_storage_allowed);
    }
}

bool ADGC_ParseBootArgs(const char *boot_args, ADGC_BootArgsInfo *out_info) {
    if (!out_info) {
        return false;
    }
    init_info(out_info);

    if (!boot_args) {
        return false;
    }

    const char *p = boot_args;
    while (*p) {
        p = skip_spaces(p);
        if (!*p) {
            break;
        }
        const char *end = next_token(p);
        size_t len = (size_t)(end - p);
        if (len == 0) {
            break;
        }

        const char *eq = (const char *)memchr(p, '=', len);
        if (eq) {
            size_t key_len = (size_t)(eq - p);
            size_t val_len = len - key_len - 1;
            if (key_len > 0 && val_len > 0) {
                char key[64];
                char val[64];
                if (key_len < sizeof(key) && val_len < sizeof(val)) {
                    memcpy(key, p, key_len);
                    key[key_len] = '\0';
                    memcpy(val, eq + 1, val_len);
                    val[val_len] = '\0';
                    apply_kv(out_info, key, val);
                }
            }
        }

        p = end;
    }

    return true;
}

const char *ADGC_DefaultExternalStorageRoot(void) {
    return "/sdcard";
}

const char *ADGC_DefaultMobileEnvironmentRoot(void) {
    return "/System/Mobile Enviroment";
}
