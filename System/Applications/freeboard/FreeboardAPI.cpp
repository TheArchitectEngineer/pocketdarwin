/*
 * FreeboardAPI.cpp
 * PocketDarwin freeboard - Implementation of public API
 */

#include "FreeboardAPI.h"
#include "AppBundle.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include <string>
#include <ctime>
#include <sys/utsname.h>
#include <sys/sysctl.h>
#include <fstream>
#include <sstream>

// Global app cache
static std::vector<freeboard::AppInfo> g_appCache;
static bool g_cacheValid = false;

// Helper function to convert C++ AppInfo to C FreeboardAppInfo
static FreeboardAppInfo* convertAppInfo(const freeboard::AppInfo& cppApp) {
    FreeboardAppInfo* app = (FreeboardAppInfo*)malloc(sizeof(FreeboardAppInfo));
    if (!app) return nullptr;
    
    app->displayName = strdup(cppApp.displayName.c_str());
    app->bundlePath = strdup(cppApp.bundlePath.c_str());
    app->executablePath = strdup(cppApp.executablePath.c_str());
    app->iconPath = strdup(cppApp.iconPath.c_str());
    app->hasCustomIcon = cppApp.hasCustomIcon;
    app->pid = 0; // TODO: Implement process tracking
    
    return app;
}

static void refreshCacheIfNeeded() {
    if (!g_cacheValid) {
        g_appCache = freeboard::scanApplications("/Applications");
        g_cacheValid = true;
    }
}

// Application discovery and enumeration
FreeboardAppInfo** freeboard_get_all_apps(int* count) {
    if (!count) return nullptr;
    
    refreshCacheIfNeeded();
    *count = (int)g_appCache.size();
    
    if (*count == 0) return nullptr;
    
    FreeboardAppInfo** apps = (FreeboardAppInfo**)malloc(*count * sizeof(FreeboardAppInfo*));
    if (!apps) {
        *count = 0;
        return nullptr;
    }
    
    for (int i = 0; i < *count; i++) {
        apps[i] = convertAppInfo(g_appCache[i]);
    }
    
    return apps;
}

FreeboardAppInfo* freeboard_get_app_by_name(const char* name) {
    if (!name) return nullptr;
    
    refreshCacheIfNeeded();
    
    for (const auto& app : g_appCache) {
        if (app.displayName == name) {
            return convertAppInfo(app);
        }
    }
    
    return nullptr;
}

FreeboardAppInfo* freeboard_get_app_by_bundle_id(const char* bundleId) {
    if (!bundleId) return nullptr;
    
    refreshCacheIfNeeded();
    
    for (const auto& app : g_appCache) {
        // Simple check - in a real implementation, we'd parse the bundle identifier
        if (app.bundlePath.find(bundleId) != std::string::npos) {
            return convertAppInfo(app);
        }
    }
    
    return nullptr;
}

// Application lifecycle management
FreeboardResult freeboard_launch_app(const char* bundlePath) {
    if (!bundlePath) return FREEBOARD_APP_NOT_FOUND;
    
    refreshCacheIfNeeded();
    
    // Find the app by bundle path
    for (const auto& app : g_appCache) {
        if (app.bundlePath == bundlePath) {
            pid_t pid = fork();
            if (pid < 0) return FREEBOARD_LAUNCH_FAILED;
            if (pid == 0) {
                // Child process
                if (chdir(app.bundlePath.c_str()) != 0) {
                    _exit(127);
                }
                const char* argv[] = { app.executablePath.c_str(), nullptr };
                execv(app.executablePath.c_str(), const_cast<char**>(argv));
                _exit(127);
            }
            return FREEBOARD_LAUNCH_SUCCESS;
        }
    }
    
    return FREEBOARD_APP_NOT_FOUND;
}

FreeboardResult freeboard_launch_app_by_name(const char* name) {
    if (!name) return FREEBOARD_APP_NOT_FOUND;
    
    refreshCacheIfNeeded();
    
    for (const auto& app : g_appCache) {
        if (app.displayName == name) {
            return freeboard_launch_app(app.bundlePath.c_str());
        }
    }
    
    return FREEBOARD_APP_NOT_FOUND;
}

bool freeboard_is_app_running(const char* bundlePath) {
    // TODO: Implement process tracking using ps or similar
    // For now, return false
    (void)bundlePath;
    return false;
}

FreeboardResult freeboard_terminate_app(const char* bundlePath) {
    // TODO: Implement process termination
    (void)bundlePath;
    return FREEBOARD_LAUNCH_FAILED;
}

// Icon management
bool freeboard_app_has_icon(const char* bundlePath) {
    if (!bundlePath) return false;
    
    refreshCacheIfNeeded();
    
    for (const auto& app : g_appCache) {
        if (app.bundlePath == bundlePath) {
            return app.hasCustomIcon;
        }
    }
    
    return false;
}

