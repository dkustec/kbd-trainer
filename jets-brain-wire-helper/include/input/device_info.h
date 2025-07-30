#pragma once

#include <stdbool.h>
#include "standard_input.h"

#define MAX_DEVICE_NAME_LENGTH 128
#define MAX_CONNECTION_TYPE_LENGTH 32
#define MAX_DRIVER_NAME_LENGTH 64

// Device connection types
typedef enum {
    CONNECTION_USB,
    CONNECTION_BLUETOOTH,
    CONNECTION_WIRELESS_24GHZ,
    CONNECTION_WIRED,
    CONNECTION_UNKNOWN
} ConnectionType;

// Device capabilities
typedef struct {
    bool has_dpad;
    bool has_left_stick;
    bool has_right_stick;
    int button_count;
    bool has_rumble;
    bool has_gyro;
    bool has_touchpad;
} DeviceCapabilities;

// Device information
typedef struct {
    char device_name[MAX_DEVICE_NAME_LENGTH];
    char model[MAX_DEVICE_NAME_LENGTH];
    ConnectionType connection_type;
    char driver_name[MAX_DRIVER_NAME_LENGTH];
    
    // Status information
    bool is_connected;
    int battery_level;  // 0-100, -1 if not applicable
    float input_latency_ms;
    bool is_calibrated;
    
    DeviceCapabilities capabilities;
    
    // Technical details (for advanced users)
    uint16_t vendor_id;
    uint16_t product_id;
    char serial_number[64];
} DeviceInfo;

// Helper functions
const char* GetConnectionTypeName(ConnectionType type);
void UpdateDeviceInfo(DeviceInfo* info);
bool IsDeviceWireless(DeviceInfo* info);
const char* GetBatteryStatusString(int battery_level);
