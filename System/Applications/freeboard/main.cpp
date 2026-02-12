/*
 * freeboard - PocketDarwin SpringBoard clone (iOS 5/6 style)
 * X11, single foreground apps, scans /Applications for .app bundles.
 * iOS 5/6 skeuomorphic design with rounded icons, dock, and grid layout.
 */

#include "AppBundle.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <cerrno>
#include <algorithm>

namespace {

const int ICON_SIZE = 88;
const int ICON_GAP = 16;
const int PADDING = 20;
const int STATUS_BAR_HEIGHT = 40;
const int COLS = 4;
const int ROWS = 4;
const int DOCK_ICON_SIZE = 76;
const int DOCK_HEIGHT = 120;
const int DOCK_ICON_GAP = 12;
const int DOCK_COLS = 4;
const int LABEL_HEIGHT = 18;
const int CELL_HEIGHT = ICON_SIZE + ICON_GAP + LABEL_HEIGHT;
const int CELL_WIDTH = ICON_SIZE + ICON_GAP;

/* iOS 5/6 skeuomorphic colors (RGB) */
const unsigned long STATUS_BG   = 0x8E8E93;  /* Dark gray status bar */
const unsigned long SCREEN_BG   = 0x6495ED;  /* Default blue gradient background */
const unsigned long ICON_BG     = 0xF7F7F7;  /* Light icon background */
const unsigned long ICON_BORDER = 0xCCCCCC;  /* Icon border color */
const unsigned long LABEL_COLOR = 0xFFFFFF;  /* White text labels */
const unsigned long STATUS_TEXT = 0xFFFFFF;
const unsigned long DOCK_BG     = 0x000000;  /* Black dock background */
const unsigned long DOCK_BORDER = 0x333333;  /* Dock border */

} // namespace

static unsigned long rgbToPixel(Display* dpy, int screen, unsigned long rgb) {
    unsigned long r = (rgb >> 16) & 0xff;
    unsigned long g = (rgb >> 8) & 0xff;
    unsigned long b = rgb & 0xff;
    return (r << 16) | (g << 8) | b;
}

static void drawRoundedRect(Display* dpy, Window win, GC gc, int x, int y, int width, int height, int radius) {
    // Draw the main rectangle with rounded corners using multiple rectangles
    XFillRectangle(dpy, win, gc, x + radius, y, width - 2 * radius, height);
    XFillRectangle(dpy, win, gc, x, y + radius, radius, height - 2 * radius);
    XFillRectangle(dpy, win, gc, x + width - radius, y + radius, radius, height - 2 * radius);
    
    // Draw corner circles (approximated with small rectangles)
    for (int i = 0; i < radius; i++) {
        for (int j = 0; j < radius; j++) {
            if (i * i + j * j <= radius * radius) {
                XDrawPoint(dpy, win, gc, x + i, y + j);
                XDrawPoint(dpy, win, gc, x + width - 1 - i, y + j);
                XDrawPoint(dpy, win, gc, x + i, y + height - 1 - j);
                XDrawPoint(dpy, win, gc, x + width - 1 - i, y + height - 1 - j);
            }
        }
    }
}

static void drawIconWithGloss(Display* dpy, Window win, GC gc, int x, int y, int size, const std::string& appName, bool hasCustomIcon) {
    // Draw icon background with rounded corners
    unsigned long bgPixel = rgbToPixel(dpy, DefaultScreen(dpy), ICON_BG);
    unsigned long borderPixel = rgbToPixel(dpy, DefaultScreen(dpy), ICON_BORDER);
    
    XSetForeground(dpy, gc, bgPixel);
    drawRoundedRect(dpy, win, gc, x, y, size, size, 12);
    
    // Draw border
    XSetForeground(dpy, gc, borderPixel);
    XDrawRectangle(dpy, win, gc, x, y, size, size);
    
    // Draw gloss effect (top half lighter overlay)
    unsigned long glossPixel = rgbToPixel(dpy, DefaultScreen(dpy), 0xFFFFFF);
    XSetForeground(dpy, gc, glossPixel);
    
    // Create gloss effect with transparency simulation using stipple
    for (int i = 0; i < size / 2; i++) {
        int alpha = 255 - (i * 255 / (size / 2));
        if (i % 2 == 0) {
            XDrawLine(dpy, win, gc, x + 2, y + i, x + size - 2, y + i);
        }
    }
    
    // Draw app name or first letter
    if (!appName.empty()) {
        char letter = (appName[0] >= 'a' && appName[0] <= 'z') ? (char)(appName[0] - 32) : appName[0];
        XSetForeground(dpy, gc, rgbToPixel(dpy, DefaultScreen(dpy), 0x333333));
        
        // Center the letter (approximate centering)
        XDrawString(dpy, win, gc, x + size / 2 - 4, y + size / 2 + 4, &letter, 1);
    }
}