char* freeboard_get_app_icon_path(const char* bundlePath) {
    if (!bundlePath) return nullptr;
    
    refreshCacheIfNeeded();
    
    for (const auto& app : g_appCache) {
        if (app.bundlePath == bundlePath) {
            return strdup(app.iconPath.c_str());
        }
    }
    
    return nullptr;
}

bool freeboard_set_app_icon(const char* bundlePath, const char* iconPath) {
    // TODO: Implement icon setting functionality
    (void)bundlePath;
    (void)iconPath;
    return false;
}

// Dock management
FreeboardAppInfo** freeboard_get_dock_apps(int* count) {
    if (!count) return nullptr;
    
    refreshCacheIfNeeded();
    
    // Return first 4 apps for dock (simplified)
    *count = std::min(4, (int)g_appCache.size());
    if (*count == 0) return nullptr;
    
    FreeboardAppInfo** apps = (FreeboardAppInfo**)malloc(*count * sizeof(FreeboardAppInfo*));
    if (!apps) {
        *count = 0;
        return nullptr;
    }
    
    for (int i = 0; i < *count; i++) {
        apps[i] = convertAppInfo(g_appCache[i]);
    }
    
    return apps;
}

bool freeboard_add_to_dock(const char* bundlePath) {
    // TODO: Implement dock persistence
    (void)bundlePath;
    return false;
}

bool freeboard_remove_from_dock(const char* bundlePath) {
    // TODO: Implement dock persistence
    (void)bundlePath;
    return false;
}

bool freeboard_is_in_dock(const char* bundlePath) {
    // TODO: Implement dock checking
    (void)bundlePath;
    return false;
}

// Grid management
int freeboard_get_app_grid_position(const char* bundlePath, int* row, int* col) {
    if (!bundlePath || !row || !col) return -1;
    
    refreshCacheIfNeeded();
    
    for (size_t i = 0; i < g_appCache.size() && i < 16; i++) {
        if (g_appCache[i].bundlePath == bundlePath) {
            *row = (int)(i / 4);
            *col = (int)(i % 4);
            return 0;
        }
    }
    
    return -1;
}

bool freeboard_set_app_grid_position(const char* bundlePath, int row, int col) {
    // TODO: Implement grid position persistence
    (void)bundlePath;
    (void)row;
    (void)col;
    return false;
}

// Utility functions
void freeboard_refresh_app_cache() {
    g_appCache = freeboard::scanApplications("/Applications");
    g_cacheValid = true;
}

void freeboard_free_app_info(FreeboardAppInfo* app) {
    if (!app) return;
    
    free(app->displayName);
    free(app->bundlePath);
    free(app->executablePath);
    free(app->iconPath);
    free(app);
}

void freeboard_free_app_array(FreeboardAppInfo** apps, int count) {
    if (!apps) return;
    
    for (int i = 0; i < count; i++) {
        freeboard_free_app_info(apps[i]);
    }
    
    free(apps);
}

// System integration
void freeboard_show_notification(const char* title, const char* message) {
    // TODO: Implement notification system
    (void)title;
    (void)message;
}

void freeboard_set_wallpaper(const char* imagePath) {
    // TODO: Implement wallpaper setting
    (void)imagePath;
}

char* freeboard_get_wallpaper_path() {
    // TODO: Implement wallpaper getting
    return strdup("/System/Library/Desktop Pictures/Solid Colors/Solid Gray Pro Ultra Dark.png");
}

// Battery information functions
FreeboardBatteryInfo* freeboard_get_battery_info() {
    FreeboardBatteryInfo* battery = (FreeboardBatteryInfo*)malloc(sizeof(FreeboardBatteryInfo));
    if (!battery) return nullptr;
    
    // Initialize with default values
    battery->isPresent = false;
    battery->isCharging = false;
    battery->chargePercent = -1;
    battery->timeRemaining = -1;
    battery->temperature = -1;
    battery->health = strdup("Unknown");
    battery->technology = strdup("Unknown");
    
    // Try to read battery information from system
    // On macOS, we can use system_profiler or ioreg
    FILE* pipe = popen("system_profiler SPPowerDataType -json 2>/dev/null", "r");
    if (pipe) {
        std::string output;
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            output += buffer;
        }
        pclose(pipe);
        
        // Parse JSON output (simplified parsing)
        if (output.find("\"Battery Information\"") != std::string::npos) {
            battery->isPresent = true;
            
            // Extract charge percentage
            size_t chargePos = output.find("\"Charge Remaining\"");
            if (chargePos != std::string::npos) {
                // Simple parsing - in real implementation, use proper JSON parser
                battery->chargePercent = 75; // Placeholder
            }
            
            // Extract charging state
            if (output.find("\"Battery Power\"") != std::string::npos) {
                battery->isCharging = true;
            }
            
            battery->health = strdup("Good");
            battery->technology = strdup("Li-ion");
        }
    }
    
    return battery;
}

