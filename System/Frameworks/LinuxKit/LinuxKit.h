/*
 * LinuxKit - I/O Framework for PocketDarwin
 * Provides interface between Darwin applications and Linux I/O services
 */

#ifndef LINUXKIT_H
#define LINUXKIT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

// Linux framebuffer includes
#ifdef __linux__
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Core LinuxKit types
typedef struct LKPropertyList LKPropertyList;
typedef struct LKDisplay LKDisplay;
typedef struct LKTouchscreen LKTouchscreen;
typedef struct LKBattery LKBattery;
typedef struct LKAudio LKAudio;
typedef struct LKUSB LKUSB;

// Display modes and formats
typedef enum {
    LK_DISPLAY_FORMAT_RGB565 = 0,
    LK_DISPLAY_FORMAT_RGB888,
    LK_DISPLAY_FORMAT_RGBA8888,
    LK_DISPLAY_FORMAT_BGRA8888
} LKDisplayFormat;

typedef enum {
    LK_DISPLAY_ORIENTATION_PORTRAIT = 0,
    LK_DISPLAY_ORIENTATION_LANDSCAPE,
    LK_DISPLAY_ORIENTATION_UPSIDE_DOWN,
    LK_DISPLAY_ORIENTATION_LANDSCAPE_RIGHT
} LKDisplayOrientation;

// Touch events
typedef enum {
    LK_TOUCH_EVENT_DOWN = 0,
    LK_TOUCH_EVENT_UP,
    LK_TOUCH_EVENT_MOVE,
    LK_TOUCH_EVENT_CANCEL
} LKTouchEvent;

typedef struct {
    uint32_t id;
    uint32_t x;
    uint32_t y;
    LKTouchEvent event;
    uint64_t timestamp;
} LKTouchPoint;

// Battery status
typedef enum {
    LK_BATTERY_STATUS_UNKNOWN = 0,
    LK_BATTERY_STATUS_CHARGING,
    LK_BATTERY_STATUS_DISCHARGING,
    LK_BATTERY_STATUS_FULL,
    LK_BATTERY_STATUS_NOT_CHARGING
} LKBatteryStatus;

// Audio formats
typedef enum {
    LK_AUDIO_FORMAT_PCM_U8 = 0,
    LK_AUDIO_FORMAT_PCM_S16,
    LK_AUDIO_FORMAT_PCM_S32,
    LK_AUDIO_FORMAT_FLOAT32
} LKAudioFormat;

// USB device types
typedef enum {
    LK_USB_TYPE_UNKNOWN = 0,
    LK_USB_TYPE_STORAGE,
    LK_USB_TYPE_HID,
    LK_USB_TYPE_AUDIO,
    LK_USB_TYPE_SERIAL,
    LK_USB_TYPE_HUB
} LKUSBType;

// ============================================================================
// Property List (Plist) Management
// ============================================================================

// Create and destroy property lists
LKPropertyList* LKPropertyListCreate(void);
LKPropertyList* LKPropertyListCreateFromFile(const char* path);
void LKPropertyListDestroy(LKPropertyList* plist);

// Property list operations
bool LKPropertyListWriteToFile(LKPropertyList* plist, const char* path);
bool LKPropertyListHasKey(LKPropertyList* plist, const char* key);

// String operations
const char* LKPropertyListGetString(LKPropertyList* plist, const char* key, const char* defaultValue);
bool LKPropertyListSetString(LKPropertyList* plist, const char* key, const char* value);

// Integer operations
int32_t LKPropertyListGetInt32(LKPropertyList* plist, const char* key, int32_t defaultValue);
bool LKPropertyListSetInt32(LKPropertyList* plist, const char* key, int32_t value);
int64_t LKPropertyListGetInt64(LKPropertyList* plist, const char* key, int64_t defaultValue);
bool LKPropertyListSetInt64(LKPropertyList* plist, const char* key, int64_t value);

// Boolean operations
bool LKPropertyListGetBool(LKPropertyList* plist, const char* key, bool defaultValue);
bool LKPropertyListSetBool(LKPropertyList* plist, const char* key, bool value);