static int getCol(int x, int width) {
    int left = PADDING;
    if (x < left) return -1;
    int col = (x - left) / CELL_WIDTH;
    if (col >= COLS) return -1;
    return col;
}

static int getRow(int y, int height) {
    int top = STATUS_BAR_HEIGHT + PADDING;
    int bottom = height - DOCK_HEIGHT;
    if (y < top || y >= bottom) return -1;
    int row = (y - top) / CELL_HEIGHT;
    if (row >= ROWS) return -1;
    return row;
}

static int getDockCol(int x, int width) {
    int dockWidth = DOCK_COLS * DOCK_ICON_SIZE + (DOCK_COLS - 1) * DOCK_ICON_GAP;
    int dockLeft = (width - dockWidth) / 2;
    if (x < dockLeft || x >= dockLeft + dockWidth) return -1;
    return (x - dockLeft) / (DOCK_ICON_SIZE + DOCK_ICON_GAP);
}

static bool pointInMainIcon(int x, int y, int col, int row) {
    int left = PADDING + col * CELL_WIDTH;
    int top = STATUS_BAR_HEIGHT + PADDING + row * CELL_HEIGHT;
    return x >= left && x < left + ICON_SIZE && y >= top && y < top + ICON_SIZE;
}

static bool pointInDockIcon(int x, int y, int dockCol, int height) {
    int dockWidth = DOCK_COLS * DOCK_ICON_SIZE + (DOCK_COLS - 1) * DOCK_ICON_GAP;
    int dockLeft = (800 - dockWidth) / 2; // Assuming 800px width for now
    int dockTop = height - DOCK_HEIGHT + 20;
    int left = dockLeft + dockCol * (DOCK_ICON_SIZE + DOCK_ICON_GAP);
    return x >= left && x < left + DOCK_ICON_SIZE && y >= dockTop && y < dockTop + DOCK_ICON_SIZE;
}

static void launchApp(const std::string& executablePath, const std::string& bundlePath) {
    pid_t pid = fork();
    if (pid < 0) return;
    if (pid == 0) {
        /* Child: run the .app executable; set cwd to bundle so relative paths work */
        if (chdir(bundlePath.c_str()) != 0) { /* ignore */ }
        const char* argv[] = { executablePath.c_str(), nullptr };
        execv(executablePath.c_str(), const_cast<char**>(argv));
        _exit(127);
    }
    /* Parent: optionally wait or leave child in background; single foreground = WM handles it */
    (void)pid;
}

