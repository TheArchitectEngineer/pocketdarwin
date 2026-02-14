/*
 * FreeBoard - Framebuffer-based launcher using LinuxKit
 * Patches for real device framebuffer access
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include "../Frameworks/LinuxKit/LinuxKit.h"

// UI Layout constants
#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600
#define STATUS_HEIGHT    40
#define DOCK_HEIGHT      120
#define DOCK_WIDTH       400
#define ICON_SIZE        60
#define ICON_SPACING     12
#define DOCK_CORNER_RADIUS 20

// Color definitions (32-bit RGBA)
#define COLOR_BLACK      0xFF000000
#define COLOR_WHITE      0xFFFFFFFF
#define COLOR_GRAY       0xFF808080
#define COLOR_DARK_GRAY   0xFF404040
#define COLOR_BLUE       0xFF007AFF
#define COLOR_ORANGE     0xFFFF9500
#define COLOR_LIGHT_GRAY  0xFFCCCCCC
#define COLOR_SEMI_BLACK  0x80000000

// App icons
typedef struct {
    const char* name;
    uint32_t color;
    char initial;
} AppIcon;

static AppIcon defaultIcons[] = {
    {"App Store", COLOR_BLUE, 'A'},
    {"Books", COLOR_ORANGE, 'B'},
    {"Calculator", COLOR_GRAY, 'C'},
    {"Calendar", COLOR_WHITE, 'D'}
};

// Global state
static LKDisplay* g_display = NULL;
static LKTouchscreen* g_touchscreen = NULL;
static LKBattery* g_battery = NULL;
static uint32_t* g_framebuffer = NULL;
static uint32_t g_framebufferWidth = 0;
static uint32_t g_framebufferHeight = 0;
static uint32_t g_framebufferPitch = 0;
static bool g_running = true;
static int g_fb_fd = -1;

// ============================================================================
// Real Framebuffer Access Functions
// ============================================================================

static bool openFramebufferDevice(const char* device) {
    g_fb_fd = open(device, O_RDWR);
    if (g_fb_fd < 0) {
        LKLogError("Failed to open framebuffer device %s", device);
        return false;
    }
    
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    
    // Get variable screen info
    if (ioctl(g_fb_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        LKLogError("Failed to get variable screen info");
        close(g_fb_fd);
        g_fb_fd = -1;
        return false;
    }
    
    // Get fixed screen info
    if (ioctl(g_fb_fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        LKLogError("Failed to get fixed screen info");
        close(g_fb_fd);
        g_fb_fd = -1;
        return false;
    }
    
    // Store framebuffer info
    g_framebufferWidth = vinfo.xres;
    g_framebufferHeight = vinfo.yres;
    g_framebufferPitch = finfo.line_length;
    
    LKLogInfo("Framebuffer: %dx%d, pitch=%d, bpp=%d", 
              g_framebufferWidth, g_framebufferHeight, g_framebufferPitch, vinfo.bits_per_pixel);
    
    // Map framebuffer memory
    size_t size = g_framebufferHeight * g_framebufferPitch;
    g_framebuffer = (uint32_t*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, g_fb_fd, 0);
    
    if (g_framebuffer == MAP_FAILED) {
        LKLogError("Failed to map framebuffer memory");
        close(g_fb_fd);
        g_fb_fd = -1;
        g_framebuffer = NULL;
        return false;
    }
    
    LKLogInfo("Framebuffer mapped successfully at %p", g_framebuffer);
    return true;
}

static void closeFramebufferDevice(void) {
    if (g_framebuffer && g_framebuffer != MAP_FAILED) {
        size_t size = g_framebufferHeight * g_framebufferPitch;
        munmap(g_framebuffer, size);
        g_framebuffer = NULL;
    }
    
    if (g_fb_fd >= 0) {
        close(g_fb_fd);
        g_fb_fd = -1;
    }
    
    LKLogInfo("Framebuffer device closed");
}

// ============================================================================
// Enhanced LinuxKit Display Implementation
// ============================================================================

// Override the LinuxKit display functions to use real framebuffer
static bool lkDisplayInitializeReal(LKDisplay* display, uint32_t width, uint32_t height, LKDisplayFormat format) {
    if (!display) {
        return false;
    }
    
    // Try to open framebuffer device
    const char* fb_devices[] = {"/dev/fb0", "/dev/fb1", "/dev/graphics/fb0", NULL};
    
    for (int i = 0; fb_devices[i]; i++) {
        if (openFramebufferDevice(fb_devices[i])) {
            break;
        }
    }
    
    if (g_fb_fd < 0) {
        LKLogWarn("Failed to open real framebuffer, using simulated mode");
        return false;
    }
    
    // Update display info with real framebuffer properties
    display->width = g_framebufferWidth;
    display->height = g_framebufferHeight;
    display->pitch = g_framebufferPitch;
    display->format = format;
    display->framebuffer = g_framebuffer;
    
    LKLogInfo("Real framebuffer initialized: %dx%d", display->width, display->height);
    return true;
}

static void* lkDisplayGetFramebufferReal(LKDisplay* display) {
    return g_framebuffer;
}

static void lkDisplayFlushReal(LKDisplay* display) {
    // For real framebuffer, changes are immediate
    // But we can add sync if needed
    if (g_fb_fd >= 0) {
        // Force display update (some drivers need this)
        ioctl(g_fb_fd, FBIOBLANK, 0);
    }
}

// ============================================================================
// Drawing Functions
// ============================================================================

static void setPixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= g_framebufferWidth || y >= g_framebufferHeight) return;
    
    // Handle different pixel formats
    if (g_framebufferPitch == g_framebufferWidth * 4) {
        // RGBA8888 - direct mapping
        g_framebuffer[y * g_framebufferWidth + x] = color;
    } else if (g_framebufferPitch == g_framebufferWidth * 2) {
        // RGB565 - convert to 16-bit
        uint8_t r = (color >> 24) & 0xFF;
        uint8_t g = (color >> 16) & 0xFF;
        uint8_t b = (color >> 8) & 0xFF;
        
        uint16_t rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        ((uint16_t*)g_framebuffer)[y * (g_framebufferPitch/2) + x] = rgb565;
    } else if (g_framebufferPitch == g_framebufferWidth * 3) {
        // RGB888 - convert to 24-bit
        uint8_t* pixel = (uint8_t*)g_framebuffer + y * g_framebufferPitch + x * 3;
        pixel[0] = (color >> 24) & 0xFF; // R
        pixel[1] = (color >> 16) & 0xFF; // G
        pixel[2] = (color >> 8) & 0xFF;  // B
    }
}

static uint32_t getPixel(uint32_t x, uint32_t y) {
    if (x >= g_framebufferWidth || y >= g_framebufferHeight) return 0;
    
    if (g_framebufferPitch == g_framebufferWidth * 4) {
        return g_framebuffer[y * g_framebufferWidth + x];
    }
    
    return 0; // Simplified for other formats
}

static void fillRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    for (uint32_t py = y; py < y + h && py < g_framebufferHeight; py++) {
        for (uint32_t px = x; px < x + w && px < g_framebufferWidth; px++) {
            setPixel(px, py, color);
        }
    }
}

static void drawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    // Top and bottom edges
    for (uint32_t px = x; px < x + w && px < g_framebufferWidth; px++) {
        if (y < g_framebufferHeight) setPixel(px, y, color);
        if (y + h - 1 < g_framebufferHeight) setPixel(px, y + h - 1, color);
    }
    
    // Left and right edges
    for (uint32_t py = y; py < y + h && py < g_framebufferHeight; py++) {
        if (x < g_framebufferWidth) setPixel(x, py, color);
        if (x + w - 1 < g_framebufferWidth) setPixel(x + w - 1, py, color);
    }
}

static void drawRoundedRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t radius, uint32_t color) {
    // Fill main rectangle
    fillRect(x + radius, y, w - 2 * radius, h, color);
    fillRect(x, y + radius, radius, h - 2 * radius, color);
    fillRect(x + w - radius, y + radius, radius, h - 2 * radius, color);
    
    // Draw rounded corners (simple approximation)
    for (int i = 0; i < radius; i++) {
        for (int j = 0; j < radius; j++) {
            if (i * i + j * j <= radius * radius) {
                // Top-left corner
                if (x + i < g_framebufferWidth && y + j < g_framebufferHeight) {
                    setPixel(x + i, y + j, color);
                }
                // Top-right corner
                if (x + w - 1 - i < g_framebufferWidth && y + j < g_framebufferHeight) {
                    setPixel(x + w - 1 - i, y + j, color);
                }
                // Bottom-left corner
                if (x + i < g_framebufferWidth && y + h - 1 - j < g_framebufferHeight) {
                    setPixel(x + i, y + h - 1 - j, color);
                }
                // Bottom-right corner
                if (x + w - 1 - i < g_framebufferWidth && y + h - 1 - j < g_framebufferHeight) {
                    setPixel(x + w - 1 - i, y + h - 1 - j, color);
                }
            }
        }
    }
}

static void drawCircle(uint32_t cx, uint32_t cy, uint32_t radius, uint32_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                uint32_t px = cx + x;
                uint32_t py = cy + y;
                if (px < g_framebufferWidth && py < g_framebufferHeight) {
                    setPixel(px, py, color);
                }
            }
        }
    }
}

static void drawChar(uint32_t x, uint32_t y, char c, uint32_t color, uint32_t size) {
    // Simple 5x7 bitmap font representation
    if (c == ' ') return;
    
    uint32_t charWidth = size / 2;
    uint32_t charHeight = size;
    
    // Draw the character as a simple rectangle with the character
    fillRect(x, y, charWidth, charHeight, color);
    
    // Add a small dot to indicate it's a character
    uint32_t dotSize = size / 6;
    fillRect(x + charWidth/2 - dotSize/2, y + charHeight/2 - dotSize/2, dotSize, dotSize, COLOR_BLACK);
}

static void drawString(uint32_t x, uint32_t y, const char* str, uint32_t color, uint32_t size) {
    uint32_t currentX = x;
    while (*str && currentX < g_framebufferWidth - size) {
        drawChar(currentX, y, *str, color, size);
        currentX += size / 2 + 2; // Character spacing
        str++;
    }
}

// ============================================================================
// UI Rendering Functions
// ============================================================================

static void renderWallpaper(void) {
    // Create a gradient wallpaper similar to the reference image
    for (uint32_t y = 0; y < g_framebufferHeight; y++) {
        for (uint32_t x = 0; x < g_framebufferWidth; x++) {
            // Create a nice gradient
            float r = 0.4f + 0.2f * sinf(x * 0.01f);
            float g = 0.6f + 0.2f * sinf(y * 0.01f + 1.0f);
            float b = 0.8f + 0.2f * sinf((x + y) * 0.01f + 2.0f);
            
            uint32_t color = ((uint32_t)(r * 255) << 24) | 
                           ((uint32_t)(g * 255) << 16) | 
                           ((uint32_t)(b * 255) << 8) | 
                           0xFF;
            
            setPixel(x, y, color);
        }
    }
}

static void renderStatusBar(void) {
    // Status bar is transparent - we just draw the text
    uint32_t textY = STATUS_HEIGHT - 15;
    
    // Draw time (left side)
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timeStr[32];
    strftime(timeStr, sizeof(timeStr), "%H:%M", tm_info);
    
    drawString(20, textY, timeStr, COLOR_WHITE, 16);
    
    // Draw WiFi icon (simple representation)
    uint32_t wifiX = g_framebufferWidth - 120;
    uint32_t wifiY = textY - 8;
    
    // Simple WiFi signal waves
    for (int i = 0; i < 3; i++) {
        uint32_t waveHeight = 4 + i * 4;
        uint32_t waveWidth = 8 + i * 3;
        uint32_t waveY = wifiY + 8 - waveHeight/2;
        
        for (uint32_t w = 0; w < waveWidth; w++) {
            float angle = (float)w / waveWidth * 3.14159f;
            int offsetX = (int)(cos(angle) * (4 + i * 3));
            int offsetY = (int)(sin(angle) * (4 + i * 3));
            
            if (wifiX + offsetX < g_framebufferWidth && waveY + offsetY < g_framebufferHeight) {
                setPixel(wifiX + offsetX, waveY + offsetY, COLOR_WHITE);
            }
        }
    }
    
    // Draw battery indicator
    uint32_t batteryX = g_framebufferWidth - 80;
    uint32_t batteryY = textY - 8;
    uint32_t batteryWidth = 30;
    uint32_t batteryHeight = 16;
    
    // Battery outline
    drawRect(batteryX, batteryY, batteryWidth, batteryHeight, COLOR_WHITE);
    
    // Battery tip
    fillRect(batteryX + batteryWidth, batteryY + 4, 4, 8, COLOR_WHITE);
    
    // Battery fill
    int32_t percentage = LKBatteryGetPercentage(g_battery);
    uint32_t fillWidth = (batteryWidth - 4) * percentage / 100;
    
    if (percentage > 20) {
        fillRect(batteryX + 2, batteryY + 2, fillWidth, batteryHeight - 4, COLOR_WHITE);
    } else {
        // Low battery - red fill
        uint32_t redColor = 0xFF0000FF;
        fillRect(batteryX + 2, batteryY + 2, fillWidth, batteryHeight - 4, redColor);
    }
}

static void renderDock(void) {
    uint32_t dockX = (g_framebufferWidth - DOCK_WIDTH) / 2;
    uint32_t dockY = g_framebufferHeight - DOCK_HEIGHT - 20;
    
    // Dock background with transparency effect
    uint32_t dockBgColor = COLOR_SEMI_BLACK;
    drawRoundedRect(dockX, dockY, DOCK_WIDTH, DOCK_HEIGHT, DOCK_CORNER_RADIUS, dockBgColor);
    
    // Dock border
    drawRoundedRect(dockX, dockY, DOCK_WIDTH, DOCK_HEIGHT, DOCK_CORNER_RADIUS, COLOR_LIGHT_GRAY);
    
    // Top highlight
    for (uint32_t x = dockX + DOCK_CORNER_RADIUS; x < dockX + DOCK_WIDTH - DOCK_CORNER_RADIUS; x++) {
        if (dockY < g_framebufferHeight) {
            setPixel(x, dockY, COLOR_WHITE);
        }
    }
    
    // Bottom shadow
    for (uint32_t x = dockX + DOCK_CORNER_RADIUS; x < dockX + DOCK_WIDTH - DOCK_CORNER_RADIUS; x++) {
        if (dockY + DOCK_HEIGHT - 1 < g_framebufferHeight) {
            setPixel(x, dockY + DOCK_HEIGHT - 1, COLOR_DARK_GRAY);
        }
    }
    
    // Render icons
    uint32_t iconStartX = dockX + 40;
    uint32_t iconY = dockY + (DOCK_HEIGHT - ICON_SIZE) / 2;
    
    for (int i = 0; i < 4; i++) {
        uint32_t iconX = iconStartX + i * (ICON_SIZE + ICON_SPACING);
        
        // Icon background
        drawRoundedRect(iconX, iconY, ICON_SIZE, ICON_SIZE, 12, defaultIcons[i].color);
        
        // Icon border
        drawRoundedRect(iconX, iconY, ICON_SIZE, ICON_SIZE, 12, COLOR_BLACK);
        
        // Icon initial
        drawChar(iconX + ICON_SIZE/2 - 8, iconY + ICON_SIZE/2 - 8, 
                defaultIcons[i].initial, COLOR_WHITE, 16);
    }
}

static void render(void) {
    if (!g_framebuffer) return;
    
    // Clear framebuffer
    memset(g_framebuffer, 0, g_framebufferHeight * g_framebufferPitch);
    
    // Render UI layers
    renderWallpaper();
    renderStatusBar();
    renderDock();
    
    // Flush to display
    lkDisplayFlushReal(NULL);
}

// ============================================================================
// Input Handling
// ============================================================================

static void handleTouchInput(void) {
    LKTouchPoint touch;
    
    while (LKTouchscreenHasEvent(g_touchscreen)) {
        if (LKTouchscreenGetEvent(g_touchscreen, &touch)) {
            LKLogDebug("Touch event: (%d, %d) - %d", touch.x, touch.y, touch.event);
            
            // Handle touch events (icon clicks, etc.)
            if (touch.event == LK_TOUCH_EVENT_DOWN) {
                // Check if touch is on dock icons
                uint32_t dockX = (g_framebufferWidth - DOCK_WIDTH) / 2;
                uint32_t dockY = g_framebufferHeight - DOCK_HEIGHT - 20;
                uint32_t iconStartX = dockX + 40;
                uint32_t iconY = dockY + (DOCK_HEIGHT - ICON_SIZE) / 2;
                
                for (int i = 0; i < 4; i++) {
                    uint32_t iconX = iconStartX + i * (ICON_SIZE + ICON_SPACING);
                    
                    if (touch.x >= iconX && touch.x < iconX + ICON_SIZE &&
                        touch.y >= iconY && touch.y < iconY + ICON_SIZE) {
                        LKLogInfo("Icon clicked: %s", defaultIcons[i].name);
                        // In a real implementation, this would launch the app
                        break;
                    }
                }
            }
        }
    }
}

// ============================================================================
// Main Application
// ============================================================================

static bool initialize(void) {
    LKLogInfo("Initializing FreeBoard with LinuxKit framebuffer");
    
    // Initialize LinuxKit
    if (!LKInitialize()) {
        LKLogError("Failed to initialize LinuxKit");
        return false;
    }
    
    // Try to initialize real framebuffer first
    g_display = LKDisplayCreate();
    if (!g_display) {
        LKLogError("Failed to create display");
        return false;
    }
    
    // Try real framebuffer initialization
    if (!lkDisplayInitializeReal(g_display, SCREEN_WIDTH, SCREEN_HEIGHT, LK_DISPLAY_FORMAT_RGBA8888)) {
        LKLogWarn("Real framebuffer not available, falling back to simulated mode");
        
        // Fallback to simulated mode
        if (!LKDisplayInitialize(g_display, SCREEN_WIDTH, SCREEN_HEIGHT, LK_DISPLAY_FORMAT_RGBA8888)) {
            LKLogError("Failed to initialize display");
            return false;
        }
        
        g_framebuffer = (uint32_t*)LKDisplayGetFramebuffer(g_display);
        g_framebufferWidth = SCREEN_WIDTH;
        g_framebufferHeight = SCREEN_HEIGHT;
        g_framebufferPitch = SCREEN_WIDTH * 4;
    }
    
    if (!LKDisplayEnable(g_display)) {
        LKLogError("Failed to enable display");
        return false;
    }
    
    // Initialize touchscreen
    g_touchscreen = LKTouchscreenCreate();
    if (!g_touchscreen) {
        LKLogError("Failed to create touchscreen");
        return false;
    }
    
    if (!LKTouchscreenInitialize(g_touchscreen)) {
        LKLogError("Failed to initialize touchscreen");
        return false;
    }
    
    if (!LKTouchscreenEnable(g_touchscreen)) {
        LKLogError("Failed to enable touchscreen");
        return false;
    }
    
    // Initialize battery
    g_battery = LKBatteryCreate();
    if (!g_battery) {
        LKLogError("Failed to create battery");
        return false;
    }
    
    if (!LKBatteryUpdateStatus(g_battery)) {
        LKLogWarn("Failed to update battery status");
    }
    
    LKLogInfo("FreeBoard initialized successfully");
    LKLogInfo("Framebuffer: %dx%d, pitch=%d", g_framebufferWidth, g_framebufferHeight, g_framebufferPitch);
    return true;
}

static void cleanup(void) {
    LKLogInfo("Cleaning up FreeBoard");
    
    closeFramebufferDevice();
    
    if (g_battery) {
        LKBatteryDestroy(g_battery);
        g_battery = NULL;
    }
    
    if (g_touchscreen) {
        LKTouchscreenDestroy(g_touchscreen);
        g_touchscreen = NULL;
    }
    
    if (g_display) {
        LKDisplayDestroy(g_display);
        g_display = NULL;
    }
    
    LKShutdown();
    
    LKLogInfo("FreeBoard cleanup complete");
}

int main(int argc, char* argv[]) {
    LKLogInfo("FreeBoard - PocketDarwin Modern Launcher");
    LKLogInfo("=======================================");
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Usage: %s [OPTIONS]\n", argv[0]);
            printf("Options:\n");
            printf("  -h, --help     Show this help message\n");
            printf("  --version      Show version information\n");
            printf("  --debug        Enable debug logging\n");
            printf("  --fb-device    Specify framebuffer device (default: /dev/fb0)\n");
            return 0;
        } else if (strcmp(argv[i], "--version") == 0) {
            printf("FreeBoard 1.0.0\n");
            printf("PocketDarwin Modern Launcher with LinuxKit\n");
            return 0;
        } else if (strcmp(argv[i], "--debug") == 0) {
            LKSetLogLevel(LK_LOG_LEVEL_DEBUG);
        } else if (strcmp(argv[i], "--fb-device") == 0 && i + 1 < argc) {
            // Custom framebuffer device
            i++;
            LKLogInfo("Using custom framebuffer device: %s", argv[i]);
            if (!openFramebufferDevice(argv[i])) {
                LKLogError("Failed to open custom framebuffer device");
                return 1;
            }
        } else {
            printf("Unknown option: %s\n", argv[i]);
            printf("Use --help for usage information\n");
            return 1;
        }
    }
    
    // Initialize the application
    if (!initialize()) {
        LKLogError("Failed to initialize FreeBoard");
        return 1;
    }
    
    // Show system information
    LKPropertyList* sysInfo = LKGetSystemInfo();
    if (sysInfo) {
        LKLogInfo("Platform: %s", LKPropertyListGetString(sysInfo, "platform", "Unknown"));
        LKLogInfo("Architecture: %s", LKPropertyListGetString(sysInfo, "architecture", "Unknown"));
        LKLogInfo("Kernel: %s", LKPropertyListGetString(sysInfo, "kernel", "Unknown"));
        LKPropertyListDestroy(sysInfo);
    }
    
    LKLogInfo("Resolution: %dx%d", g_framebufferWidth, g_framebufferHeight);
    LKLogInfo("Framebuffer: %s", g_fb_fd >= 0 ? "Real device" : "Simulated");
    LKLogInfo("UI Style: Framebuffer-based with LinuxKit");
    LKLogInfo("Press Ctrl+C to exit");
    
    // Main loop
    uint32_t frameCount = 0;
    time_t lastBatteryUpdate = time(NULL);
    
    while (g_running) {
        // Handle input
        handleTouchInput();
        
        // Update battery status every 30 seconds
        time_t now = time(NULL);
        if (now - lastBatteryUpdate >= 30) {
            if (g_battery) {
                LKBatteryUpdateStatus(g_battery);
                LKLogDebug("Battery: %d%%", LKBatteryGetPercentage(g_battery));
            }
            lastBatteryUpdate = now;
        }
        
        // Render frame
        render();
        
        // Frame limiting (60 FPS)
        usleep(16667); // ~60 FPS
        
        frameCount++;
        
        // Log frame rate every 10 seconds
        if (frameCount % 600 == 0) {
            LKLogDebug("Rendered %d frames", frameCount);
        }
    }
    
    // Cleanup
    cleanup();
    
    LKLogInfo("FreeBoard exited cleanly");
    return 0;
}
