/*
 * FreeBoard - Framebuffer-based UI for PocketDarwin
 * Direct framebuffer rendering without X11, Cocoa, or window managers
 */

#ifndef FREEBOARD_FRAMEBUFFER_H
#define FREEBOARD_FRAMEBUFFER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Framebuffer information structure
typedef struct {
    uint8_t* buffer;        // Pointer to framebuffer memory
    uint32_t width;          // Width in pixels
    uint32_t height;         // Height in pixels
    uint32_t pitch;          // Bytes per row
    uint32_t bpp;            // Bits per pixel (32 = RGBA)
    uint32_t red_mask;       // Red channel bit mask
    uint32_t green_mask;     // Green channel bit mask
    uint32_t blue_mask;      // Blue channel bit mask
    uint32_t alpha_mask;     // Alpha channel bit mask
} framebuffer_t;

// Color definitions (32-bit RGBA)
#define COLOR_BLACK      0xFF000000
#define COLOR_WHITE      0xFFFFFFFF
#define COLOR_GRAY       0xFF808080
#define COLOR_DARK_GRAY   0xFF404040
#define COLOR_BLUE       0xFF007AFF
#define COLOR_ORANGE     0xFFFF9500
#define COLOR_LIGHT_GRAY  0xFFCCCCCC

// UI Layout constants
#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600
#define STATUS_HEIGHT    40
#define DOCK_HEIGHT      120
#define DOCK_WIDTH       400
#define ICON_SIZE        60
#define ICON_SPACING     12

// Framebuffer operations
bool framebuffer_init(framebuffer_t* fb);
void framebuffer_clear(framebuffer_t* fb, uint32_t color);
void framebuffer_set_pixel(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t color);
uint32_t framebuffer_get_pixel(framebuffer_t* fb, uint32_t x, uint32_t y);
void framebuffer_fill_rect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void framebuffer_draw_rect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void framebuffer_draw_rounded_rect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t radius, uint32_t color);
void framebuffer_draw_circle(framebuffer_t* fb, uint32_t cx, uint32_t cy, uint32_t radius, uint32_t color);
void framebuffer_draw_line(framebuffer_t* fb, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color);
void framebuffer_blit(framebuffer_t* dst, framebuffer_t* src, uint32_t dst_x, uint32_t dst_y);

// Text rendering (simple bitmap font)
void framebuffer_draw_char(framebuffer_t* fb, uint32_t x, uint32_t y, char c, uint32_t color);
void framebuffer_draw_string(framebuffer_t* fb, uint32_t x, uint32_t y, const char* str, uint32_t color);

// Utility functions
uint32_t rgba_to_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void color_to_rgba(uint32_t color, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
uint32_t blend_colors(uint32_t src, uint32_t dst, uint8_t alpha);

// Platform-specific framebuffer access
bool framebuffer_map_device_memory(framebuffer_t* fb, const char* device);
void framebuffer_unmap_device_memory(framebuffer_t* fb);

#ifdef __cplusplus
}
#endif

#endif // FREEBOARD_FRAMEBUFFER_H
