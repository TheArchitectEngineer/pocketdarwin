# Freeboard - PocketDarwin SpringBoard Clone

Freeboard is an iOS 5/6 style application launcher for PocketDarwin, featuring skeuomorphic design, rounded icons, and a working API.

## Features

### iOS 5/6 Design
- **Skeuomorphic Interface**: Authentic iOS 5/6 styling with glossy icons and shadows
- **Rounded Icons**: All app icons feature rounded corners with gloss effects
- **App Grid**: 4x4 main app grid layout
- **Dock**: 4-icon dock at the bottom with black background
- **Status Bar**: Dark gray status bar matching iOS 5/6 aesthetic

### Icon Support
- **PNG Icons**: Uses `.png` files instead of `.icns` files
- **Automatic Detection**: Scans for common icon names (icon.png, Icon.png, AppIcon.png, etc.)
- **Fallback**: Displays first letter of app name when no custom icon is available
- **Gloss Effect**: Realistic iOS 5/6 style gloss overlay on all icons

### Working API
Freeboard provides a comprehensive C API for application management:

```c
#include "FreeboardAPI.h"

// Get all applications
int count;
FreeboardAppInfo** apps = freeboard_get_all_apps(&count);

// Launch an app by name
FreeboardResult result = freeboard_launch_app_by_name("Calculator");

// Check if app has custom icon
bool hasIcon = freeboard_app_has_icon("/Applications/Calculator.app");

// Get dock apps
int dockCount;
FreeboardAppInfo** dockApps = freeboard_get_dock_apps(&dockCount);
```

### API Functions

#### App Discovery
- `freeboard_get_all_apps()` - Get all installed applications
- `freeboard_get_app_by_name()` - Find app by display name
- `freeboard_get_app_by_bundle_id()` - Find app by bundle identifier

#### App Management
- `freeboard_launch_app()` - Launch app by bundle path
- `freeboard_launch_app_by_name()` - Launch app by name
- `freeboard_is_app_running()` - Check if app is running
- `freeboard_terminate_app()` - Terminate running app

#### Icon Management
- `freeboard_app_has_icon()` - Check if app has custom icon
- `freeboard_get_app_icon_path()` - Get path to app's PNG icon
- `freeboard_set_app_icon()` - Set custom icon for app

#### Dock Management
- `freeboard_get_dock_apps()` - Get apps in the dock
- `freeboard_add_to_dock()` - Add app to dock
- `freeboard_remove_from_dock()` - Remove app from dock
- `freeboard_is_in_dock()` - Check if app is in dock

#### Grid Management
- `freeboard_get_app_grid_position()` - Get app's grid position
- `freeboard_set_app_grid_position()` - Set app's grid position

#### System Information APIs
- `freeboard_get_system_info()` - Get comprehensive system information
- `freeboard_get_os_version()` - Get OS version string
- `freeboard_get_kernel_version()` - Get kernel version
- `freeboard_get_hostname()` - Get system hostname
- `freeboard_get_architecture()` - Get system architecture (x86_64, arm64)
- `freeboard_get_cpu_cores()` - Get number of CPU cores
- `freeboard_get_total_memory()` - Get total system memory in bytes
- `freeboard_get_available_memory()` - Get available memory in bytes
- `freeboard_get_cpu_usage()` - Get current CPU usage percentage
- `freeboard_get_system_uptime()` - Get system uptime in seconds

#### Battery Information APIs
- `freeboard_get_battery_info()` - Get comprehensive battery information
- `freeboard_is_battery_charging()` - Check if battery is charging
- `freeboard_get_battery_percentage()` - Get battery charge percentage (0-100)
- `freeboard_get_battery_time_remaining()` - Get time remaining in seconds
- `freeboard_has_battery()` - Check if system has a battery

#### System Time APIs
- `freeboard_get_system_time()` - Get detailed system time information
- `freeboard_get_formatted_time()` - Get time formatted with strftime pattern
- `freeboard_get_current_date_string()` - Get current date as "YYYY-MM-DD"
- `freeboard_get_current_time_string()` - Get current time as "HH:MM:SS"
- `freeboard_get_unix_timestamp()` - Get current Unix timestamp

#### Hardware Information APIs
- `freeboard_get_hardware_model()` - Get hardware model name
- `freeboard_get_serial_number()` - Get system serial number
- `freeboard_is_laptop()` - Check if system is a laptop
- `freeboard_has_battery()` - Check if system has battery

