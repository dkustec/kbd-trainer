#include "../include/input/input_device.h"
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <string.h>

// Keyboard-specific data
typedef struct {
    // Key mappings (configurable)
    SDL_Scancode move_up;
    SDL_Scancode move_down;
    SDL_Scancode move_left;
    SDL_Scancode move_right;
    
    SDL_Scancode btn_l_punch;
    SDL_Scancode btn_r_punch;
    SDL_Scancode btn_l_kick;
    SDL_Scancode btn_r_kick;
    SDL_Scancode btn_select;
    SDL_Scancode btn_back;
    
    // Current input state
    StandardInput current_input;
    uint64_t direction_start_time;
    GameDirection last_direction;
} KeyboardData;

// Forward declarations
static bool KeyboardInit(InputDevice* device);
static StandardInput* KeyboardPoll(InputDevice* device);
static void KeyboardCleanup(InputDevice* device);
static bool KeyboardConfigure(InputDevice* device);
static void KeyboardUpdateInfo(InputDevice* device);

// Helper functions
static GameDirection GetDirectionFromKeys(const Uint8* keyboard_state, KeyboardData* kb_data);
static void SetDefaultKeyMapping(KeyboardData* kb_data);

InputDevice* CreateKeyboardDevice(void)
{
    InputDevice* device = malloc(sizeof(InputDevice));
    if (!device) return NULL;
    
    // Initialize device info
    memset(&device->info, 0, sizeof(DeviceInfo));
    strcpy(device->info.device_name, "Keyboard");
    strcpy(device->info.model, "Standard Keyboard");
    strcpy(device->info.driver_name, "SDL3 Keyboard");
    device->info.connection_type = CONNECTION_USB;  // Assume USB for most keyboards
    device->info.is_connected = true;
    device->info.battery_level = -1;  // N/A for keyboards
    device->info.input_latency_ms = 1.0f;  // Very low latency
    device->info.is_calibrated = true;
    
    // Set capabilities
    device->info.capabilities.has_dpad = true;  // Emulated via keys
    device->info.capabilities.has_left_stick = false;
    device->info.capabilities.has_right_stick = false;
    device->info.capabilities.button_count = 6;  // Our 6 game buttons
    device->info.capabilities.has_rumble = false;
    device->info.capabilities.has_gyro = false;
    device->info.capabilities.has_touchpad = false;
    
    // Set interface functions
    device->interface.init = KeyboardInit;
    device->interface.poll = KeyboardPoll;
    device->interface.cleanup = KeyboardCleanup;
    device->interface.configure = KeyboardConfigure;
    device->interface.update_info = KeyboardUpdateInfo;
    
    device->device_data = NULL;
    device->is_initialized = false;
    
    return device;
}

static bool KeyboardInit(InputDevice* device)
{
    if (!device) return false;
    
    // Allocate keyboard-specific data
    KeyboardData* kb_data = malloc(sizeof(KeyboardData));
    if (!kb_data) return false;
    
    memset(kb_data, 0, sizeof(KeyboardData));
    
    // Set default key mappings
    SetDefaultKeyMapping(kb_data);
    
    // Initialize timing
    kb_data->direction_start_time = 0;
    kb_data->last_direction = NEUTRAL;
    
    device->device_data = kb_data;
    
    printf("Keyboard device initialized\n");
    printf("Default controls: WASD for movement, UIJK for buttons\n");
    printf("U=L_PUNCH, I=R_PUNCH, J=L_KICK, K=R_KICK, Enter=SELECT, Esc=BACK\n");
    
    return true;
}

static StandardInput* KeyboardPoll(InputDevice* device)
{
    if (!device || !device->device_data) return NULL;
    
    KeyboardData* kb_data = (KeyboardData*)device->device_data;
    const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
    
    uint64_t current_time = SDL_GetTicksNS();
    
    // Get current direction
    GameDirection current_direction = GetDirectionFromKeys(keyboard_state, kb_data);
    
    // Update direction timing
    if (current_direction != kb_data->last_direction) {
        kb_data->direction_start_time = current_time;
        kb_data->last_direction = current_direction;
    }
    
    // Calculate how long direction has been held
    uint64_t direction_duration = current_time - kb_data->direction_start_time;
    
    // Fill in standard input
    kb_data->current_input.direction = current_direction;
    kb_data->current_input.timestamp_ns = current_time;
    kb_data->current_input.direction_duration_ns = direction_duration;
    
    // Check buttons
    kb_data->current_input.buttons[L_PUNCH] = keyboard_state[kb_data->btn_l_punch];
    kb_data->current_input.buttons[R_PUNCH] = keyboard_state[kb_data->btn_r_punch];
    kb_data->current_input.buttons[L_KICK] = keyboard_state[kb_data->btn_l_kick];
    kb_data->current_input.buttons[R_KICK] = keyboard_state[kb_data->btn_r_kick];
    kb_data->current_input.buttons[MENU_SELECT] = keyboard_state[kb_data->btn_select];
    kb_data->current_input.buttons[MENU_BACK] = keyboard_state[kb_data->btn_back];
    
    return &kb_data->current_input;
}

static void KeyboardCleanup(InputDevice* device)
{
    if (!device) return;
    
    if (device->device_data) {
        free(device->device_data);
        device->device_data = NULL;
    }
    
    printf("Keyboard device cleaned up\n");
}

static bool KeyboardConfigure(InputDevice* device)
{
    if (!device || !device->device_data) return false;
    
    KeyboardData* kb_data = (KeyboardData*)device->device_data;
    
    printf("\nKeyboard Configuration\n");
    printf("=====================\n");
    printf("Current mappings:\n");
    printf("Movement: W(up) A(left) S(down) D(right)\n");
    printf("L_PUNCH: U, R_PUNCH: I, L_KICK: J, R_KICK: K\n");
    printf("SELECT: Enter, BACK: Escape\n");
    printf("\nConfiguration options not yet implemented.\n");
    printf("Using default WASD + UIJK layout.\n");
    
    return true;
}

static void KeyboardUpdateInfo(InputDevice* device)
{
    if (!device) return;
    
    // Keyboard info is mostly static, but we can check connection
    device->info.is_connected = true;  // Keyboards are always "connected"
}

static GameDirection GetDirectionFromKeys(const Uint8* keyboard_state, KeyboardData* kb_data)
{
    bool up = keyboard_state[kb_data->move_up];
    bool down = keyboard_state[kb_data->move_down];
    bool left = keyboard_state[kb_data->move_left];
    bool right = keyboard_state[kb_data->move_right];
    
    // Convert key combinations to directions
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

static void SetDefaultKeyMapping(KeyboardData* kb_data)
{
    // WASD for movement
    kb_data->move_up = SDL_SCANCODE_W;
    kb_data->move_down = SDL_SCANCODE_S;
    kb_data->move_left = SDL_SCANCODE_A;
    kb_data->move_right = SDL_SCANCODE_D;
    
    // UIJK for face buttons
    kb_data->btn_l_punch = SDL_SCANCODE_U;  // Top-left
    kb_data->btn_r_punch = SDL_SCANCODE_I;  // Top-right
    kb_data->btn_l_kick = SDL_SCANCODE_J;   // Bottom-left
    kb_data->btn_r_kick = SDL_SCANCODE_K;   // Bottom-right
    
    // Menu buttons
    kb_data->btn_select = SDL_SCANCODE_RETURN;
    kb_data->btn_back = SDL_SCANCODE_ESCAPE;
}
