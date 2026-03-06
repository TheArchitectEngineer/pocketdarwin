#ifndef ISTDLIB_H
#define ISTDLIB_H

#include <stddef.h>

// Memory
void* i_malloc(size_t size);
void  i_free(void* ptr, size_t size);
void* i_calloc(size_t count, size_t size);
void* i_realloc(void* ptr, size_t old_size, size_t new_size);

// Memory utilities
void i_memset(void* ptr, int value, size_t size);
void i_memcpy(void* dest, const void* src, size_t size);

// Strings
size_t i_strlen(const char* s);
char*  i_strcpy(char* dest, const char* src);
int    i_strcmp(const char* a, const char* b);

// Output
void i_writeln(const char* str);

#endif // ISTDLIB_H