int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::fprintf(stderr, "freeboard: cannot open X display\n");
        return 1;
    }

    int screen = DefaultScreen(display);
    int depth = DefaultDepth(display, screen);
    Visual* visual = DefaultVisual(display, screen);
    Window root = RootWindow(display, screen);

    /* Colors */
    Colormap cmap = DefaultColormap(display, screen);
    XColor statusBg, screenBg, iconBg, iconBorder, labelColor, statusText, dockBg, dockBorder;
    statusBg.red   = (STATUS_BG >> 16) * 257; statusBg.green = ((STATUS_BG >> 8) & 0xff) * 257; statusBg.blue = (STATUS_BG & 0xff) * 257;
    screenBg.red   = (SCREEN_BG >> 16) * 257; screenBg.green = ((SCREEN_BG >> 8) & 0xff) * 257; screenBg.blue = (SCREEN_BG & 0xff) * 257;
    iconBg.red     = (ICON_BG >> 16) * 257;   iconBg.green   = ((ICON_BG >> 8) & 0xff) * 257;   iconBg.blue   = (ICON_BG & 0xff) * 257;
    iconBorder.red = (ICON_BORDER >> 16) * 257; iconBorder.green = ((ICON_BORDER >> 8) & 0xff) * 257; iconBorder.blue = (ICON_BORDER & 0xff) * 257;
    labelColor.red = (LABEL_COLOR >> 16) * 257; labelColor.green = ((LABEL_COLOR >> 8) & 0xff) * 257; labelColor.blue = (LABEL_COLOR & 0xff) * 257;
    statusText.red = (STATUS_TEXT >> 16) * 257; statusText.green = ((STATUS_TEXT >> 8) & 0xff) * 257; statusText.blue = (STATUS_TEXT & 0xff) * 257;
    dockBg.red     = (DOCK_BG >> 16) * 257;   dockBg.green   = ((DOCK_BG >> 8) & 0xff) * 257;   dockBg.blue   = (DOCK_BG & 0xff) * 257;
    dockBorder.red = (DOCK_BORDER >> 16) * 257; dockBorder.green = ((DOCK_BORDER >> 8) & 0xff) * 257; dockBorder.blue = (DOCK_BORDER & 0xff) * 257;
    
    statusBg.flags = screenBg.flags = iconBg.flags = iconBorder.flags = labelColor.flags = statusText.flags = dockBg.flags = dockBorder.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(display, cmap, &statusBg);
    XAllocColor(display, cmap, &screenBg);
    XAllocColor(display, cmap, &iconBg);
    XAllocColor(display, cmap, &iconBorder);
    XAllocColor(display, cmap, &labelColor);
    XAllocColor(display, cmap, &statusText);
    XAllocColor(display, cmap, &dockBg);
    XAllocColor(display, cmap, &dockBorder);

    int width = PADDING * 2 + COLS * CELL_WIDTH - ICON_GAP;
    int height = STATUS_BAR_HEIGHT + PADDING * 2 + ROWS * CELL_HEIGHT - ICON_GAP + DOCK_HEIGHT;

    Window win = XCreateWindow(display, root, 0, 0, (unsigned)width, (unsigned)height, 0,
                               depth, InputOutput, visual, 0, nullptr);
    XSetWindowBackground(display, win, screenBg.pixel);
    XStoreName(display, win, "PocketDarwin");

    XSelectInput(display, win, ExposureMask | ButtonPressMask | KeyPressMask);
    XMapWindow(display, win);

    GC gc = XCreateGC(display, win, 0, nullptr);
    XFontStruct* font = XLoadQueryFont(display, "fixed");
    if (!font) font = XLoadQueryFont(display, "9x15");
    if (font) XSetFont(display, gc, font->fid);

    std::vector<freeboard::AppInfo> apps = freeboard::scanApplications("/Applications");
    
    // Separate apps into main grid (first 16) and dock (first 4 for now)
    std::vector<freeboard::AppInfo> mainApps;
    std::vector<freeboard::AppInfo> dockApps;
    
    for (size_t i = 0; i < apps.size() && i < ROWS * COLS; i++) {
        mainApps.push_back(apps[i]);
    }
    
    for (size_t i = 0; i < apps.size() && i < DOCK_COLS; i++) {
        dockApps.push_back(apps[i]);
    }

    auto draw = [&]() {
        /* Status bar */
        XSetForeground(display, gc, statusBg.pixel);
        XFillRectangle(display, win, gc, 0, 0, (unsigned)width, STATUS_BAR_HEIGHT);
        XSetForeground(display, gc, statusText.pixel);
        XDrawString(display, win, gc, PADDING, STATUS_BAR_HEIGHT - 12, "PocketDarwin", 12);

        /* Main app grid */
        for (size_t i = 0; i < mainApps.size(); i++) {
            int col = (int)(i % COLS);
            int row = (int)(i / COLS);
            int left = PADDING + col * CELL_WIDTH;
            int top = STATUS_BAR_HEIGHT + PADDING + row * CELL_HEIGHT;

            // Draw iOS 5/6 style icon with gloss
            drawIconWithGloss(display, win, gc, left, top, ICON_SIZE, mainApps[i].displayName, mainApps[i].hasCustomIcon);

            /* Label under icon */
            XSetForeground(display, gc, labelColor.pixel);
            std::string label = mainApps[i].displayName;
            if (label.size() > 10) label = label.substr(0, 9) + ".";
            int labelY = top + ICON_SIZE + LABEL_HEIGHT - 2;
            int textWidth = XTextWidth(font, label.c_str(), (int)label.size());
            int textX = left + (ICON_SIZE - textWidth) / 2;
            XDrawString(display, win, gc, textX, labelY, label.c_str(), (int)label.size());
        }

        /* Draw dock */
        int dockWidth = DOCK_COLS * DOCK_ICON_SIZE + (DOCK_COLS - 1) * DOCK_ICON_GAP;
        int dockLeft = (width - dockWidth) / 2;
        int dockTop = height - DOCK_HEIGHT + 10;
        
        // Dock background with rounded corners
        XSetForeground(display, gc, dockBg.pixel);
        drawRoundedRect(display, win, gc, dockLeft - 20, dockTop - 10, dockWidth + 40, DOCK_HEIGHT - 10, 15);
        
        // Dock border
        XSetForeground(display, gc, dockBorder.pixel);
        XDrawRectangle(display, win, gc, dockLeft - 20, dockTop - 10, dockWidth + 40, DOCK_HEIGHT - 10);

        /* Dock icons */
        for (size_t i = 0; i < dockApps.size(); i++) {
            int left = dockLeft + i * (DOCK_ICON_SIZE + DOCK_ICON_GAP);
            
            // Draw dock icon with gloss effect
            drawIconWithGloss(display, win, gc, left, dockTop, DOCK_ICON_SIZE, dockApps[i].displayName, dockApps[i].hasCustomIcon);

            /* Dock labels */
            XSetForeground(display, gc, labelColor.pixel);
            std::string label = dockApps[i].displayName;
            if (label.size() > 8) label = label.substr(0, 7) + ".";
            int labelY = dockTop + DOCK_ICON_SIZE + 12;
            int textWidth = XTextWidth(font, label.c_str(), (int)label.size());
            int textX = left + (DOCK_ICON_SIZE - textWidth) / 2;
            XDrawString(display, win, gc, textX, labelY, label.c_str(), (int)label.size());
        }
    };

    bool running = true;
    while (running) {
        XEvent ev;
        XNextEvent(display, &ev);
        switch (ev.type) {
            case Expose:
                if (ev.xexpose.count == 0) draw();
                break;
            case ButtonPress:
                if (ev.xbutton.button == Button1) {
                    // Check main grid icons first
                    int col = getCol(ev.xbutton.x, width);
                    int row = getRow(ev.xbutton.y, height);
                    if (col >= 0 && row >= 0 && pointInMainIcon(ev.xbutton.x, ev.xbutton.y, col, row)) {
                        size_t idx = (size_t)(row * COLS + col);
                        if (idx < mainApps.size()) {
                            launchApp(mainApps[idx].executablePath, mainApps[idx].bundlePath);
                        }
                    } else {
                        // Check dock icons
                        int dockCol = getDockCol(ev.xbutton.x, width);
                        if (dockCol >= 0 && pointInDockIcon(ev.xbutton.x, ev.xbutton.y, dockCol, height)) {
                            if (dockCol < dockApps.size()) {
                                launchApp(dockApps[dockCol].executablePath, dockApps[dockCol].bundlePath);
                            }
                        }
                    }
                }
                break;
            case KeyPress:
                if (XLookupKeysym(&ev.xkey, 0) == XK_Escape) running = false;
                break;
        }
    }

    XFreeGC(display, gc);
    XDestroyWindow(display, win);
    XCloseDisplay(display);
    return 0;
}
