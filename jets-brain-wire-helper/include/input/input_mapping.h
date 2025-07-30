#pragma once

#include <stdbool.h>
#include "standard_input.h"

#define MAX_KEY_NAME_LENGTH 32
#define MAX_MAPPING_NAME_LENGTH 64

// Key/button mapping structure
typedef struct {
    char name[MAX_KEY_NAME_LENGTH];
    int physical_key;  // SDL scancode or gamepad button
    GameButton mapped_function;
} ButtonMapping;

// Complete input mapping configuration
typedef struct {
    char mapping_name[MAX_MAPPING_NAME_LENGTH];
    ButtonMapping movement_keys[4];  // Up, Down, Left, Right
    ButtonMapping button_mappings[MAX_GAME_BUTTONS];
    bool use_dpad;  // For gamepads: true = D-pad, false = stick
} InputMapping;

// Mapping management
bool SaveInputMapping(InputMapping* mapping, const char* filename);
bool LoadInputMapping(InputMapping* mapping, const char* filename);
void CreateDefaultKeyboardMapping(InputMapping* mapping);
void CreateDefaultGamepadMapping(InputMapping* mapping);

// Mapping application
bool ApplyMappingToDevice(struct InputDevice* device, InputMapping* mapping);
void PrintMapping(InputMapping* mapping);
