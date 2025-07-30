#include "../include/input/input_device.h"
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <string.h>

// Generic gamepad data
typedef struct {
    SDL_Gamepad* gamepad;
    SDL_JoystickID joystick_id;  // Changed from index to ID
    bool use_dpad;  // User preference: D-pad or left stick
    
    // Current input state
    StandardInput current_input;
    uint64_t direction_start_time;
    GameDirection last_direction;
} GenericData;

// Forward declarations
static bool GenericInit(InputDevice* device);
static StandardInput* GenericPoll(InputDevice* device);
static void GenericCleanup(InputDevice* device);
static bool GenericConfigure(InputDevice* device);
static void GenericUpdateInfo(InputDevice* device);

// Helper functions
static GameDirection GetDirectionFromGeneric(GenericData* generic_data);
static bool IsGenericGamepad(SDL_Gamepad* gamepad);

InputDevice* CreateGenericGamepadDevice(void)
{
    InputDevice* device = malloc(sizeof(InputDevice));
    if (!device) return NULL;
    
    // Initialize device info
    memset(&device->info, 0, sizeof(DeviceInfo));
    strcpy(device->info.device_name, "Generic Gamepad");
    strcpy(device->info.model, "Generic HID Controller");
    strcpy(device->info.driver_name, "SDL3 Gamepad");
    device->info.connection_type = CONNECTION_UNKNOWN;
    device->info.is_connected = false;
    device->info.battery_level = -1;
    device->info.input_latency_ms = 4.0f;  // Assume higher latency for generic
    device->info.is_calibrated = true;
    device->info.vendor_id = 0x0000;
    device->info.product_id = 0x0000;
    
    // Set capabilities (conservative defaults)
    device->info.capabilities.has_dpad = true;
    device->info.capabilities.has_left_stick = true;
    device->info.capabilities.has_right_stick = true;
    device->info.capabilities.button_count = 8;  // Minimum expected
    device->info.capabilities.has_rumble = false;  // Don't assume
    device->info.capabilities.has_gyro = false;
    device->info.capabilities.has_touchpad = false;
    
    // Set interface functions
    device->interface.init = GenericInit;
    device->interface.poll = GenericPoll;
    device->interface.cleanup = GenericCleanup;
    device->interface.configure = GenericConfigure;
    device->interface.update_info = GenericUpdateInfo;
    
    device->device_data = NULL;
    device->is_initialized = false;
    
    return device;
}

static bool GenericInit(InputDevice* device)
{
    if (!device) return false;
    
    // Look for any remaining gamepads not handled by specific drivers
    int count = 0;
    SDL_JoystickID* joysticks = SDL_GetJoysticks(&count);
    SDL_Gamepad* gamepad = NULL;
    SDL_JoystickID found_id = 0;
    
    if (joysticks && count > 0) {
        for (int i = 0; i < count; i++) {
            if (SDL_IsGamepad(joysticks[i])) {
                SDL_Gamepad* test_gamepad = SDL_OpenGamepad(joysticks[i]);
                if (test_gamepad && IsGenericGamepad(test_gamepad)) {
                    gamepad = test_gamepad;
                    found_id = joysticks[i];
                    break;
                } else if (test_gamepad) {
                    SDL_CloseGamepad(test_gamepad);
                }
            }
        }
    }
    
    if (joysticks) {
        SDL_free(joysticks);
    }
    
    if (!gamepad) {
        printf("No generic gamepad found\n");
        return false;
    }
    
    // Allocate generic gamepad data
    GenericData* generic_data = malloc(sizeof(GenericData));
    if (!generic_data) {
        SDL_CloseGamepad(gamepad);
        return false;
    }
    
    memset(generic_data, 0, sizeof(GenericData));
    generic_data->gamepad = gamepad;
    generic_data->joystick_id = found_id;  // Store joystick ID instead of index
    generic_data->use_dpad = true;  // Default to D-pad
    generic_data->direction_start_time = 0;
    generic_data->last_direction = NEUTRAL;
    
    device->device_data = generic_data;
    device->info.is_connected = true;
    
    // Update device info with actual controller details
    const char* name = SDL_GetGamepadName(gamepad);
    if (name) {
        strncpy(device->info.device_name, name, sizeof(device->info.device_name) - 1);
    }
    
    // Get vendor/product ID
    SDL_Joystick* joystick = SDL_GetGamepadJoystick(gamepad);
    if (joystick) {
        device->info.vendor_id = SDL_GetJoystickVendor(joystick);
        device->info.product_id = SDL_GetJoystickProduct(joystick);
        
        // Try to detect some capabilities
        int num_buttons = SDL_GetNumJoystickButtons(joystick);
        device->info.capabilities.button_count = num_buttons;
        
        // Assume USB for most generic controllers
        device->info.connection_type = CONNECTION_USB;
    }
    
    printf("Generic Gamepad initialized: %s\n", device->info.device_name);
    printf("Vendor ID: 0x%04X, Product ID: 0x%04X\n", 
           device->info.vendor_id, device->info.product_id);
    printf("Using D-pad for movement (configurable)\n");
    
    return true;
}

