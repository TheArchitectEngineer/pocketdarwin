#ifndef PD_STDIO_H
#define PD_STDIO_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __pd_FILE FILE;

int printf(const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
int puts(const char *s);

#ifdef __cplusplus
}
#endif

#endif
