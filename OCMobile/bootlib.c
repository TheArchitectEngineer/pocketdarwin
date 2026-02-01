#include "bootstd.h"
#include <stdarg.h>

// Memory functions
void *memcpy(void *dst, const void *src, size_t n) {
    u8 *d = dst;
    const u8 *s = src;
    while (n--) *d++ = *s++;
    return dst;
}

void *memset(void *dst, int val, size_t n) {
    u8 *d = dst;
    while (n--) *d++ = val;
    return dst;
}

int memcmp(const void *a, const void *b, size_t n) {
    const u8 *aa = a, *bb = b;
    while (n--) {
        if (*aa != *bb) return *aa - *bb;
        aa++; bb++;
    }
    return 0;
}

char *strcpy(char *dst, const char *src) {
    char *d = dst;
    while ((*d++ = *src++));
    return dst;
}

// Boot allocator
static u8 heap[1024*1024]; // 1MB
static size_t used = 0;

void *boot_alloc(size_t size) {
    if (used + size > sizeof(heap)) return NULL;
    void *p = heap + used;
    used += size;
    return p;
}

// Framebuffer
framebuffer_t g_fb;

int framebuffer_init(framebuffer_t *fb) {
    fb->base = (void*)0x80000000ULL;
    fb->width = 1080;
    fb->height = 1920;
    fb->pitch = 4320;
    fb->bpp = 32;
    g_fb = *fb;
    return 0;
}

void fb_fill_rect(framebuffer_t *fb, u32 x, u32 y, u32 w, u32 h, u32 color) {
    if (!fb->base) return;
    u32 *pixels = (u32*)fb->base;
    u32 pitch_words = fb->pitch / 4;
    for (u32 i = y; i < y+h && i < fb->height; i++) {
        for (u32 j = x; j < x+w && j < fb->width; j++) {
            pixels[i * pitch_words + j] = color;
        }
    }
}

// Font
static const u8 font[128][8] = {
    [' '] = {0,0,0,0,0,0,0,0},
    ['A'] = {0x18,0x24,0x42,0x42,0x7E,0x42,0x42,0x42},
    ['B'] = {0x7C,0x42,0x42,0x7C,0x42,0x42,0x42,0x7C},
    ['C'] = {0x3C,0x42,0x40,0x40,0x40,0x40,0x42,0x3C},
    ['D'] = {0x78,0x44,0x42,0x42,0x42,0x42,0x44,0x78},
    ['E'] = {0x7E,0x40,0x40,0x7C,0x40,0x40,0x40,0x7E},
    ['F'] = {0x7E,0x40,0x40,0x7C,0x40,0x40,0x40,0x40},
    ['G'] = {0x3C,0x42,0x40,0x4E,0x42,0x42,0x42,0x3C},
    ['H'] = {0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x42},
    ['I'] = {0x7C,0x10,0x10,0x10,0x10,0x10,0x10,0x7C},
    ['J'] = {0x1E,0x04,0x04,0x04,0x04,0x04,0x44,0x38},
    ['K'] = {0x42,0x44,0x48,0x70,0x48,0x44,0x42,0x42},
    ['L'] = {0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x7E},
    ['M'] = {0x42,0x66,0x5A,0x42,0x42,0x42,0x42,0x42},
    ['N'] = {0x42,0x62,0x52,0x4A,0x46,0x42,0x42,0x42},
    ['O'] = {0x3C,0x42,0x42,0x42,0x42,0x42,0x42,0x3C},
    ['P'] = {0x7C,0x42,0x42,0x7C,0x40,0x40,0x40,0x40},
    ['Q'] = {0x3C,0x42,0x42,0x42,0x4A,0x46,0x42,0x3C},
    ['R'] = {0x7C,0x42,0x42,0x7C,0x48,0x44,0x42,0x42},
    ['S'] = {0x3E,0x40,0x40,0x3C,0x02,0x02,0x02,0x7C},
    ['T'] = {0x7E,0x10,0x10,0x10,0x10,0x10,0x10,0x10},
    ['U'] = {0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x3C},
    ['V'] = {0x42,0x42,0x42,0x42,0x42,0x42,0x24,0x18},
    ['W'] = {0x42,0x42,0x42,0x42,0x42,0x5A,0x66,0x42},
    ['X'] = {0x42,0x42,0x24,0x18,0x18,0x24,0x42,0x42},
    ['Y'] = {0x42,0x42,0x42,0x24,0x18,0x10,0x10,0x10},
    ['Z'] = {0x7E,0x02,0x04,0x08,0x10,0x20,0x40,0x7E},
    ['0'] = {0x3C,0x42,0x46,0x4A,0x52,0x62,0x42,0x3C},
    ['1'] = {0x18,0x28,0x08,0x08,0x08,0x08,0x08,0x3E},
    ['2'] = {0x3C,0x42,0x02,0x04,0x18,0x20,0x40,0x7E},
    ['3'] = {0x3C,0x42,0x02,0x1C,0x02,0x02,0x42,0x3C},
    ['4'] = {0x04,0x0C,0x14,0x24,0x44,0x7E,0x04,0x04},
    ['5'] = {0x7E,0x40,0x40,0x7C,0x02,0x02,0x02,0x7C},
    ['6'] = {0x3C,0x40,0x40,0x7C,0x42,0x42,0x42,0x3C},
    ['7'] = {0x7E,0x02,0x04,0x08,0x10,0x10,0x10,0x10},
    ['8'] = {0x3C,0x42,0x42,0x3C,0x42,0x42,0x42,0x3C},
    ['9'] = {0x3C,0x42,0x42,0x3E,0x02,0x02,0x02,0x3C},
};

