# LinuxKit - I/O Framework for PocketDarwin

## Overview

LinuxKit is a lightweight I/O framework that provides the interface between Darwin applications and Linux I/O services in the PocketDarwin hybrid environment. It handles display, touchscreen, audio, USB, and battery management through a clean, unified API.

## Architecture

```
Darwin Applications (FreeBoard)
           ↓
    LinuxKit API (LK* functions)
           ↓
    Linux I/O Services (Kernel)
           ↓
    Hardware (Display, Touch, etc.)
```

## Features

### Core Components

- **Display Management** (`LKDisplay*`) - Framebuffer access and control
- **Touchscreen Input** (`LKTouchscreen*`) - Touch event handling
- **Battery Monitoring** (`LKBattery*`) - Battery status and power management
- **Audio System** (`LKAudio*`) - Audio playback and recording
- **USB Management** (`LKUSB*`) - USB device enumeration and control
- **Property Lists** (`LKPropertyList*`) - Configuration and settings management

### Key Design Principles

- **Simple API** - Clean, easy-to-use functions with `LK*` prefix
- **Cross-Platform** - Works on Darwin/Linux hybrid environment
- **Memory Safe** - Proper reference counting and error handling
- **Extensible** - Easy to add new I/O services
- **Lightweight** - Minimal overhead for embedded systems

## Usage Example

```c
#include "LinuxKit.h"

int main() {
    // Initialize LinuxKit
    if (!LKInitialize()) {
        LKLogError("Failed to initialize LinuxKit");
        return 1;
    }
    
    // Create and initialize display
    LKDisplay* display = LKDisplayCreate();
    LKDisplayInitialize(display, 800, 600, LK_DISPLAY_FORMAT_RGBA8888);
    LKDisplayEnable(display);
    
    // Get framebuffer and draw something
    uint32_t* framebuffer = (uint32_t*)LKDisplayGetFramebuffer(display);
    framebuffer[100 * 800 + 100] = 0xFFFFFFFF; // White pixel
    
    // Flush to display
    LKDisplayFlush(display);
    
    // Cleanup
    LKDisplayDestroy(display);
    LKShutdown();
    return 0;
}
```

## API Reference

### Display Management

```c
// Create and configure display
LKDisplay* display = LKDisplayCreate();
LKDisplayInitialize(display, width, height, format);
LKDisplayEnable(display);

// Access framebuffer
void* framebuffer = LKDisplayGetFramebuffer(display);
uint32_t width = LKDisplayGetWidth(display);
uint32_t height = LKDisplayGetHeight(display);

// Update display
LKDisplayFlush(display);
```

### Touchscreen Input

```c
// Initialize touchscreen
LKTouchscreen* touch = LKTouchscreenCreate();
LKTouchscreenInitialize(touch);
LKTouchscreenEnable(touch);

// Handle touch events
LKTouchPoint point;
if (LKTouchscreenGetEvent(touch, &point)) {
    printf("Touch at (%d, %d)\n", point.x, point.y);
}
```

### Battery Monitoring

```c
// Monitor battery
LKBattery* battery = LKBatteryCreate();
LKBatteryUpdateStatus(battery);

int32_t percentage = LKBatteryGetPercentage(battery);
LKBatteryStatus status = LKBatteryGetStatus(battery);
```

### Property Lists

```c
// Create and use property lists
LKPropertyList* config = LKPropertyListCreate();
LKPropertyListSetString(config, "name", "FreeBoard");
LKPropertyListSetInt32(config, "version", 1);

const char* name = LKPropertyListGetString(config, "name", "Default");
int32_t version = LKPropertyListGetInt32(config, "version", 0);
```

## Building

### Prerequisites

- C99 compatible compiler (gcc, clang)
- Standard C library
- POSIX compatibility

### Build Commands

```bash
# Build LinuxKit library
cd System/Frameworks/LinuxKit
make

# Build static library only
make liblinuxkit.a

# Build shared library
make liblinuxkit.so

# Install system-wide
make install PREFIX=/usr/local

# Clean build
make clean
```

### Integration with FreeBoard

```bash
# Build FreeBoard with LinuxKit
cd System/Applications/freeboard
./autogen.sh
./configure
make freeboard_fb
```

## Configuration

### System Configuration

LinuxKit uses property list files for configuration:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Display</key>
    <dict>
        <key>Width</key>
        <integer>800</integer>
        <key>Height</key>
        <integer>600</integer>
        <key>Format</key>
        <string>RGBA8888</string>
    </dict>
    <key>Touchscreen</key>
    <dict>
        <key>Enabled</key>
        <true/>
        <key>Calibration</key>
        <dict>
            <key>MinX</key>
            <integer>0</integer>
            <key>MaxX</key>
            <integer>800</integer>
        </dict>
    </dict>
</dict>
</plist>
```

### Runtime Configuration

```c
// Load configuration
LKPropertyList* config = LKLoadConfiguration("/etc/linuxkit/config.plist");

