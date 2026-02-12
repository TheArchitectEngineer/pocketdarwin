/*
 * freeboard - PocketDarwin SpringBoard clone (iOS 7–like)
 * X11, single foreground apps, scans /Applications for .app bundles.
 * No animations, no blur. C++ and Objective-C with libX11.
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

namespace {

const int ICON_SIZE = 80;
const int ICON_GAP = 24;
const int PADDING = 32;
const int STATUS_BAR_HEIGHT = 44;
const int COLS = 4;
const int LABEL_HEIGHT = 20;
const int CELL_HEIGHT = ICON_SIZE + ICON_GAP + LABEL_HEIGHT;
const int CELL_WIDTH = ICON_SIZE + ICON_GAP;

/* iOS 7–like colors (RGB) */
const unsigned long STATUS_BG   = 0xF2F2F7;  /*rgb(149, 149, 150) */
const unsigned long SCREEN_BG   = 0xE5E5EA;  /*rgb(63, 63, 68) */
const unsigned long ICON_BG     = 0x5AC8FA;  /* #5AC8FA blue */
const unsigned long LABEL_COLOR = 0x1C1C1E;  /*rgb(219, 219, 226) */
const unsigned long STATUS_TEXT = 0x1C1C1E;

} // namespace

static unsigned long rgbToPixel(Display* dpy, int screen, unsigned long rgb) {
    unsigned long r = (rgb >> 16) & 0xff;
    unsigned long g = (rgb >> 8) & 0xff;
    unsigned long b = rgb & 0xff;
    return (r << 16) | (g << 8) | b;
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
    if (y < top) return -1;
    int row = (y - top) / CELL_HEIGHT;
    return row;
}

static bool pointInIcon(int x, int y, int col, int row) {
    int left = PADDING + col * CELL_WIDTH;
    int top = STATUS_BAR_HEIGHT + PADDING + row * CELL_HEIGHT;
    return x >= left && x < left + ICON_SIZE && y >= top && y < top + ICON_SIZE;
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
    XColor statusBg, screenBg, iconBg, labelColor, statusText;
    statusBg.red   = (STATUS_BG >> 16) * 257; statusBg.green = ((STATUS_BG >> 8) & 0xff) * 257; statusBg.blue = (STATUS_BG & 0xff) * 257;
    screenBg.red   = (SCREEN_BG >> 16) * 257; screenBg.green = ((SCREEN_BG >> 8) & 0xff) * 257; screenBg.blue = (SCREEN_BG & 0xff) * 257;
    iconBg.red     = (ICON_BG >> 16) * 257;   iconBg.green   = ((ICON_BG >> 8) & 0xff) * 257;   iconBg.blue   = (ICON_BG & 0xff) * 257;
    labelColor.red = (LABEL_COLOR >> 16) * 257; labelColor.green = ((LABEL_COLOR >> 8) & 0xff) * 257; labelColor.blue = (LABEL_COLOR & 0xff) * 257;
    statusText.red = (STATUS_TEXT >> 16) * 257; statusText.green = ((STATUS_TEXT >> 8) & 0xff) * 257; statusText.blue = (STATUS_TEXT & 0xff) * 257;
    statusBg.flags = screenBg.flags = iconBg.flags = labelColor.flags = statusText.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(display, cmap, &statusBg);
    XAllocColor(display, cmap, &screenBg);
    XAllocColor(display, cmap, &iconBg);
    XAllocColor(display, cmap, &labelColor);
    XAllocColor(display, cmap, &statusText);

    int width = PADDING * 2 + COLS * CELL_WIDTH - ICON_GAP;
    int rows = 6;
    int height = STATUS_BAR_HEIGHT + PADDING * 2 + rows * CELL_HEIGHT - ICON_GAP;

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
    int totalRows = (int)((apps.size() + COLS - 1) / COLS);
    if (totalRows > rows) {
        height = STATUS_BAR_HEIGHT + PADDING * 2 + totalRows * CELL_HEIGHT - ICON_GAP;
        XResizeWindow(display, win, (unsigned)width, (unsigned)height);
    }

    auto draw = [&]() {
        /* Status bar */
        XSetForeground(display, gc, statusBg.pixel);
        XFillRectangle(display, win, gc, 0, 0, (unsigned)width, STATUS_BAR_HEIGHT);
        XSetForeground(display, gc, statusText.pixel);
        XDrawString(display, win, gc, PADDING, STATUS_BAR_HEIGHT - 16, "PocketDarwin", 12);

        /* Grid background already from window background */
        XSetForeground(display, gc, labelColor.pixel);

        for (size_t i = 0; i < apps.size(); i++) {
            int col = (int)(i % COLS);
            int row = (int)(i / COLS);
            int left = PADDING + col * CELL_WIDTH;
            int top = STATUS_BAR_HEIGHT + PADDING + row * CELL_HEIGHT;

            /* Icon background (iOS 7 style block) */
            XSetForeground(display, gc, iconBg.pixel);
            XFillRectangle(display, win, gc, left, top, ICON_SIZE, ICON_SIZE);

            /* First letter of app name in icon */
            const std::string& name = apps[i].displayName;
            char letter = name.empty() ? '?' : (name[0] >= 'a' && name[0] <= 'z') ? (char)(name[0] - 32) : name[0];
            XSetForeground(display, gc, statusBg.pixel); /* white-ish on blue */
            int tw = font ? (int)XTextWidth(font, &letter, 1) : 8;
            XDrawString(display, win, gc, left + (ICON_SIZE - tw) / 2, top + ICON_SIZE / 2 + 4, &letter, 1);

            /* Label under icon (truncate if long) */
            XSetForeground(display, gc, labelColor.pixel);
            std::string label = name;
            if (label.size() > 10) label = label.substr(0, 9) + ".";
            int labelY = top + ICON_SIZE + 4;
            XDrawString(display, win, gc, left, labelY, label.c_str(), (int)label.size());
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
                    int col = getCol(ev.xbutton.x, width);
                    int row = getRow(ev.xbutton.y, height);
                    if (col >= 0 && row >= 0 && pointInIcon(ev.xbutton.x, ev.xbutton.y, col, row)) {
                        size_t idx = (size_t)(row * COLS + col);
                        if (idx < apps.size()) {
                            launchApp(apps[idx].executablePath, apps[idx].bundlePath);
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
