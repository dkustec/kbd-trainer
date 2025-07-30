#include "../include/input/input_device.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool InitInputDevice(InputDevice* device)
{
    if (!device) return false;
    
    // Initialize with default values
    memset(&device->last_input, 0, sizeof(StandardInput));
    device->is_initialized = false;
    device->device_data = NULL;
    
    // Call device-specific initialization
    if (device->interface.init) {
        device->is_initialized = device->interface.init(device);
    }
    
    return device->is_initialized;
}

void DestroyInputDevice(InputDevice* device)
{
    if (!device) return;
    
    if (device->interface.cleanup) {
        device->interface.cleanup(device);
    }
    
    if (device->device_data) {
        free(device->device_data);
        device->device_data = NULL;
    }
    
    device->is_initialized = false;
}

StandardInput* PollInputDevice(InputDevice* device)
{
    if (!device || !device->is_initialized) return NULL;
    
    if (device->interface.poll) {
        StandardInput* input = device->interface.poll(device);
        if (input) {
            device->last_input = *input;
            return input;
        }
    }
    
    return NULL;
}

bool ConfigureInputDevice(InputDevice* device)
{
    if (!device || !device->is_initialized) return false;
    
    if (device->interface.configure) {
        return device->interface.configure(device);
    }
    
    return false;
}

int DetectInputDevices(InputDevice** devices, int max_devices)
{
    if (!devices || max_devices <= 0) return 0;
    
    int device_count = 0;
    
    // Try to create each device type
    if (device_count < max_devices) {
        InputDevice* keyboard = CreateKeyboardDevice();
        if (keyboard) {
            if (InitInputDevice(keyboard)) {
                devices[device_count++] = keyboard;
            } else {
                free(keyboard);
            }
        }
    }
    
    if (device_count < max_devices) {
        InputDevice* ps5 = CreatePS5Device();
        if (ps5) {
            if (InitInputDevice(ps5)) {
                devices[device_count++] = ps5;
            } else {
                free(ps5);
            }
        }
    }
    
    if (device_count < max_devices) {
        InputDevice* xbox = CreateXboxDevice();
        if (xbox) {
            if (InitInputDevice(xbox)) {
                devices[device_count++] = xbox;
            } else {
                free(xbox);
            }
        }
    }
    
    if (device_count < max_devices) {
        InputDevice* generic = CreateGenericGamepadDevice();
        if (generic) {
            if (InitInputDevice(generic)) {
                devices[device_count++] = generic;
            } else {
                free(generic);
            }
        }
    }
    
    return device_count;
}

InputDevice* SelectInputDeviceFromList(InputDevice* devices, int device_count)
{
    if (!devices || device_count <= 0) return NULL;
    
    printf("\nAvailable Input Devices:\n");
    printf("========================\n");
    
    for (int i = 0; i < device_count; i++) {
        printf("%d. %s (%s)\n", i + 1, 
               devices[i].info.device_name,
               GetConnectionTypeName(devices[i].info.connection_type));
        
        if (devices[i].info.battery_level >= 0) {
            printf("   Battery: %d%% (%s)\n", 
                   devices[i].info.battery_level,
                   GetBatteryStatusString(devices[i].info.battery_level));
        }
        
        if (devices[i].info.input_latency_ms > 0) {
            printf("   Latency: %.1fms\n", devices[i].info.input_latency_ms);
        }
        printf("\n");
    }
    
    printf("Select device (1-%d): ", device_count);
    
    int choice;
    if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= device_count) {
        return &devices[choice - 1];
    }
    
    // Default to first device
    return &devices[0];
}

void PrintDeviceInfo(DeviceInfo* info)
{
    if (!info) return;
    
    printf("\nDevice Information:\n");
    printf("==================\n");
    printf("Name: %s\n", info->device_name);
    printf("Model: %s\n", info->model);
    printf("Connection: %s\n", GetConnectionTypeName(info->connection_type));
    printf("Driver: %s\n", info->driver_name);
    printf("Connected: %s\n", info->is_connected ? "Yes" : "No");
    
    if (info->battery_level >= 0) {
        printf("Battery: %d%% (%s)\n", 
               info->battery_level, 
               GetBatteryStatusString(info->battery_level));
    }
    
    if (info->input_latency_ms > 0) {
        printf("Input Latency: %.1fms\n", info->input_latency_ms);
    }
    
    printf("Calibrated: %s\n", info->is_calibrated ? "Yes" : "No");
    
    printf("\nCapabilities:\n");
    printf("- D-Pad: %s\n", info->capabilities.has_dpad ? "Yes" : "No");
    printf("- Left Stick: %s\n", info->capabilities.has_left_stick ? "Yes" : "No");
    printf("- Right Stick: %s\n", info->capabilities.has_right_stick ? "Yes" : "No");
    printf("- Button Count: %d\n", info->capabilities.button_count);
    printf("- Rumble: %s\n", info->capabilities.has_rumble ? "Yes" : "No");
    printf("- Gyro: %s\n", info->capabilities.has_gyro ? "Yes" : "No");
    printf("- Touchpad: %s\n", info->capabilities.has_touchpad ? "Yes" : "No");
    
    if (strlen(info->serial_number) > 0) {
        printf("\nTechnical:\n");
        printf("- Vendor ID: 0x%04X\n", info->vendor_id);
        printf("- Product ID: 0x%04X\n", info->product_id);
        printf("- Serial: %s\n", info->serial_number);
    }
    
    printf("\n");
}
