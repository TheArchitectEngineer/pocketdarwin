# freeboard

A SpringBoard-style application launcher for **PocketDarwin**: iOS 7–like home screen that runs under X11, shows a grid of apps from `/Applications`, and launches a single foreground app when you tap an icon.

## Features

- **X11 only** – Uses libX11 for display and input (no Cocoa/Wayland).
- **Single foreground app** – Clicking an icon runs the `.app` executable; the window manager handles focus.
- **Scans `/Applications`** – Discovers `.app` bundles and reads `Contents/Info.plist` for display name and executable.
- **iOS 7–style UI** – Flat, static look: light gray background, status bar, grid of icon cells with labels. No animations or blur.

## Requirements

- C and C++ compilers
- Objective-C++ compiler (for `.app` scanning)
- **libX11** (with pkg-config)
- **Foundation** (macOS/Darwin framework for plist and file APIs)

On macOS with XQuartz, install X11 and pkg-config (e.g. via Homebrew) so `pkg-config --cflags --libs x11` works.

## Build

```bash
autoreconf -fi
./configure
make
```

Install (optional):

```bash
make install
```

## Run

```bash
./freeboard
```

Ensure your X display is available (e.g. XQuartz or a PocketDarwin X session). The window shows “PocketDarwin” in the status bar and a grid of apps found in `/Applications`. Click an icon to launch that app; press **Escape** to quit freeboard.

## Layout

| File          | Purpose |
|---------------|--------|
| `main.cpp`    | X11 window, drawing, event loop, launch on click |
| `AppBundle.h` | Declares app info struct and `scanApplications()` |
| `AppScanner.mm` | Implements scanning of `/Applications` and reading `.app`/Info.plist (Objective-C + Foundation) |

## License

Part of the PocketDarwin project.
