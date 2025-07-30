#include "../include/input/input_device.h"
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <string.h>

// PS5 Controller vendor/product IDs
#define PS5_VENDOR_ID 0x054C
#define PS5_PRODUCT_ID_DUALSENSE 0x0CE6
#define PS5_PRODUCT_ID_DUALSENSE_EDGE 0x0DF2

// PS5-specific data
typedef struct {
    SDL_Gamepad* gamepad;
    int joystick_index;
    bool use_dpad;  // User preference: D-pad or left stick
    
    // Current input state
    StandardInput current_input;
    uint64_t direction_start_time;
    GameDirection last_direction;
} PS5Data;

// Forward declarations
static bool PS5Init(InputDevice* device);
static StandardInput* PS5Poll(InputDevice* device);
static void PS5Cleanup(InputDevice* device);
static bool PS5Configure(InputDevice* device);
static void PS5UpdateInfo(InputDevice* device);

// Helper functions
static GameDirection GetDirectionFromPS5(PS5Data* ps5_data);
static bool IsPS5Controller(SDL_Gamepad* gamepad);

InputDevice* CreatePS5Device(void)
{
    InputDevice* device = malloc(sizeof(InputDevice));
    if (!device) return NULL;
    
    // Initialize device info
    memset(&device->info, 0, sizeof(DeviceInfo));
    strcpy(device->info.device_name, "DualSense Controller");
    strcpy(device->info.model, "PlayStation 5 Controller");
    strcpy(device->info.driver_name, "SDL3 Gamepad");
    device->info.connection_type = CONNECTION_UNKNOWN;  // Will be detected
    device->info.is_connected = false;
    device->info.battery_level = -1;
    device->info.input_latency_ms = 3.0f;  // Typical for PS5 controllers
    device->info.is_calibrated = true;
    device->info.vendor_id = PS5_VENDOR_ID;
    device->info.product_id = PS5_PRODUCT_ID_DUALSENSE;
    
    // Set capabilities
    device->info.capabilities.has_dpad = true;
    device->info.capabilities.has_left_stick = true;
    device->info.capabilities.has_right_stick = true;
    device->info.capabilities.button_count = 14;  // PS5 has many buttons
    device->info.capabilities.has_rumble = true;
    device->info.capabilities.has_gyro = true;
    device->info.capabilities.has_touchpad = true;
    
    // Set interface functions
    device->interface.init = PS5Init;
    device->interface.poll = PS5Poll;
    device->interface.cleanup = PS5Cleanup;
    device->interface.configure = PS5Configure;
    device->interface.update_info = PS5UpdateInfo;
    
    device->device_data = NULL;
    device->is_initialized = false;
    
    return device;
}