static StandardInput* GenericPoll(InputDevice* device)
{
    if (!device || !device->device_data) return NULL;
    
    GenericData* generic_data = (GenericData*)device->device_data;
    
    if (!generic_data->gamepad) return NULL;
    
    uint64_t current_time = SDL_GetTicksNS();
    
    // Get current direction
    GameDirection current_direction = GetDirectionFromGeneric(generic_data);
    
    // Update direction timing
    if (current_direction != generic_data->last_direction) {
        generic_data->direction_start_time = current_time;
        generic_data->last_direction = current_direction;
    }
    
    // Calculate how long direction has been held
    uint64_t direction_duration = current_time - generic_data->direction_start_time;
    
    // Fill in standard input
    generic_data->current_input.direction = current_direction;
    generic_data->current_input.timestamp_ns = current_time;
    generic_data->current_input.direction_duration_ns = direction_duration;
    
    // Map buttons using positional layout (safest for unknown controllers)
    // Face buttons are typically arranged: Y(top), A(bottom), X(left), B(right)
    // We map by position: Top-left, Top-right, Bottom-left, Bottom-right
    generic_data->current_input.buttons[L_PUNCH] = SDL_GetGamepadButton(generic_data->gamepad, SDL_GAMEPAD_BUTTON_WEST);    // Left
    generic_data->current_input.buttons[R_PUNCH] = SDL_GetGamepadButton(generic_data->gamepad, SDL_GAMEPAD_BUTTON_NORTH);   // Top
    generic_data->current_input.buttons[L_KICK] = SDL_GetGamepadButton(generic_data->gamepad, SDL_GAMEPAD_BUTTON_SOUTH);    // Bottom
    generic_data->current_input.buttons[R_KICK] = SDL_GetGamepadButton(generic_data->gamepad, SDL_GAMEPAD_BUTTON_EAST);     // Right
    generic_data->current_input.buttons[MENU_SELECT] = SDL_GetGamepadButton(generic_data->gamepad, SDL_GAMEPAD_BUTTON_START);
    generic_data->current_input.buttons[MENU_BACK] = SDL_GetGamepadButton(generic_data->gamepad, SDL_GAMEPAD_BUTTON_BACK);
    
    return &generic_data->current_input;
}

static void GenericCleanup(InputDevice* device)
{
    if (!device) return;
    
    GenericData* generic_data = (GenericData*)device->device_data;
    if (generic_data) {
        if (generic_data->gamepad) {
            SDL_CloseGamepad(generic_data->gamepad);
        }
        free(generic_data);
        device->device_data = NULL;
    }
    
    printf("Generic Gamepad cleaned up\n");
}

static bool GenericConfigure(InputDevice* device)
{
    if (!device || !device->device_data) return false;
    
    GenericData* generic_data = (GenericData*)device->device_data;
    
    printf("\nGeneric Gamepad Configuration\n");
    printf("============================\n");
    printf("Current input method: %s\n", generic_data->use_dpad ? "D-Pad" : "Left Stick");
    printf("Button mapping (positional):\n");
    printf("- Left face button = L_PUNCH\n");
    printf("- Top face button = R_PUNCH\n");
    printf("- Bottom face button = L_KICK\n");
    printf("- Right face button = R_KICK\n");
    printf("- Start/Menu = SELECT\n");
    printf("- Back/View = BACK\n");
    printf("\nNote: Generic controllers use positional button mapping\n");
    printf("Toggle input method? (y/N): ");
    
    char choice;
    if (scanf(" %c", &choice) == 1 && (choice == 'y' || choice == 'Y')) {
        generic_data->use_dpad = !generic_data->use_dpad;
        printf("Switched to %s\n", generic_data->use_dpad ? "D-Pad" : "Left Stick");
    }
    
    return true;
}

static void GenericUpdateInfo(InputDevice* device)
{
    if (!device || !device->device_data) return;
    
    GenericData* generic_data = (GenericData*)device->device_data;
    
    if (generic_data->gamepad) {
        // Check if controller is still connected
        device->info.is_connected = SDL_GamepadConnected(generic_data->gamepad);
    } else {
        device->info.is_connected = false;
    }
    
    // Generic controllers typically don't provide battery info
    device->info.battery_level = -1;
}

static GameDirection GetDirectionFromGeneric(GenericData* generic_data)
{
    if (!generic_data || !generic_data->gamepad) return NEUTRAL;
    
    bool up, down, left, right;
    
    if (generic_data->use_dpad) {
        // Use D-pad
        up = SDL_GetGamepadButton(generic_data->gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP);
        down = SDL_GetGamepadButton(generic_data->gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
        left = SDL_GetGamepadButton(generic_data->gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
        right = SDL_GetGamepadButton(generic_data->gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
    } else {
        // Use left stick with deadzone
        float stick_x = SDL_GetGamepadAxis(generic_data->gamepad, SDL_GAMEPAD_AXIS_LEFTX) / 32768.0f;
        float stick_y = SDL_GetGamepadAxis(generic_data->gamepad, SDL_GAMEPAD_AXIS_LEFTY) / 32768.0f;
        
        const float deadzone = 0.3f;
        
        up = stick_y < -deadzone;
        down = stick_y > deadzone;
        left = stick_x < -deadzone;
        right = stick_x > deadzone;
    }
    
    // Convert to direction
    if (up && left) return UP_BACK;
    if (up && right) return UP_FORWARD;
    if (down && left) return DOWN_BACK;
    if (down && right) return DOWN_FORWARD;
    if (up) return UP;
    if (down) return DOWN;
    if (left) return BACK;
    if (right) return FORWARD;
    
    return NEUTRAL;
}

static bool IsGenericGamepad(SDL_Gamepad* gamepad)
{
    if (!gamepad) return false;
    
    // Get vendor ID to exclude known controllers handled by specific drivers
    SDL_Joystick* joystick = SDL_GetGamepadJoystick(gamepad);
    if (!joystick) return false;
    
    Uint16 vendor = SDL_GetJoystickVendor(joystick);
    
    // Exclude PS5 controllers (Sony)
    if (vendor == 0x054C) return false;
    
    // Exclude Xbox controllers (Microsoft, Logitech, Razer)
    if (vendor == 0x045E || vendor == 0x046D || vendor == 0x1532) return false;
    
    // Accept other gamepads as generic
    return true;
}
