#include "../include/input/input_device.h"
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <string.h>

// Xbox Controller vendor IDs (Microsoft and compatible)
#define XBOX_VENDOR_ID_MICROSOFT 0x045E
#define XBOX_VENDOR_ID_LOGITECH 0x046D
#define XBOX_VENDOR_ID_RAZER 0x1532

// Xbox-specific data
typedef struct {
    SDL_Gamepad* gamepad;
    int joystick_index;
    bool use_dpad;  // User preference: D-pad or left stick
    bool dpad_is_same_axis;  // Whether D-pad uses same axis as stick
    
    // Current input state
    StandardInput current_input;
    uint64_t direction_start_time;
    GameDirection last_direction;
} XboxData;

// Forward declarations
static bool XboxInit(InputDevice* device);
static StandardInput* XboxPoll(InputDevice* device);
static void XboxCleanup(InputDevice* device);
static bool XboxConfigure(InputDevice* device);
static void XboxUpdateInfo(InputDevice* device);

// Helper functions
static GameDirection GetDirectionFromXbox(XboxData* xbox_data);
static bool IsXboxController(SDL_Gamepad* gamepad);
static void DetectDpadAxisConfiguration(XboxData* xbox_data);

InputDevice* CreateXboxDevice(void)
{
    InputDevice* device = malloc(sizeof(InputDevice));
    if (!device) return NULL;
    
    // Initialize device info
    memset(&device->info, 0, sizeof(DeviceInfo));
    strcpy(device->info.device_name, "Xbox Controller");
    strcpy(device->info.model, "Xbox Series X|S Controller");
    strcpy(device->info.driver_name, "SDL3 Gamepad");
    device->info.connection_type = CONNECTION_UNKNOWN;  // Will be detected
    device->info.is_connected = false;
    device->info.battery_level = -1;
    device->info.input_latency_ms = 2.5f;  // Typical for Xbox controllers
    device->info.is_calibrated = true;
    device->info.vendor_id = XBOX_VENDOR_ID_MICROSOFT;
    device->info.product_id = 0x0000;  // Will be set when detected
    
    // Set capabilities
    device->info.capabilities.has_dpad = true;
    device->info.capabilities.has_left_stick = true;
    device->info.capabilities.has_right_stick = true;
    device->info.capabilities.button_count = 11;  // Xbox standard buttons
    device->info.capabilities.has_rumble = true;
    device->info.capabilities.has_gyro = false;  // Most Xbox controllers don't have gyro
    device->info.capabilities.has_touchpad = false;
    
    // Set interface functions
    device->interface.init = XboxInit;
    device->interface.poll = XboxPoll;
    device->interface.cleanup = XboxCleanup;
    device->interface.configure = XboxConfigure;
    device->interface.update_info = XboxUpdateInfo;
    
    device->device_data = NULL;
    device->is_initialized = false;
    
    return device;
}