// Apply settings
int32_t width = LKPropertyListGetInt32(config, "Display/Width", 800);
int32_t height = LKPropertyListGetInt32(config, "Display/Height", 600);
```

## Error Handling

LinuxKit provides comprehensive error handling:

```c
// Check function return values
if (!LKDisplayInitialize(display, 800, 600, LK_DISPLAY_FORMAT_RGBA8888)) {
    LKError error = LKGetLastError();
    printf("Error: %s\n", LKGetErrorString(error));
}

// Logging
LKSetLogLevel(LK_LOG_LEVEL_DEBUG);
LKLogInfo("Application started");
LKLogError("Something went wrong");
```

## Memory Management

LinuxKit uses reference counting for automatic memory management:

```c
// Objects are reference counted
LKDisplay* display = LKDisplayCreate();  // refCount = 1

// When you're done
LKDisplayDestroy(display);  // Decrements refCount, frees if 0
```

## Threading

LinuxKit is designed to be thread-safe for read operations:

```c
// Multiple threads can safely read from the same display
void* getFramebuffer() {
    return LKDisplayGetFramebuffer(display);  // Thread-safe
}

// Write operations should be serialized
void updateDisplay() {
    // Use mutex or ensure single-threaded access for writes
    LKDisplayFlush(display);
}
```

## Platform Integration

### Linux Kernel Interface

LinuxKit communicates with the Linux kernel through:

1. **Device files** - `/dev/fb0`, `/dev/input/event0`, etc.
2. **Sysfs** - `/sys/class/power_supply/`, `/sys/class/input/`
3. **Procfs** - `/proc/`, `/sys/`
4. **IOCTL** - Direct device communication

### Darwin Integration

LinuxKit provides a clean C API that Darwin applications can use:

```c
// In Darwin application
#include "LinuxKit.h"

// The LinuxKit library handles the Darwin/Linux communication
// Applications don't need to know about the underlying implementation
```

## Testing

### Unit Tests

```bash
# Run built-in tests
make test

# Test with valgrind
valgrind --leak-check=full ./test_framework
```

### Integration Testing

```c
// Test framework integration
#include "LinuxKit.h"

void test_display() {
    LKDisplay* display = LKDisplayCreate();
    assert(display != NULL);
    
    bool result = LKDisplayInitialize(display, 800, 600, LK_DISPLAY_FORMAT_RGBA8888);
    assert(result == true);
    
    void* fb = LKDisplayGetFramebuffer(display);
    assert(fb != NULL);
    
    LKDisplayDestroy(display);
}
```

## Performance Considerations

### Optimizations

- **Direct framebuffer access** - No copying overhead
- **Minimal allocations** - Reuse objects when possible
- **Efficient event handling** - Batch processing of events
- **Lazy initialization** - Initialize components only when needed

### Memory Usage

- **Display framebuffer**: width × height × 4 bytes (RGBA8888)
- **Touchscreen events**: ~32 bytes per event
- **Battery status**: ~64 bytes total
- **Property lists**: Dynamic based on content

## Future Enhancements

### Planned Features

1. **Hardware Acceleration** - GPU acceleration for graphics
2. **Multi-Display Support** - Multiple monitor support
3. **Advanced Audio** - 3D audio, effects processing
4. **Network I/O** - WiFi, Bluetooth support
5. **Security** - Sandboxing, permissions

### Extensibility

```c
// Adding new I/O services
typedef struct LKCamera LKCamera;

LKCamera* LKCameraCreate(void);
bool LKCameraCapture(LKCamera* camera, void* buffer, size_t size);
```

## Troubleshooting

### Common Issues

**Display not working:**
```bash
# Check framebuffer device
ls -la /dev/fb*
# Check permissions
ls -la /dev/input/event*
```

**Touchscreen not responding:**
```bash
# Check input devices
cat /proc/bus/input/devices
# Test with evtest
evtest /dev/input/event0
```

**Battery status not updating:**
```bash
# Check power supply
ls -la /sys/class/power_supply/
cat /sys/class/power_supply/BAT/status
```

### Debug Mode

```c
// Enable debug logging
LKSetLogLevel(LK_LOG_LEVEL_DEBUG);

// Enable verbose output
LKLogDebug("Framebuffer address: %p", framebuffer);
LKLogDebug("Display size: %dx%d", width, height);
```

## License

LinuxKit is part of the PocketDarwin project and follows the same licensing terms.

## Contributing

When contributing to LinuxKit:

1. **Follow the coding style** - C99, `LK*` prefix for functions
2. **Add tests** - Unit tests for new functionality
3. **Update documentation** - API reference and examples
4. **Memory safety** - Proper reference counting and error handling
5. **Cross-platform** - Ensure Darwin/Linux compatibility

## Conclusion

LinuxKit provides a clean, efficient interface between Darwin applications and Linux I/O services in the PocketDarwin environment. It's designed to be lightweight, extensible, and easy to use while providing comprehensive I/O capabilities for embedded systems.