static bool PS5Init(InputDevice* device)
{
    if (!device) return false;
    
    // Look for PS5 controllers
    int count = 0;
    SDL_JoystickID* joysticks = SDL_GetJoysticks(&count);
    SDL_Gamepad* gamepad = NULL;
    SDL_JoystickID found_id = 0;
    
    if (joysticks && count > 0) {
        for (int i = 0; i < count; i++) {
            if (SDL_IsGamepad(joysticks[i])) {
                SDL_Gamepad* test_gamepad = SDL_OpenGamepad(joysticks[i]);
                if (test_gamepad && IsPS5Controller(test_gamepad)) {
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
        printf("No PS5 controller found\n");
        return false;
    }
    
    // Allocate PS5-specific data
    PS5Data* ps5_data = malloc(sizeof(PS5Data));
    if (!ps5_data) {
        SDL_CloseGamepad(gamepad);
        return false;
    }
    
    memset(ps5_data, 0, sizeof(PS5Data));
    ps5_data->gamepad = gamepad;
    ps5_data->joystick_index = found_id;
    ps5_data->use_dpad = true;  // Default to D-pad
    ps5_data->direction_start_time = 0;
    ps5_data->last_direction = NEUTRAL;
    
    device->device_data = ps5_data;
    device->info.is_connected = true;
    
    // Update device info with actual controller details
    const char* name = SDL_GetGamepadName(gamepad);
    if (name) {
        strncpy(device->info.device_name, name, sizeof(device->info.device_name) - 1);
    }
    
    // Detect connection type (simplified)
    device->info.connection_type = CONNECTION_USB;  // Would need more sophisticated detection
    
    printf("PS5 Controller initialized: %s\n", device->info.device_name);
    printf("Using D-pad for movement (configurable)\n");
    
    return true;
}

static StandardInput* PS5Poll(InputDevice* device)
{
    if (!device || !device->device_data) return NULL;
    
    PS5Data* ps5_data = (PS5Data*)device->device_data;
    
    if (!ps5_data->gamepad) return NULL;
    
    uint64_t current_time = SDL_GetTicksNS();
    
    // Get current direction
    GameDirection current_direction = GetDirectionFromPS5(ps5_data);
    
    // Update direction timing
    if (current_direction != ps5_data->last_direction) {
        ps5_data->direction_start_time = current_time;
        ps5_data->last_direction = current_direction;
    }
    
    // Calculate how long direction has been held
    uint64_t direction_duration = current_time - ps5_data->direction_start_time;
    
    // Fill in standard input
    ps5_data->current_input.direction = current_direction;
    ps5_data->current_input.timestamp_ns = current_time;
    ps5_data->current_input.direction_duration_ns = direction_duration;
    
    // Map PS5 buttons to our standard layout
    // PS5: Square=L_PUNCH, Triangle=R_PUNCH, Cross=L_KICK, Circle=R_KICK
    ps5_data->current_input.buttons[L_PUNCH] = SDL_GetGamepadButton(ps5_data->gamepad, SDL_GAMEPAD_BUTTON_WEST);    // Square
    ps5_data->current_input.buttons[R_PUNCH] = SDL_GetGamepadButton(ps5_data->gamepad, SDL_GAMEPAD_BUTTON_NORTH);   // Triangle
    ps5_data->current_input.buttons[L_KICK] = SDL_GetGamepadButton(ps5_data->gamepad, SDL_GAMEPAD_BUTTON_SOUTH);    // Cross
    ps5_data->current_input.buttons[R_KICK] = SDL_GetGamepadButton(ps5_data->gamepad, SDL_GAMEPAD_BUTTON_EAST);     // Circle
    ps5_data->current_input.buttons[MENU_SELECT] = SDL_GetGamepadButton(ps5_data->gamepad, SDL_GAMEPAD_BUTTON_START);
    ps5_data->current_input.buttons[MENU_BACK] = SDL_GetGamepadButton(ps5_data->gamepad, SDL_GAMEPAD_BUTTON_BACK);
    
    return &ps5_data->current_input;
}

static void PS5Cleanup(InputDevice* device)
{
    if (!device) return;
    
    PS5Data* ps5_data = (PS5Data*)device->device_data;
    if (ps5_data) {
        if (ps5_data->gamepad) {
            SDL_CloseGamepad(ps5_data->gamepad);
        }
        free(ps5_data);
        device->device_data = NULL;
    }
    
    printf("PS5 Controller cleaned up\n");
}

static bool PS5Configure(InputDevice* device)
{
    if (!device || !device->device_data) return false;
    
    PS5Data* ps5_data = (PS5Data*)device->device_data;
    
    printf("\nPS5 Controller Configuration\n");
    printf("===========================\n");
    printf("Current input method: %s\n", ps5_data->use_dpad ? "D-Pad" : "Left Stick");
    printf("Button mapping (PlayStation convention):\n");
    printf("- Square (□) = L_PUNCH\n");
    printf("- Triangle (△) = R_PUNCH\n");
    printf("- Cross (✕) = L_KICK\n");
    printf("- Circle (○) = R_KICK\n");
    printf("- Options = SELECT\n");
    printf("- Share = BACK\n");
    printf("\nToggle input method? (y/N): ");
    
    char choice;
    if (scanf(" %c", &choice) == 1 && (choice == 'y' || choice == 'Y')) {
        ps5_data->use_dpad = !ps5_data->use_dpad;
        printf("Switched to %s\n", ps5_data->use_dpad ? "D-Pad" : "Left Stick");
    }
    
    return true;
}

static void PS5UpdateInfo(InputDevice* device)
{
    if (!device || !device->device_data) return;
    
    PS5Data* ps5_data = (PS5Data*)device->device_data;
    
    if (ps5_data->gamepad) {
        // Check if controller is still connected
        device->info.is_connected = SDL_GamepadConnected(ps5_data->gamepad);
        
        // Try to get battery level (not always available)
        // This would require platform-specific code for accurate battery reading
        device->info.battery_level = 75;  // Placeholder
    } else {
        device->info.is_connected = false;
        device->info.battery_level = -1;
    }
}

static GameDirection GetDirectionFromPS5(PS5Data* ps5_data)
{
    if (!ps5_data || !ps5_data->gamepad) return NEUTRAL;
    
    bool up, down, left, right;
    
    if (ps5_data->use_dpad) {
        // Use D-pad
        up = SDL_GetGamepadButton(ps5_data->gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP);
        down = SDL_GetGamepadButton(ps5_data->gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
        left = SDL_GetGamepadButton(ps5_data->gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
        right = SDL_GetGamepadButton(ps5_data->gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
    } else {
        // Use left stick with deadzone
        float stick_x = SDL_GetGamepadAxis(ps5_data->gamepad, SDL_GAMEPAD_AXIS_LEFTX) / 32768.0f;
        float stick_y = SDL_GetGamepadAxis(ps5_data->gamepad, SDL_GAMEPAD_AXIS_LEFTY) / 32768.0f;
        
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

static bool IsPS5Controller(SDL_Gamepad* gamepad)
{
    if (!gamepad) return false;
    
    // Check vendor/product ID
    SDL_Joystick* joystick = SDL_GetGamepadJoystick(gamepad);
    if (!joystick) return false;
    
    Uint16 vendor = SDL_GetJoystickVendor(joystick);
    Uint16 product = SDL_GetJoystickProduct(joystick);
    
    return (vendor == PS5_VENDOR_ID && 
            (product == PS5_PRODUCT_ID_DUALSENSE || product == PS5_PRODUCT_ID_DUALSENSE_EDGE));
}
