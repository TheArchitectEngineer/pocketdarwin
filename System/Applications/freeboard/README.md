# FreeBoard - Modern OpenGL-based Launcher

A modern, OpenGL-based launcher for PocketDarwin inspired by iOS dock design with wallpaper, transparent status bar, and blurred dock effects.

## Features

### Visual Design
- **Wallpaper Layer**: Fullscreen background with aspect-ratio-preserving scaling and center crop
- **Status Bar**: Transparent top bar with time, WiFi indicator, and battery percentage
- **Dock**: Bottom dock with real background blur, rounded corners, and semi-transparency
- **App Icons**: Clean rounded rectangles with centered glyphs and consistent spacing

### Technical Features
- **OpenGL Rendering**: Hardware-accelerated graphics with OpenGL 1.2+ compatibility
- **Blur Shaders**: Gaussian blur implementation for dock background effect
- **Constraint-based Layout**: Responsive design that adapts to resolution changes
- **Command Line Interface**: Flexible configuration options

## Building

### Prerequisites
- X11 development libraries
- OpenGL development libraries
- C++17 compatible compiler

### Build Commands
```bash
# Build the new FreeBoard
make -f Makefile.new

# Clean build files
make -f Makefile.new clean

# Debug build
make -f Makefile.new debug
```

## Usage

### Basic Usage
```bash
# Run with default settings (800x600)
./freeboard_new

# Custom resolution
./freeboard_new -r 1024x768

# With custom wallpaper
./freeboard_new -r 1280x720 -w /path/to/wallpaper.png

# Show help
./freeboard_new --help
```

### Command Line Options
- `-r, --resolution WIDTHxHEIGHT`: Set window resolution (default: 800x600)
- `-w, --wallpaper PATH`: Set wallpaper image path
- `-h, --help`: Show help message

### Controls
- **ESC**: Exit FreeBoard
- **Q**: Exit FreeBoard
- **Ctrl+C**: Force exit

## Architecture

### Core Components

#### FreeBoardUI Class
Main UI controller that orchestrates all rendering components and handles window management.

#### WallpaperRenderer
- Loads and renders fullscreen wallpaper
- Handles aspect ratio preservation with center crop
- Falls back to procedural gradient if no image provided

#### BlurShader
- OpenGL shader implementation for Gaussian blur
- Used for dock background blur effect
- Supports configurable blur radius

#### StatusBar
- Transparent status bar rendering
- Time display with automatic updates
- WiFi and battery indicators
- Minimal, clean design

#### Dock
- Rounded rectangle container with blur effect
- Semi-transparent background (60-75% opacity)
- Icon layout with consistent spacing
- Top highlight and bottom shadow

#### DockIcon
- Individual app icon rendering
- Rounded rectangle backgrounds
- Centered glyph display
- Customizable colors

### Layout System

The layout system uses constraint-based positioning:

```cpp
struct LayoutConstraints {
    int statusBarHeight = 40;
    int dockHeight = 120;
    int dockCornerRadius = 20;
    float dockOpacity = 0.7f;
    int iconSize = 60;
    int iconCornerRadius = 12;
    // ... more constraints
};
```

### Rendering Pipeline

1. **Wallpaper Layer**: Bottom layer, fullscreen background
2. **Status Bar**: Top overlay, transparent with indicators
3. **Dock**: Floating layer with blur effect
4. **Icons**: Dock children, perfectly aligned

## Design Principles

### Visual Hierarchy
1. **Wallpaper**: Background foundation
2. **Dock**: Primary interactive element
3. **Icons**: Secondary interactive elements  
4. **Status Info**: Tertiary information display

### Minimal Design
- Maximize whitespace
- Avoid visual clutter
- Muted color palette
- Clean geometric shapes
- Subtle effects only when functional

### Performance
- Single-pass rendering where possible
- Efficient texture usage
- Minimal state changes
- 60 FPS target with adaptive frame timing

## Configuration

### Default Icons
The system creates placeholder icons matching the reference image:
- **App Store** (Blue)
- **Books** (Orange)  
- **Calculator** (Gray)
- **Calendar** (White)

### Wallpaper Support
- PNG, JPEG, and other common formats
- Automatic aspect ratio preservation
- Center crop for mismatched ratios
- Procedural gradient fallback

## Development

### File Structure
```
FreeBoardUI.h          - Main UI system interface
FreeBoardUI.cpp        - Implementation of UI components
main_new.cpp          - Application entry point
stb_image.h           - Image loading library
Makefile.new          - Build configuration
```

### Extending the System

#### Adding New Icons
```cpp
dock_->addIcon(std::make_shared<DockIcon>("AppName", "#RRGGBB"));
```

#### Custom Layout Constraints
Modify `LayoutConstraints` struct to adjust spacing, sizes, and opacity.

#### Additional Shader Effects
Extend `BlurShader` class or create new shader classes for additional effects.

## Troubleshooting

### Common Issues

#### OpenGL Context Creation
Ensure X11 with GLX support is available:
```bash
# Check GLX support
glxinfo | grep "direct rendering"
```

#### Image Loading
If wallpaper fails to load, the system falls back to a procedural gradient.

#### Performance
For optimal performance:
- Use wallpaper images matching display resolution
- Avoid excessive blur radius values
- Ensure hardware acceleration is available

### Debug Mode
Build with debug symbols:
```bash
make -f Makefile.new debug
```

## License

This project is part of PocketDarwin and follows the same licensing terms.

## Contributing

When contributing to FreeBoard:
1. Maintain the minimal, clean design aesthetic
2. Follow the existing code structure
3. Add appropriate documentation
4. Test with various resolutions and wallpaper images
5. Ensure OpenGL compatibility with older hardware
