#ifndef PD_STDLIB_H
#define PD_STDLIB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t count, size_t size);
void *realloc(void *ptr, size_t size);

void abort(void) __attribute__((noreturn));

#ifdef __cplusplus
}
#endif

#endif
