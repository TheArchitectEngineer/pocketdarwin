#define _GNU_SOURCE

#include "istdlib.h"
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

// --------------------
// Memory utilities
// --------------------

// Cached system page size
static size_t page_size() {
    static size_t size = 0;
    if (size == 0) {
        size = sysconf(_SC_PAGESIZE);
    }
    return size;
}

// Round up size to nearest page boundary
static size_t align_page(size_t size) {
    size_t ps = page_size();
    return (size + ps - 1) & ~(ps - 1);
}

// Minimal malloc using mmap
void* i_malloc(size_t size) {
    size_t total = align_page(size);
    void* ptr = mmap(NULL, total, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("i_malloc mmap");
        return NULL;
    }
    return ptr;
}

// Free memory with protection
void i_free(void* ptr, size_t size) {
    if (!ptr) return;
    size_t total = align_page(size);

    mprotect(ptr, total, PROT_NONE);
    munmap(ptr, total);
}

// Zero-initialized memory
void* i_calloc(size_t count, size_t size) {
    size_t total_size = count * size;
    void* ptr = i_malloc(total_size);
    if (!ptr) return NULL;
    i_memset(ptr, 0, total_size);
    return ptr;
}

// Reallocate memory (simple version)
void* i_realloc(void* ptr, size_t old_size, size_t new_size) {
    if (!ptr) return i_malloc(new_size);
    void* new_ptr = i_malloc(new_size);
    if (!new_ptr) return NULL;
    size_t copy_size = old_size < new_size ? old_size : new_size;
    i_memcpy(new_ptr, ptr, copy_size);
    i_free(ptr, old_size);
    return new_ptr;
}

// Simple memset
void i_memset(void* ptr, int value, size_t size) {
    unsigned char* p = ptr;
    for (size_t i = 0; i < size; i++) p[i] = (unsigned char)value;
}

// Simple memcpy
void i_memcpy(void* dest, const void* src, size_t size) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    for (size_t i = 0; i < size; i++) d[i] = s[i];
}

// --------------------
// String utilities
// --------------------

// strlen
size_t i_strlen(const char* s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

// strcpy
char* i_strcpy(char* dest, const char* src) {
    size_t i = 0;
    while ((dest[i] = src[i])) i++;
    return dest;
}

// strcmp
int i_strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

// --------------------
// Output utilities
// --------------------

// Minimal writeln: string + newline
void i_writeln(const char* str) {
    if (!str) return;
    size_t len = i_strlen(str);
    write(1, str, len);
    write(1, "\n", 1);
}