#### Network Information APIs
- `freeboard_is_network_available()` - Check if network is available
- `freeboard_get_ip_address()` - Get primary IP address
- `freeboard_get_mac_address()` - Get MAC address
- `freeboard_is_wifi_enabled()` - Check if WiFi is enabled
- `freeboard_get_network_signal_strength()` - Get WiFi signal strength (0-100)

## Building

```bash
cd /Volumes/FileSystemMac/Developer/Projects/PocketDarwin/System/Applications/freeboard
make
```

## Usage

Run the compiled binary:
```bash
./freeboard
```

### Controls
- **Click** on any app icon to launch it
- **ESC** key to exit freeboard

## Icon Guidelines

To add custom icons to your applications:

1. Place PNG icons in the `Contents/Resources/` directory of your `.app` bundle
2. Use one of these naming conventions:
   - `icon.png`
   - `Icon.png`
   - `AppIcon.png`
   - `app-icon.png`
   - `[AppName].png` (where [AppName] is your app's name)

3. Recommended icon size: 512x512 pixels (will be scaled automatically)

## API Usage Example

```c
#include "FreeboardAPI.h"

int main() {
    // Refresh app cache
    freeboard_refresh_app_cache();
    
    // Get all apps
    int count;
    FreeboardAppInfo** apps = freeboard_get_all_apps(&count);
    
    printf("Found %d applications:\n", count);
    for (int i = 0; i < count; i++) {
        printf("- %s (%s)\n", apps[i]->displayName, apps[i]->bundlePath);
        
        if (apps[i]->hasCustomIcon) {
            printf("  Icon: %s\n", apps[i]->iconPath);
        }
        
        // Launch the first app
        if (i == 0) {
            freeboard_launch_app(apps[i]->bundlePath);
        }
    }
    
    // Clean up
    freeboard_free_app_array(apps, count);
    
    return 0;
}
```

### System Information Examples

```c
#include "FreeboardAPI.h"

void print_system_info() {
    // Get comprehensive system information
    FreeboardSystemInfo* info = freeboard_get_system_info();
    if (info) {
        printf("OS: %s %s\n", info->osName, info->osVersion);
        printf("Architecture: %s\n", info->architecture);
        printf("CPU: %s (%d cores)\n", info->cpuModel, info->cpuCores);
        printf("Memory: %ld MB total, %ld MB available\n", 
               info->totalMemory / (1024*1024), 
               info->availableMemory / (1024*1024));
        printf("CPU Usage: %.1f%%\n", info->cpuUsage);
        printf("Uptime: %.1f seconds\n", info->uptime);
        freeboard_free_system_info(info);
    }
}

void print_battery_info() {
    if (freeboard_has_battery()) {
        FreeboardBatteryInfo* battery = freeboard_get_battery_info();
        if (battery) {
            printf("Battery: %d%% %s\n", 
                   battery->chargePercent,
                   battery->isCharging ? "(charging)" : "");
            printf("Health: %s\n", battery->health);
            printf("Technology: %s\n", battery->technology);
            freeboard_free_battery_info(battery);
        }
    } else {
        printf("No battery detected\n");
    }
}

void print_time_info() {
    // Get current time
    FreeboardSystemTime* time = freeboard_get_system_time();
    if (time) {
        printf("Current time: %04d-%02d-%02d %02d:%02d:%02d\n",
               time->year, time->month, time->day,
               time->hour, time->minute, time->second);
        printf("Timezone: %s\n", time->timezone);
        freeboard_free_system_time(time);
    }
    
    // Get formatted time
    char* formatted = freeboard_get_formatted_time("%A, %B %d, %Y %I:%M %p");
    printf("Formatted: %s\n", formatted);
    free(formatted);
}

void print_network_info() {
    printf("Network: %s\n", freeboard_is_network_available() ? "Available" : "Unavailable");
    
    char* ip = freeboard_get_ip_address();
    char* mac = freeboard_get_mac_address();
    printf("IP: %s\n", ip);
    printf("MAC: %s\n", mac);
    
    if (freeboard_is_wifi_enabled()) {
        int signal = freeboard_get_network_signal_strength();
        if (signal >= 0) {
            printf("WiFi Signal: %d%%\n", signal);
        }
    }
    
    free(ip);
    free(mac);
}
```

## Technical Details

- **Language**: C++ with Objective-C components
- **Graphics**: X11 window system
- **Icon Format**: PNG with automatic scaling
- **Grid Layout**: 4x4 main grid + 4-icon dock
- **Memory Management**: Manual memory management for API users

## Compatibility

- Requires X11 server
- Compatible with PocketDarwin application bundles
- Supports standard macOS `.app` bundle structure
