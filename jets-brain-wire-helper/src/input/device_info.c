#include "../include/input/device_info.h"
#include <string.h>

const char* GetConnectionTypeName(ConnectionType type)
{
    switch (type) {
        case CONNECTION_USB:         return "USB";
        case CONNECTION_BLUETOOTH:   return "Bluetooth";
        case CONNECTION_WIRELESS_24GHZ: return "2.4GHz Wireless";
        case CONNECTION_WIRED:       return "Wired";
        case CONNECTION_UNKNOWN:     return "Unknown";
        default:                     return "Unknown";
    }
}

void UpdateDeviceInfo(DeviceInfo* info)
{
    if (!info) return;
    
    // This would be implemented by specific device drivers
    // For now, just ensure basic consistency
    if (!info->is_connected) {
        info->battery_level = -1;
        info->input_latency_ms = 0.0f;
    }
}

bool IsDeviceWireless(DeviceInfo* info)
{
    if (!info) return false;
    
    return (info->connection_type == CONNECTION_BLUETOOTH ||
            info->connection_type == CONNECTION_WIRELESS_24GHZ);
}

const char* GetBatteryStatusString(int battery_level)
{
    if (battery_level < 0) return "N/A";
    if (battery_level <= 15) return "Low";
    if (battery_level <= 50) return "Medium";
    return "Good";
}
