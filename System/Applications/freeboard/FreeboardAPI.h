/*
 * FreeboardAPI.h
 * PocketDarwin freeboard - Public API for app discovery and management
 * Provides C API for external applications to interact with freeboard
 */

#ifndef FREEBOARD_API_H
#define FREEBOARD_API_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// App information structure for C API
typedef struct {
    char* displayName;
    char* bundlePath;
    char* executablePath;
    char* iconPath;
    bool hasCustomIcon;
    uint32_t pid;  // Process ID if running, 0 otherwise
} FreeboardAppInfo;

// App management operations
typedef enum {
    FREEBOARD_LAUNCH_SUCCESS = 0,
    FREEBOARD_LAUNCH_FAILED = 1,
    FREEBOARD_APP_NOT_FOUND = 2,
    FREEBOARD_PERMISSION_DENIED = 3
} FreeboardResult;

// Application discovery and enumeration
FreeboardAppInfo** freeboard_get_all_apps(int* count);
FreeboardAppInfo* freeboard_get_app_by_name(const char* name);
FreeboardAppInfo* freeboard_get_app_by_bundle_id(const char* bundleId);

// Application lifecycle management
FreeboardResult freeboard_launch_app(const char* bundlePath);
FreeboardResult freeboard_launch_app_by_name(const char* name);
bool freeboard_is_app_running(const char* bundlePath);
FreeboardResult freeboard_terminate_app(const char* bundlePath);

// Icon management
bool freeboard_app_has_icon(const char* bundlePath);
char* freeboard_get_app_icon_path(const char* bundlePath);
bool freeboard_set_app_icon(const char* bundlePath, const char* iconPath);

// Dock management (for iOS 5/6 style dock)
FreeboardAppInfo** freeboard_get_dock_apps(int* count);
bool freeboard_add_to_dock(const char* bundlePath);
bool freeboard_remove_from_dock(const char* bundlePath);
bool freeboard_is_in_dock(const char* bundlePath);

// Grid management
int freeboard_get_app_grid_position(const char* bundlePath, int* row, int* col);
bool freeboard_set_app_grid_position(const char* bundlePath, int row, int col);

// Utility functions
void freeboard_refresh_app_cache();
void freeboard_free_app_info(FreeboardAppInfo* app);
void freeboard_free_app_array(FreeboardAppInfo** apps, int count);

// System integration
void freeboard_show_notification(const char* title, const char* message);
void freeboard_set_wallpaper(const char* imagePath);
char* freeboard_get_wallpaper_path();

// System Information APIs

// Battery information structure
typedef struct {
    bool isPresent;
    bool isCharging;
    int chargePercent;    // 0-100
    int timeRemaining;    // seconds remaining, -1 if unknown
    int temperature;      // temperature in Celsius, -1 if unknown
    char* health;         // "Good", "Fair", "Poor", etc.
    char* technology;     // "Li-ion", "Li-Poly", etc.
} FreeboardBatteryInfo;

// System time structure
typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int weekday;          // 0=Sunday, 1=Monday, ..., 6=Saturday
    char* timezone;       // timezone string
    long timestamp;       // Unix timestamp
} FreeboardSystemTime;

// System information structure
typedef struct {
    char* osName;         // "PocketDarwin"
    char* osVersion;      // version string
    char* kernelVersion;  // kernel version
    char* hostname;       // system hostname
    char* architecture;   // "arm64", "x86_64", etc.
    char* cpuModel;       // CPU model name
    int cpuCores;         // number of CPU cores
    long totalMemory;     // total memory in bytes
    long availableMemory; // available memory in bytes
    long usedMemory;      // used memory in bytes
    double cpuUsage;      // CPU usage percentage (0.0-100.0)
    double uptime;        // system uptime in seconds
} FreeboardSystemInfo;

// Battery information functions
FreeboardBatteryInfo* freeboard_get_battery_info();
bool freeboard_is_battery_charging();
int freeboard_get_battery_percentage();
int freeboard_get_battery_time_remaining();
void freeboard_free_battery_info(FreeboardBatteryInfo* battery);

// System time functions
FreeboardSystemTime* freeboard_get_system_time();
char* freeboard_get_formatted_time(const char* format); // strftime format
char* freeboard_get_current_date_string();
char* freeboard_get_current_time_string();
long freeboard_get_unix_timestamp();
void freeboard_free_system_time(FreeboardSystemTime* time);

// System information functions
FreeboardSystemInfo* freeboard_get_system_info();
char* freeboard_get_os_version();
char* freeboard_get_kernel_version();
char* freeboard_get_hostname();
char* freeboard_get_architecture();
int freeboard_get_cpu_cores();
long freeboard_get_total_memory();
long freeboard_get_available_memory();
double freeboard_get_cpu_usage();
double freeboard_get_system_uptime();
void freeboard_free_system_info(FreeboardSystemInfo* info);

// Hardware information
char* freeboard_get_hardware_model();
char* freeboard_get_serial_number();
bool freeboard_is_laptop();
bool freeboard_has_battery();

// Network information
bool freeboard_is_network_available();
char* freeboard_get_ip_address();
char* freeboard_get_mac_address();
bool freeboard_is_wifi_enabled();
int freeboard_get_network_signal_strength(); // WiFi signal strength 0-100

#ifdef __cplusplus
}
#endif

#endif /* FREEBOARD_API_H */