static bool XboxInit(InputDevice* device)
{
    if (!device) return false;
    
    // Look for Xbox controllers
    int count = 0;
    SDL_JoystickID* joysticks = SDL_GetJoysticks(&count);
    SDL_Gamepad* gamepad = NULL;
    SDL_JoystickID found_id = 0;
    
    if (joysticks && count > 0) {
        for (int i = 0; i < count; i++) {
            if (SDL_IsGamepad(joysticks[i])) {
                SDL_Gamepad* test_gamepad = SDL_OpenGamepad(joysticks[i]);
                if (test_gamepad && IsXboxController(test_gamepad)) {
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
        printf("No Xbox controller found\n");
        return false;
    }
    
    // Allocate Xbox-specific data
    XboxData* xbox_data = malloc(sizeof(XboxData));
    if (!xbox_data) {
        SDL_CloseGamepad(gamepad);
        return false;
    }
    
    memset(xbox_data, 0, sizeof(XboxData));
    xbox_data->gamepad = gamepad;
    xbox_data->joystick_index = found_id;
    xbox_data->use_dpad = true;  // Default to D-pad
    xbox_data->direction_start_time = 0;
    xbox_data->last_direction = NEUTRAL;
    
    // Detect D-pad configuration
    DetectDpadAxisConfiguration(xbox_data);
    
    device->device_data = xbox_data;
    device->info.is_connected = true;
    
    // Update device info with actual controller details
    const char* name = SDL_GetGamepadName(gamepad);
    if (name) {
        strncpy(device->info.device_name, name, sizeof(device->info.device_name) - 1);
    }
    
    // Get actual vendor/product ID
    SDL_Joystick* joystick = SDL_GetGamepadJoystick(gamepad);
    if (joystick) {
        device->info.vendor_id = SDL_GetJoystickVendor(joystick);
        device->info.product_id = SDL_GetJoystickProduct(joystick);
    }
    
    // Detect connection type (simplified)
    device->info.connection_type = CONNECTION_USB;  // Would need more sophisticated detection
    
    printf("Xbox Controller initialized: %s\n", device->info.device_name);
    printf("D-pad configuration: %s\n", 
           xbox_data->dpad_is_same_axis ? "Same axis as stick" : "Separate from stick");
    printf("Using D-pad for movement (configurable)\n");
    
    return true;
}

static StandardInput* XboxPoll(InputDevice* device)
{
    if (!device || !device->device_data) return NULL;
    
    XboxData* xbox_data = (XboxData*)device->device_data;
    
    if (!xbox_data->gamepad) return NULL;
    
    uint64_t current_time = SDL_GetTicksNS();
    
    // Get current direction
    GameDirection current_direction = GetDirectionFromXbox(xbox_data);
    
    // Update direction timing
    if (current_direction != xbox_data->last_direction) {
        xbox_data->direction_start_time = current_time;
        xbox_data->last_direction = current_direction;
    }
    
    // Calculate how long direction has been held
    uint64_t direction_duration = current_time - xbox_data->direction_start_time;
    
    // Fill in standard input
    xbox_data->current_input.direction = current_direction;
    xbox_data->current_input.timestamp_ns = current_time;
    xbox_data->current_input.direction_duration_ns = direction_duration;
    
    // Map Xbox buttons to our standard layout
    // Xbox: X=L_PUNCH, Y=R_PUNCH, A=L_KICK, B=R_KICK
    xbox_data->current_input.buttons[L_PUNCH] = SDL_GetGamepadButton(xbox_data->gamepad, SDL_GAMEPAD_BUTTON_WEST);   // X
    xbox_data->current_input.buttons[R_PUNCH] = SDL_GetGamepadButton(xbox_data->gamepad, SDL_GAMEPAD_BUTTON_NORTH);  // Y
    xbox_data->current_input.buttons[L_KICK] = SDL_GetGamepadButton(xbox_data->gamepad, SDL_GAMEPAD_BUTTON_SOUTH);   // A
    xbox_data->current_input.buttons[R_KICK] = SDL_GetGamepadButton(xbox_data->gamepad, SDL_GAMEPAD_BUTTON_EAST);    // B
    xbox_data->current_input.buttons[MENU_SELECT] = SDL_GetGamepadButton(xbox_data->gamepad, SDL_GAMEPAD_BUTTON_START);
    xbox_data->current_input.buttons[MENU_BACK] = SDL_GetGamepadButton(xbox_data->gamepad, SDL_GAMEPAD_BUTTON_BACK);
    
    return &xbox_data->current_input;
}

static void XboxCleanup(InputDevice* device)
{
    if (!device) return;
    
    XboxData* xbox_data = (XboxData*)device->device_data;
    if (xbox_data) {
        if (xbox_data->gamepad) {
            SDL_CloseGamepad(xbox_data->gamepad);
        }
        free(xbox_data);
        device->device_data = NULL;
    }
    
    printf("Xbox Controller cleaned up\n");
}

static bool XboxConfigure(InputDevice* device)
{
    if (!device || !device->device_data) return false;
    
    XboxData* xbox_data = (XboxData*)device->device_data;
    
    printf("\nXbox Controller Configuration\n");
    printf("============================\n");
    printf("Current input method: %s\n", xbox_data->use_dpad ? "D-Pad" : "Left Stick");
    printf("D-pad configuration: %s\n", 
           xbox_data->dpad_is_same_axis ? "Same axis as stick" : "Separate from stick");
    printf("Button mapping (Xbox convention):\n");
    printf("- X = L_PUNCH\n");
    printf("- Y = R_PUNCH\n");
    printf("- A = L_KICK\n");
    printf("- B = R_KICK\n");
    printf("- Menu = SELECT\n");
    printf("- View = BACK\n");
    printf("\nToggle input method? (y/N): ");
    
    char choice;
    if (scanf(" %c", &choice) == 1 && (choice == 'y' || choice == 'Y')) {
        xbox_data->use_dpad = !xbox_data->use_dpad;
        printf("Switched to %s\n", xbox_data->use_dpad ? "D-Pad" : "Left Stick");
    }
    
    return true;
}

static void XboxUpdateInfo(InputDevice* device)
{
    if (!device || !device->device_data) return;
    
    XboxData* xbox_data = (XboxData*)device->device_data;
    
    if (xbox_data->gamepad) {
        // Check if controller is still connected
        device->info.is_connected = SDL_GamepadConnected(xbox_data->gamepad);
        
        // Xbox controllers typically don't provide battery info via SDL
        device->info.battery_level = -1;  // N/A
    } else {
        device->info.is_connected = false;
        device->info.battery_level = -1;
    }
}

static GameDirection GetDirectionFromXbox(XboxData* xbox_data)
{
    if (!xbox_data || !xbox_data->gamepad) return NEUTRAL;
    
    bool up, down, left, right;
    
    if (xbox_data->use_dpad) {
        // Use D-pad
        up = SDL_GetGamepadButton(xbox_data->gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP);
        down = SDL_GetGamepadButton(xbox_data->gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
        left = SDL_GetGamepadButton(xbox_data->gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
        right = SDL_GetGamepadButton(xbox_data->gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
    } else {
        // Use left stick with deadzone
        float stick_x = SDL_GetGamepadAxis(xbox_data->gamepad, SDL_GAMEPAD_AXIS_LEFTX) / 32768.0f;
        float stick_y = SDL_GetGamepadAxis(xbox_data->gamepad, SDL_GAMEPAD_AXIS_LEFTY) / 32768.0f;
        
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

static bool IsXboxController(SDL_Gamepad* gamepad)
{
    if (!gamepad) return false;
    
    // Check vendor ID for known Xbox-compatible manufacturers
    SDL_Joystick* joystick = SDL_GetGamepadJoystick(gamepad);
    if (!joystick) return false;
    
    Uint16 vendor = SDL_GetJoystickVendor(joystick);
    
    // Check for Microsoft, Logitech, Razer, and other Xbox-compatible vendors
    return (vendor == XBOX_VENDOR_ID_MICROSOFT || 
            vendor == XBOX_VENDOR_ID_LOGITECH ||
            vendor == XBOX_VENDOR_ID_RAZER);
}

static void DetectDpadAxisConfiguration(XboxData* xbox_data)
{
    if (!xbox_data || !xbox_data->gamepad) return;
    
    // This is a simplified detection - in reality you'd need to test
    // various controllers to determine their D-pad behavior
    // For now, assume most Xbox controllers have separate D-pad
    xbox_data->dpad_is_same_axis = false;
    
    // Some generic controllers that report as Xbox might use same axis
    // This would require more sophisticated detection based on specific models
}