bool freeboard_is_battery_charging() {
    FreeboardBatteryInfo* battery = freeboard_get_battery_info();
    if (!battery) return false;
    
    bool charging = battery->isCharging;
    freeboard_free_battery_info(battery);
    return charging;
}

int freeboard_get_battery_percentage() {
    FreeboardBatteryInfo* battery = freeboard_get_battery_info();
    if (!battery) return -1;
    
    int percentage = battery->chargePercent;
    freeboard_free_battery_info(battery);
    return percentage;
}

int freeboard_get_battery_time_remaining() {
    FreeboardBatteryInfo* battery = freeboard_get_battery_info();
    if (!battery) return -1;
    
    int timeRemaining = battery->timeRemaining;
    freeboard_free_battery_info(battery);
    return timeRemaining;
}

void freeboard_free_battery_info(FreeboardBatteryInfo* battery) {
    if (!battery) return;
    
    free(battery->health);
    free(battery->technology);
    free(battery);
}

// System time functions
FreeboardSystemTime* freeboard_get_system_time() {
    FreeboardSystemTime* sysTime = (FreeboardSystemTime*)malloc(sizeof(FreeboardSystemTime));
    if (!sysTime) return nullptr;
    
    time_t now = time(nullptr);
    struct tm* localTime = localtime(&now);
    
    sysTime->year = localTime->tm_year + 1900;
    sysTime->month = localTime->tm_mon + 1;
    sysTime->day = localTime->tm_mday;
    sysTime->hour = localTime->tm_hour;
    sysTime->minute = localTime->tm_min;
    sysTime->second = localTime->tm_sec;
    sysTime->weekday = localTime->tm_wday;
    sysTime->timestamp = (long)now;
    
    // Get timezone
    char* tz = getenv("TZ");
    if (tz) {
        sysTime->timezone = strdup(tz);
    } else {
        sysTime->timezone = strdup("UTC");
    }
    
    return sysTime;
}

char* freeboard_get_formatted_time(const char* format) {
    if (!format) return nullptr;
    
    time_t now = time(nullptr);
    struct tm* localTime = localtime(&now);
    
    char buffer[256];
    strftime(buffer, sizeof(buffer), format, localTime);
    
    return strdup(buffer);
}

char* freeboard_get_current_date_string() {
    return freeboard_get_formatted_time("%Y-%m-%d");
}

char* freeboard_get_current_time_string() {
    return freeboard_get_formatted_time("%H:%M:%S");
}

long freeboard_get_unix_timestamp() {
    return (long)time(nullptr);
}

void freeboard_free_system_time(FreeboardSystemTime* time) {
    if (!time) return;
    
    free(time->timezone);
    free(time);
}

// System information functions
FreeboardSystemInfo* freeboard_get_system_info() {
    FreeboardSystemInfo* info = (FreeboardSystemInfo*)malloc(sizeof(FreeboardSystemInfo));
    if (!info) return nullptr;
    
    // Get system information using uname
    struct utsname sysInfo;
    if (uname(&sysInfo) == 0) {
        info->osName = strdup("PocketDarwin");
        info->osVersion = strdup(sysInfo.release);
        info->kernelVersion = strdup(sysInfo.version);
        info->hostname = strdup(sysInfo.nodename);
        info->architecture = strdup(sysInfo.machine);
    } else {
        info->osName = strdup("PocketDarwin");
        info->osVersion = strdup("Unknown");
        info->kernelVersion = strdup("Unknown");
        info->hostname = strdup("Unknown");
        info->architecture = strdup("Unknown");
    }
    
    // Get CPU information
    info->cpuModel = strdup("Apple Silicon"); // Placeholder
    info->cpuCores = (int)sysconf(_SC_NPROCESSORS_ONLN);
    
    // Get memory information
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    info->totalMemory = pages * page_size;
    info->availableMemory = info->totalMemory / 2; // Placeholder
    info->usedMemory = info->totalMemory - info->availableMemory;
    
    // Get CPU usage and uptime (simplified)
    info->cpuUsage = 25.0; // Placeholder
    info->uptime = 3600.0; // Placeholder
    
    return info;
}

char* freeboard_get_os_version() {
    struct utsname sysInfo;
    if (uname(&sysInfo) == 0) {
        return strdup(sysInfo.release);
    }
    return strdup("Unknown");
}

char* freeboard_get_kernel_version() {
    struct utsname sysInfo;
    if (uname(&sysInfo) == 0) {
        return strdup(sysInfo.version);
    }
    return strdup("Unknown");
}

char* freeboard_get_hostname() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return strdup(hostname);
    }
    return strdup("Unknown");
}

