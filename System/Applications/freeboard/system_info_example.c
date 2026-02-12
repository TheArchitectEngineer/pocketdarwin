/*
 * system_info_example.c
 * Example program demonstrating Freeboard system information APIs
 */

#include "FreeboardAPI.h"
#include <stdio.h>
#include <stdlib.h>

void print_battery_info() {
    printf("=== Battery Information ===\n");
    
    if (freeboard_has_battery()) {
        FreeboardBatteryInfo* battery = freeboard_get_battery_info();
        if (battery) {
            printf("Battery Present: %s\n", battery->isPresent ? "Yes" : "No");
            printf("Charging: %s\n", battery->isCharging ? "Yes" : "No");
            printf("Charge: %d%%\n", battery->chargePercent);
            printf("Time Remaining: %d seconds\n", battery->timeRemaining);
            printf("Health: %s\n", battery->health);
            printf("Technology: %s\n", battery->technology);
            freeboard_free_battery_info(battery);
        }
    } else {
        printf("No battery detected\n");
    }
    printf("\n");
}

void print_system_time() {
    printf("=== System Time ===\n");
    
    FreeboardSystemTime* time = freeboard_get_system_time();
    if (time) {
        printf("Date: %04d-%02d-%02d\n", time->year, time->month, time->day);
        printf("Time: %02d:%02d:%02d\n", time->hour, time->minute, time->second);
        printf("Weekday: %d (0=Sunday, 6=Saturday)\n", time->weekday);
        printf("Timezone: %s\n", time->timezone);
        printf("Unix Timestamp: %ld\n", time->timestamp);
        freeboard_free_system_time(time);
    }
    
    // Show formatted time examples
    char* dateStr = freeboard_get_current_date_string();
    char* timeStr = freeboard_get_current_time_string();
    char* formatted = freeboard_get_formatted_time("%A, %B %d, %Y %I:%M %p");
    
    printf("Current Date: %s\n", dateStr);
    printf("Current Time: %s\n", timeStr);
    printf("Formatted: %s\n", formatted);
    
    free(dateStr);
    free(timeStr);
    free(formatted);
    printf("\n");
}

void print_system_info() {
    printf("=== System Information ===\n");
    
    FreeboardSystemInfo* info = freeboard_get_system_info();
    if (info) {
        printf("OS Name: %s\n", info->osName);
        printf("OS Version: %s\n", info->osVersion);
        printf("Kernel Version: %s\n", info->kernelVersion);
        printf("Hostname: %s\n", info->hostname);
        printf("Architecture: %s\n", info->architecture);
        printf("CPU Model: %s\n", info->cpuModel);
        printf("CPU Cores: %d\n", info->cpuCores);
        printf("Total Memory: %ld MB\n", info->totalMemory / (1024 * 1024));
        printf("Available Memory: %ld MB\n", info->availableMemory / (1024 * 1024));
        printf("Used Memory: %ld MB\n", info->usedMemory / (1024 * 1024));
        printf("CPU Usage: %.1f%%\n", info->cpuUsage);
        printf("Uptime: %.1f seconds\n", info->uptime);
        freeboard_free_system_info(info);
    }
    printf("\n");
}

void print_hardware_info() {
    printf("=== Hardware Information ===\n");
    
    char* model = freeboard_get_hardware_model();
    char* serial = freeboard_get_serial_number();
    
    printf("Hardware Model: %s\n", model);
    printf("Serial Number: %s\n", serial);
    printf("Is Laptop: %s\n", freeboard_is_laptop() ? "Yes" : "No");
    printf("Has Battery: %s\n", freeboard_has_battery() ? "Yes" : "No");
    
    free(model);
    free(serial);
    printf("\n");
}

void print_network_info() {
    printf("=== Network Information ===\n");
    
    printf("Network Available: %s\n", freeboard_is_network_available() ? "Yes" : "No");
    
    char* ip = freeboard_get_ip_address();
    char* mac = freeboard_get_mac_address();
    
    printf("IP Address: %s\n", ip);
    printf("MAC Address: %s\n", mac);
    printf("WiFi Enabled: %s\n", freeboard_is_wifi_enabled() ? "Yes" : "No");
    
    int signal = freeboard_get_network_signal_strength();
    if (signal >= 0) {
        printf("WiFi Signal Strength: %d%%\n", signal);
    } else {
        printf("WiFi Signal Strength: Unknown\n");
    }
    
    free(ip);
    free(mac);
    printf("\n");
}

int main() {
    printf("Freeboard System Information API Demo\n");
    printf("=====================================\n\n");
    
    // Refresh app cache (not needed for system info, but good practice)
    freeboard_refresh_app_cache();
    
    // Print all system information
    print_system_time();
    print_battery_info();
    print_system_info();
    print_hardware_info();
    print_network_info();
    
    printf("Demo completed successfully!\n");
    return 0;
}