u32 cursor_x = 0, cursor_y = 0;

void console_init(void) {
    framebuffer_init(&g_fb);
    console_clear();
}

void putc(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y += 8;
        if (cursor_y + 8 > g_fb.height) cursor_y = 0;
        return;
    }
    if (c == '\r') {
        cursor_x = 0;
        return;
    }
    if ((u8)c >= 128) c = '?';
    const u8 *glyph = font[(u8)c];
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (glyph[i] & (0x80 >> j)) {
                fb_fill_rect(&g_fb, cursor_x + j, cursor_y + i, 1, 1, 0xFFFFFFFF);
            }
        }
    }
    cursor_x += 8;
    if (cursor_x + 8 > g_fb.width) {
        cursor_x = 0;
        cursor_y += 8;
    }
}

void puts(const char *s) {
    while (*s) putc(*s++);
}

void console_clear(void) {
    fb_fill_rect(&g_fb, 0, 0, g_fb.width, g_fb.height, 0xFF000000);
    cursor_x = 0;
    cursor_y = 0;
}

void printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[256];
    int i = 0;
    while (*fmt && i < 255) {
        if (*fmt == '%') {
            fmt++;
            if (*fmt == 's') {
                const char *s = va_arg(args, const char*);
                while (*s && i < 255) buf[i++] = *s++;
            } else if (*fmt == 'c') {
                buf[i++] = (char)va_arg(args, int);
            } else if (*fmt == 'd') {
                int d = va_arg(args, int);
                if (d < 0) { buf[i++] = '-'; d = -d; }
                char tmp[10];
                int j = 0;
                do {
                    tmp[j++] = '0' + d % 10;
                    d /= 10;
                } while (d);
                while (j--) buf[i++] = tmp[j];
            } else if (*fmt == 'x') {
                u32 x = va_arg(args, u32);
                for (int k = 7; k >= 0; k--) {
                    int nib = (x >> (k*4)) & 0xF;
                    buf[i++] = nib < 10 ? '0' + nib : 'A' + nib - 10;
                }
            } else {
                buf[i++] = '%';
                buf[i++] = *fmt;
            }
        } else {
            buf[i++] = *fmt;
        }
        fmt++;
    }
    buf[i] = 0;
    va_end(args);
    puts(buf);
}

// Input
#define GPIO_BASE 0x10000000ULL
#define GPIO_DATA 0x00
#define GPIO_DIR 0x04

void input_init(void) {
    volatile u32 *gpio_dir = (volatile u32*)(GPIO_BASE + GPIO_DIR);
    *gpio_dir &= ~((1<<114) | (1<<115) | (1<<116)); // input
}

boot_key_t input_poll(void) {
    volatile u32 *gpio_data = (volatile u32*)(GPIO_BASE + GPIO_DATA);
    u32 data = *gpio_data;
    if (!(data & (1<<114))) return KEY_DOWN;
    if (!(data & (1<<115))) return KEY_UP;
    if (!(data & (1<<116))) return KEY_SELECT;
    return KEY_NONE;
}

boot_key_t input_get(void) {
    boot_key_t k;
    while ((k = input_poll()) == KEY_NONE) udelay(10000);
    return k;
}

void udelay(u32 usec) {
    for (volatile u32 i = 0; i < usec * 10; i++);
}

// Panic/Halt
__attribute__((noreturn))
void panic(const char *reason) {
    puts("PANIC: ");
    puts(reason);
    puts("\n");
    halt();
}

__attribute__((noreturn))
void halt(void) {
    for (;;) __asm__ volatile ("wfi");
}

// FS stubs
int fs_mount(fs_t *fs, const char *path) {
    (void)fs; (void)path;
    return -1;
}

int fs_open(fs_t *fs, const char *path, file_t *out) {
    (void)fs; (void)path; (void)out;
    return -1;
}

size_t fs_read(file_t *file, void *buf, size_t size) {
    (void)file; (void)buf; (void)size;
    return 0;
}

void fs_close(file_t *file) {
    (void)file;
}