char* freeboard_get_architecture() {
    struct utsname sysInfo;
    if (uname(&sysInfo) == 0) {
        return strdup(sysInfo.machine);
    }
    return strdup("Unknown");
}

int freeboard_get_cpu_cores() {
    return (int)sysconf(_SC_NPROCESSORS_ONLN);
}

long freeboard_get_total_memory() {
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}

long freeboard_get_available_memory() {
    // Simplified - in real implementation, read from /proc/meminfo or sysctl
    return freeboard_get_total_memory() / 2;
}

double freeboard_get_cpu_usage() {
    // Placeholder - in real implementation, calculate from /proc/stat or similar
    return 25.0;
}

double freeboard_get_system_uptime() {
    // Get uptime in seconds
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    }
    return 0.0;
}

void freeboard_free_system_info(FreeboardSystemInfo* info) {
    if (!info) return;
    
    free(info->osName);
    free(info->osVersion);
    free(info->kernelVersion);
    free(info->hostname);
    free(info->architecture);
    free(info->cpuModel);
    free(info);
}

// Hardware information
char* freeboard_get_hardware_model() {
    // Try to get hardware model from system
    FILE* pipe = popen("system_profiler SPHardwareDataType -json 2>/dev/null | grep 'Model Name' | head -1", "r");
    if (pipe) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            pclose(pipe);
            // Parse the model name from output
            std::string output(buffer);
            size_t pos = output.find("\"");
            if (pos != std::string::npos) {
                pos = output.find("\"", pos + 1);
                if (pos != std::string::npos) {
                    pos++;
                    size_t endPos = output.find("\"", pos);
                    if (endPos != std::string::npos) {
                        return strdup(output.substr(pos, endPos - pos).c_str());
                    }
                }
            }
        } else {
            pclose(pipe);
        }
    }
    
    return strdup("Unknown");
}

char* freeboard_get_serial_number() {
    // Try to get serial number
    FILE* pipe = popen("system_profiler SPHardwareDataType -json 2>/dev/null | grep 'Serial Number' | head -1", "r");
    if (pipe) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            pclose(pipe);
            // Parse serial number
            std::string output(buffer);
            size_t pos = output.find("\"");
            if (pos != std::string::npos) {
                pos = output.find("\"", pos + 1);
                if (pos != std::string::npos) {
                    pos++;
                    size_t endPos = output.find("\"", pos);
                    if (endPos != std::string::npos) {
                        return strdup(output.substr(pos, endPos - pos).c_str());
                    }
                }
            }
        } else {
            pclose(pipe);
        }
    }
    
    return strdup("Unknown");
}

bool freeboard_is_laptop() {
    // Check if it's a laptop by looking for battery
    return freeboard_has_battery();
}

bool freeboard_has_battery() {
    FreeboardBatteryInfo* battery = freeboard_get_battery_info();
    if (!battery) return false;
    
    bool hasBattery = battery->isPresent;
    freeboard_free_battery_info(battery);
    return hasBattery;
}

// Network information
bool freeboard_is_network_available() {
    // Simple network check by pinging a reliable host
    int result = system("ping -c 1 8.8.8.8 >/dev/null 2>&1");
    return result == 0;
}

char* freeboard_get_ip_address() {
    FILE* pipe = popen("ifconfig | grep 'inet ' | grep -v '127.0.0.1' | head -1 | awk '{print $2}'", "r");
    if (pipe) {
        char buffer[64];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            pclose(pipe);
            buffer[strcspn(buffer, "\n")] = 0; // Remove newline
            return strdup(buffer);
        }
        pclose(pipe);
    }
    return strdup("127.0.0.1");
}

char* freeboard_get_mac_address() {
    FILE* pipe = popen("ifconfig en0 | grep 'ether' | awk '{print $2}'", "r");
    if (pipe) {
        char buffer[32];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            pclose(pipe);
            buffer[strcspn(buffer, "\n")] = 0; // Remove newline
            return strdup(buffer);
        }
        pclose(pipe);
    }
    return strdup("00:00:00:00:00:00");
}

bool freeboard_is_wifi_enabled() {
    int result = system("ifconfig en0 | grep 'status: active' >/dev/null 2>&1");
    return result == 0;
}

int freeboard_get_network_signal_strength() {
    // Get WiFi signal strength (macOS specific)
    FILE* pipe = popen("/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -I | grep 'agrCtlRSSI' | awk '{print $2}'", "r");
    if (pipe) {
        char buffer[16];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            pclose(pipe);
            int rssi = atoi(buffer);
            // Convert RSSI to percentage (rough approximation)
            // RSSI typically ranges from -30 (excellent) to -100 (poor)
            int percentage = ((rssi + 100) * 100) / 70;
            if (percentage < 0) percentage = 0;
            if (percentage > 100) percentage = 100;
            return percentage;
        }
        pclose(pipe);
    }
    return -1; // Unknown
}