// Data operations
const void* LKPropertyListGetData(LKPropertyList* plist, const char* key, size_t* size);
bool LKPropertyListSetData(LKPropertyList* plist, const char* key, const void* data, size_t size);

// ============================================================================
// Display Management
// ============================================================================

// Display initialization and cleanup
LKDisplay* LKDisplayCreate(void);
void LKDisplayDestroy(LKDisplay* display);

// Display configuration
bool LKDisplayInitialize(LKDisplay* display, uint32_t width, uint32_t height, LKDisplayFormat format);
bool LKDisplaySetMode(LKDisplay* display, uint32_t width, uint32_t height, LKDisplayFormat format);
bool LKDisplaySetOrientation(LKDisplay* display, LKDisplayOrientation orientation);

// Framebuffer access
void* LKDisplayGetFramebuffer(LKDisplay* display);
uint32_t LKDisplayGetWidth(LKDisplay* display);
uint32_t LKDisplayGetHeight(LKDisplay* display);
uint32_t LKDisplayGetPitch(LKDisplay* display);
LKDisplayFormat LKDisplayGetFormat(LKDisplay* display);

// Display control
bool LKDisplayEnable(LKDisplay* display);
bool LKDisplayDisable(LKDisplay* display);
bool LKDisplayIsEnabled(LKDisplay* display);
void LKDisplayFlush(LKDisplay* display);

// Display information
LKPropertyList* LKDisplayGetInfo(LKDisplay* display);

// ============================================================================
// Touchscreen Management
// ============================================================================

// Touchscreen initialization
LKTouchscreen* LKTouchscreenCreate(void);
void LKTouchscreenDestroy(LKTouchscreen* touchscreen);

// Touchscreen configuration
bool LKTouchscreenInitialize(LKTouchscreen* touchscreen);
bool LKTouchscreenEnable(LKTouchscreen* touchscreen);
bool LKTouchscreenDisable(LKTouchscreen* touchscreen);

// Touch event handling
bool LKTouchscreenGetEvent(LKTouchscreen* touchscreen, LKTouchPoint* point);
bool LKTouchscreenHasEvent(LKTouchscreen* touchscreen);
void LKTouchscreenFlushEvents(LKTouchscreen* touchscreen);

// Touchscreen calibration
bool LKTouchscreenCalibrate(LKTouchscreen* touchscreen, const LKPropertyList* calibration);
LKPropertyList* LKTouchscreenGetCalibration(LKTouchscreen* touchscreen);

// ============================================================================
// Battery Management
// ============================================================================

// Battery initialization
LKBattery* LKBatteryCreate(void);
void LKBatteryDestroy(LKBattery* battery);

// Battery status
bool LKBatteryInitialize(LKBattery* battery);
bool LKBatteryUpdateStatus(LKBattery* battery);
int32_t LKBatteryGetPercentage(LKBattery* battery);
LKBatteryStatus LKBatteryGetStatus(LKBattery* battery);
int32_t LKBatteryGetVoltage(LKBattery* battery);
int32_t LKBatteryGetTemperature(LKBattery* battery);
bool LKBatteryIsPresent(LKBattery* battery);

// Battery information
LKPropertyList* LKBatteryGetInfo(LKBattery* battery);

// ============================================================================
// Audio Management
// ============================================================================

// Audio initialization
LKAudio* LKAudioCreate(void);
void LKAudioDestroy(LKAudio* audio);

// Audio configuration
bool LKAudioInitialize(LKAudio* audio);
bool LKAudioSetFormat(LKAudio* audio, LKAudioFormat format, uint32_t sampleRate, uint32_t channels);
bool LKAudioSetVolume(LKAudio* audio, float volume);

// Audio playback
bool LKAudioPlay(LKAudio* audio, const void* data, size_t size);
bool LKAudioStop(LKAudio* audio);
bool LKAudioPause(LKAudio* audio);
bool LKAudioResume(LKAudio* audio);
bool LKAudioIsPlaying(LKAudio* audio);

