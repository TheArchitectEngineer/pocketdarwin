#include "bootstd.h"
#include <stdint.h>

// Platform-specific implementations for ARM64 Android

// UART for console
#define UART_BASE 0x09000000ULL
#define UART_DR   (UART_BASE + 0x00)
#define UART_FR   (UART_BASE + 0x18)

void console_init(void) {
    // Assume UART is already initialized by bootloader
}

void putc(char c) {
    volatile uint32_t *dr = (volatile uint32_t *)UART_DR;
    volatile uint32_t *fr = (volatile uint32_t *)UART_FR;
    while (*fr & (1 << 5)); // wait TXFF
    *dr = c;
}

void puts(const char *s) {
    while (*s) putc(*s++);
}

void printf(const char *fmt, ...) {
    // Simple printf, only %s %d %x
    va_list args;
    va_start(args, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            if (*fmt == 's') {
                puts(va_arg(args, const char*));
            } else if (*fmt == 'd') {
                int d = va_arg(args, int);
                if (d < 0) { putc('-'); d = -d; }
                if (d == 0) putc('0');
                else {
                    char buf[10];
                    int i = 0;
                    while (d) {
                        buf[i++] = '0' + (d % 10);
                        d /= 10;
                    }
                    while (i--) putc(buf[i]);
                }
            } else if (*fmt == 'x') {
                uint32_t x = va_arg(args, uint32_t);
                puts("0x");
                for (int i = 7; i >= 0; i--) {
                    int nib = (x >> (i*4)) & 0xF;
                    putc(nib < 10 ? '0' + nib : 'a' + nib - 10);
                }
            }
        } else {
            putc(*fmt);
        }
        fmt++;
    }
    va_end(args);
}

void console_clear(void) {
    // No clear for UART
}

// Input: GPIO keys
#define GPIO_BASE 0x0a000000ULL
#define GPIO_DATA (GPIO_BASE + 0x00)
#define GPIO_DIR  (GPIO_BASE + 0x04)

// Assume Vol+ = bit 0, Vol- = bit 1, Power = bit 2

void input_init(void) {
    volatile uint32_t *dir = (volatile uint32_t *)GPIO_DIR;
    *dir &= ~(7); // input
}

boot_key_t input_poll(void) {
    volatile uint32_t *data = (volatile uint32_t *)GPIO_DATA;
    uint32_t val = *data;
    if (!(val & 1)) return KEY_UP;
    if (!(val & 2)) return KEY_DOWN;
    if (!(val & 4)) return KEY_SELECT;
    return KEY_NONE;
}

boot_key_t input_get(void) {
    boot_key_t k;
    while ((k = input_poll()) == KEY_NONE);
    return k;
}

// Framebuffer
#define FB_BASE 0x7fc00000ULL
#define FB_WIDTH 720
#define FB_HEIGHT 1280
#define FB_PITCH (FB_WIDTH * 4)

int framebuffer_init(framebuffer_t *fb) {
    fb->base = (void *)FB_BASE;
    fb->width = FB_WIDTH;
    fb->height = FB_HEIGHT;
    fb->pitch = FB_PITCH;
    fb->bpp = 32;
    return 0;
}

void fb_fill_rect(framebuffer_t *fb, u32 x, u32 y, u32 w, u32 h, u32 color) {
    if (x + w > fb->width) w = fb->width - x;
    if (y + h > fb->height) h = fb->height - y;
    for (u32 i = 0; i < h; i++) {
        u32 *line = (u32 *)((uint8_t *)fb->base + (y + i) * fb->pitch);
        for (u32 j = 0; j < w; j++) {
            line[x + j] = color;
        }
    }
}

// FS: stub
int fs_mount(fs_t *fs, const char *path) {
    // Stub
    return 0;
}

int fs_open(fs_t *fs, const char *path, file_t *out) {
    // Stub
    return 0;
}

size_t fs_read(file_t *file, void *buf, size_t size) {
    // Stub
    return 0;
}

void fs_close(file_t *file) {
}

// Block device
#define MMC_BASE 0x0b000000ULL // Example MMC register base

typedef struct {
    u32 sectors_per_cluster;
    u32 reserved_sectors;
    u32 fat_count;
    u32 root_entries;
    u32 total_sectors;
    u32 sectors_per_fat;
    u32 root_cluster;
    u32 data_start;
} fat32_info_t;

static fat32_info_t fat_info;
static block_dev_t *g_dev;

int block_open(block_dev_t *dev, const char *path) {
    // Stub: assume path like /dev/mmcblk1
    dev->impl = (void*)MMC_BASE;
    g_dev = dev;
    return 0;
}

size_t block_read(block_dev_t *dev, u64 lba, void *buf, size_t sectors) {
    // Stub: read from MMC
    // In real, send CMD18, read data
    // For now, memset to dummy data
    memset(buf, 0xAA, sectors * 512);
    return sectors;
}

// FS: FAT32
int fs_mount(fs_t *fs, const char *path) {
    // Assume path is /dev/mmcblk1
    if (strcmp(path, "/dev/mmcblk1") != 0) return -1;
    fs->impl = g_dev;
    // Read boot sector
    u8 boot[512];
    if (block_read(g_dev, 0, boot, 1) != 1) return -1;
    // Parse FAT32
    fat_info.reserved_sectors = *(u16*)(boot + 14);
    fat_info.fat_count = boot[16];
    fat_info.sectors_per_fat = *(u32*)(boot + 36);
    fat_info.root_cluster = *(u32*)(boot + 44);
    fat_info.data_start = fat_info.reserved_sectors + fat_info.fat_count * fat_info.sectors_per_fat;
    fat_info.sectors_per_cluster = boot[13];
    return 0;
}

int fs_open(fs_t *fs, const char *path, file_t *out) {
    // Stub: find file in root dir
    // Assume path /OCM/config.plist
    // Read root cluster
    u32 cluster = fat_info.root_cluster;
    u8 buf[512];
    u64 lba = fat_info.data_start + (cluster - 2) * fat_info.sectors_per_cluster;
    if (block_read(g_dev, lba, buf, 1) != 1) return -1;
    // Find file
    // For simplicity, assume first entry is the file
    // In real, parse dir entries
    out->impl = (void*)(uintptr_t)lba; // store start LBA
    return 0;
}

size_t fs_read(file_t *file, void *buf, size_t size) {
    u64 lba = (u64)(uintptr_t)file->impl;
    size_t sectors = (size + 511) / 512;
    return block_read(g_dev, lba, buf, sectors) * 512;
}

void fs_close(file_t *file) {
}

// Memory
static uint8_t heap[1024*1024]; // 1MB
static size_t heap_used = 0;

void *boot_alloc(size_t size) {
    if (heap_used + size > sizeof(heap)) return NULL;
    void *p = &heap[heap_used];
    heap_used += size;
    return p;
}

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;
    while (n--) *d++ = *s++;
    return dst;
}

void *memset(void *dst, int val, size_t n) {
    uint8_t *d = dst;
    while (n--) *d++ = val;
    return dst;
}

int memcmp(const void *a, const void *b, size_t n) {
    const uint8_t *aa = a, *bb = b;
    while (n--) {
        if (*aa != *bb) return *aa - *bb;
        aa++; bb++;
    }
    return 0;
}

void udelay(u32 usec) {
    // Busy wait
    for (volatile u32 i = 0; i < usec * 100; i++);
}

__attribute__((noreturn))
void panic(const char *reason) {
    puts("PANIC: ");
    puts(reason);
    puts("\n");
    halt();
}

__attribute__((noreturn))
void halt(void) {
    for (;;) {
        __asm__ volatile ("wfi");
    }
}