#pragma once

#include <stdbool.h>
#include "standard_input.h"
#include "device_info.h"

// Forward declaration
struct InputDevice;

// Device interface function pointers
typedef struct {
    bool (*init)(struct InputDevice* device);
    StandardInput* (*poll)(struct InputDevice* device);
    void (*cleanup)(struct InputDevice* device);
    bool (*configure)(struct InputDevice* device);  // User customization UI
    void (*update_info)(struct InputDevice* device);  // Update device info
} DeviceInterface;

// Input device structure
typedef struct InputDevice {
    DeviceInfo info;
    DeviceInterface interface;
    void* device_data;  // Device-specific data
    bool is_initialized;
    StandardInput last_input;
} InputDevice;

// Device management
bool InitInputDevice(InputDevice* device);
void DestroyInputDevice(InputDevice* device);
StandardInput* PollInputDevice(InputDevice* device);
bool ConfigureInputDevice(InputDevice* device);

// Device detection and enumeration
int DetectInputDevices(InputDevice** devices, int max_devices);
InputDevice* CreateKeyboardDevice(void);
InputDevice* CreatePS5Device(void);
InputDevice* CreateXboxDevice(void);
InputDevice* CreateGenericGamepadDevice(void);

// Device selection
InputDevice* SelectInputDeviceFromList(InputDevice* devices, int device_count);
void PrintDeviceInfo(DeviceInfo* info);