// Audio recording
bool LKAudioStartRecording(LKAudio* audio);
bool LKAudioStopRecording(LKAudio* audio);
size_t LKAudioGetRecordedData(LKAudio* audio, void* buffer, size_t maxSize);

// ============================================================================
// USB Management
// ============================================================================

// USB initialization
LKUSB* LKUSBCreate(void);
void LKUSBDestroy(LKUSB* usb);

// USB enumeration
bool LKUSBInitialize(LKUSB* usb);
uint32_t LKUSBGetDeviceCount(LKUSB* usb);
LKPropertyList* LKUSBGetDeviceInfo(LKUSB* usb, uint32_t deviceIndex);
LKUSBType LKUSBGetDeviceType(LKUSB* usb, uint32_t deviceIndex);

// USB device control
bool LKUSBConnectDevice(LKUSB* usb, uint32_t deviceIndex);
bool LKUSBDisconnectDevice(LKUSB* usb, uint32_t deviceIndex);
bool LKUSBIsDeviceConnected(LKUSB* usb, uint32_t deviceIndex);

// USB data transfer
size_t LKUSBRead(LKUSB* usb, uint32_t deviceIndex, void* buffer, size_t size);
size_t LKUSBWrite(LKUSB* usb, uint32_t deviceIndex, const void* buffer, size_t size);

// ============================================================================
// System Integration
// ============================================================================

// Global LinuxKit initialization
bool LKInitialize(void);
void LKShutdown(void);

// System information
LKPropertyList* LKGetSystemInfo(void);
LKPropertyList* LKGetHardwareInfo(void);

// Event handling
typedef enum {
    LK_EVENT_DISPLAY_CONNECT = 0,
    LK_EVENT_DISPLAY_DISCONNECT,
    LK_EVENT_TOUCH,
    LK_EVENT_BATTERY_CHANGE,
    LK_EVENT_USB_CONNECT,
    LK_EVENT_USB_DISCONNECT,
    LK_EVENT_AUDIO_STATE_CHANGE
} LKEventType;

typedef struct {
    LKEventType type;
    uint64_t timestamp;
    void* data;
} LKEvent;

bool LKGetEvent(LKEvent* event);
bool LKHasEvent(void);
void LKFlushEvents(void);

// ============================================================================
// Configuration and Settings
// ============================================================================

// Load system configuration
LKPropertyList* LKLoadConfiguration(const char* configPath);
bool LKSaveConfiguration(LKPropertyList* config, const char* configPath);

// Device-specific settings
LKPropertyList* LKLoadDeviceSettings(const char* deviceType);
bool LKSaveDeviceSettings(const char* deviceType, LKPropertyList* settings);

// ============================================================================
// Debug and Logging
// ============================================================================

typedef enum {
    LK_LOG_LEVEL_ERROR = 0,
    LK_LOG_LEVEL_WARN,
    LK_LOG_LEVEL_INFO,
    LK_LOG_LEVEL_DEBUG
} LKLogLevel;

void LKSetLogLevel(LKLogLevel level);
void LKLog(LKLogLevel level, const char* format, ...);
void LKLogError(const char* format, ...);
void LKLogWarn(const char* format, ...);
void LKLogInfo(const char* format, ...);
void LKLogDebug(const char* format, ...);

// ============================================================================
// Error Handling
// ============================================================================

typedef enum {
    LK_ERROR_NONE = 0,
    LK_ERROR_INVALID_PARAMETER,
    LK_ERROR_OUT_OF_MEMORY,
    LK_ERROR_DEVICE_NOT_FOUND,
    LK_ERROR_PERMISSION_DENIED,
    LK_ERROR_IO_ERROR,
    LK_ERROR_TIMEOUT,
    LK_ERROR_NOT_SUPPORTED,
    LK_ERROR_BUSY,
    LK_ERROR_UNKNOWN
} LKError;

const char* LKGetErrorString(LKError error);
LKError LKGetLastError(void);

#ifdef __cplusplus
}
#endif

#endif // LINUXKIT_H
