#include "../include/input/input_mapping.h"
#include <stdio.h>
#include <string.h>
#include <SDL3/SDL.h>

bool SaveInputMapping(InputMapping* mapping, const char* filename)
{
    if (!mapping || !filename) return false;
    
    FILE* file = fopen(filename, "w");
    if (!file) return false;
    
    fprintf(file, "# KBD Trainer Input Mapping\n");
    fprintf(file, "mapping_name=%s\n", mapping->mapping_name);
    fprintf(file, "use_dpad=%d\n\n", mapping->use_dpad ? 1 : 0);
    
    fprintf(file, "[movement]\n");
    for (int i = 0; i < 4; i++) {
        fprintf(file, "%s=%d\n", mapping->movement_keys[i].name, mapping->movement_keys[i].physical_key);
    }
    
    fprintf(file, "\n[buttons]\n");
    for (int i = 0; i < MAX_GAME_BUTTONS; i++) {
        if (mapping->button_mappings[i].mapped_function != 0) {
            fprintf(file, "%s=%d\n", mapping->button_mappings[i].name, mapping->button_mappings[i].physical_key);
        }
    }
    
    fclose(file);
    return true;
}

bool LoadInputMapping(InputMapping* mapping, const char* filename)
{
    if (!mapping || !filename) return false;
    
    FILE* file = fopen(filename, "r");
    if (!file) return false;
    
    char line[256];
    bool in_movement = false;
    bool in_buttons = false;
    
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "[movement]", 10) == 0) {
            in_movement = true;
            in_buttons = false;
        } else if (strncmp(line, "[buttons]", 9) == 0) {
            in_movement = false;
            in_buttons = true;
        } else if (strncmp(line, "mapping_name=", 13) == 0) {
            sscanf(line, "mapping_name=%63[^\n]", mapping->mapping_name);
        } else if (strncmp(line, "use_dpad=", 9) == 0) {
            int use_dpad;
            sscanf(line, "use_dpad=%d", &use_dpad);
            mapping->use_dpad = (use_dpad != 0);
        }
        // Add more parsing as needed
    }
    
    fclose(file);
    return true;
}

void CreateDefaultKeyboardMapping(InputMapping* mapping)
{
    if (!mapping) return;
    
    strcpy(mapping->mapping_name, "Default Keyboard");
    mapping->use_dpad = false;  // N/A for keyboard
    
    // Movement keys (WASD)
    strcpy(mapping->movement_keys[0].name, "up");
    mapping->movement_keys[0].physical_key = SDL_SCANCODE_W;
    
    strcpy(mapping->movement_keys[1].name, "down");
    mapping->movement_keys[1].physical_key = SDL_SCANCODE_S;
    
    strcpy(mapping->movement_keys[2].name, "left");
    mapping->movement_keys[2].physical_key = SDL_SCANCODE_A;
    
    strcpy(mapping->movement_keys[3].name, "right");
    mapping->movement_keys[3].physical_key = SDL_SCANCODE_D;
    
    // Button mappings (UIJK)
    strcpy(mapping->button_mappings[L_PUNCH].name, "l_punch");
    mapping->button_mappings[L_PUNCH].physical_key = SDL_SCANCODE_U;
    mapping->button_mappings[L_PUNCH].mapped_function = L_PUNCH;
    
    strcpy(mapping->button_mappings[R_PUNCH].name, "r_punch");
    mapping->button_mappings[R_PUNCH].physical_key = SDL_SCANCODE_I;
    mapping->button_mappings[R_PUNCH].mapped_function = R_PUNCH;
    
    strcpy(mapping->button_mappings[L_KICK].name, "l_kick");
    mapping->button_mappings[L_KICK].physical_key = SDL_SCANCODE_J;
    mapping->button_mappings[L_KICK].mapped_function = L_KICK;
    
    strcpy(mapping->button_mappings[R_KICK].name, "r_kick");
    mapping->button_mappings[R_KICK].physical_key = SDL_SCANCODE_K;
    mapping->button_mappings[R_KICK].mapped_function = R_KICK;
    
    strcpy(mapping->button_mappings[MENU_SELECT].name, "select");
    mapping->button_mappings[MENU_SELECT].physical_key = SDL_SCANCODE_RETURN;
    mapping->button_mappings[MENU_SELECT].mapped_function = MENU_SELECT;
    
    strcpy(mapping->button_mappings[MENU_BACK].name, "back");
    mapping->button_mappings[MENU_BACK].physical_key = SDL_SCANCODE_ESCAPE;
    mapping->button_mappings[MENU_BACK].mapped_function = MENU_BACK;
}

void CreateDefaultGamepadMapping(InputMapping* mapping)
{
    if (!mapping) return;
    
    strcpy(mapping->mapping_name, "Default Gamepad");
    mapping->use_dpad = true;
    
    // Movement is handled differently for gamepads (D-pad/stick)
    // Button mappings use SDL gamepad buttons
    strcpy(mapping->button_mappings[L_PUNCH].name, "l_punch");
    mapping->button_mappings[L_PUNCH].physical_key = SDL_GAMEPAD_BUTTON_WEST;  // Left button
    mapping->button_mappings[L_PUNCH].mapped_function = L_PUNCH;
    
    strcpy(mapping->button_mappings[R_PUNCH].name, "r_punch");
    mapping->button_mappings[R_PUNCH].physical_key = SDL_GAMEPAD_BUTTON_NORTH;  // Top button
    mapping->button_mappings[R_PUNCH].mapped_function = R_PUNCH;
    
    strcpy(mapping->button_mappings[L_KICK].name, "l_kick");
    mapping->button_mappings[L_KICK].physical_key = SDL_GAMEPAD_BUTTON_SOUTH;  // Bottom button
    mapping->button_mappings[L_KICK].mapped_function = L_KICK;
    
    strcpy(mapping->button_mappings[R_KICK].name, "r_kick");
    mapping->button_mappings[R_KICK].physical_key = SDL_GAMEPAD_BUTTON_EAST;  // Right button
    mapping->button_mappings[R_KICK].mapped_function = R_KICK;
    
    strcpy(mapping->button_mappings[MENU_SELECT].name, "select");
    mapping->button_mappings[MENU_SELECT].physical_key = SDL_GAMEPAD_BUTTON_START;
    mapping->button_mappings[MENU_SELECT].mapped_function = MENU_SELECT;
    
    strcpy(mapping->button_mappings[MENU_BACK].name, "back");
    mapping->button_mappings[MENU_BACK].physical_key = SDL_GAMEPAD_BUTTON_BACK;
    mapping->button_mappings[MENU_BACK].mapped_function = MENU_BACK;
}

bool ApplyMappingToDevice(struct InputDevice* device, InputMapping* mapping)
{
    // This would be implemented by individual device drivers
    // For now, just return success
    return (device && mapping);
}

void PrintMapping(InputMapping* mapping)
{
    if (!mapping) return;
    
    printf("\nInput Mapping: %s\n", mapping->mapping_name);
    printf("================\n");
    
    printf("Movement:\n");
    for (int i = 0; i < 4; i++) {
        printf("  %s: %d\n", mapping->movement_keys[i].name, mapping->movement_keys[i].physical_key);
    }
    
    printf("Buttons:\n");
    for (int i = 0; i < MAX_GAME_BUTTONS; i++) {
        if (mapping->button_mappings[i].mapped_function != 0) {
            printf("  %s: %d\n", mapping->button_mappings[i].name, mapping->button_mappings[i].physical_key);
        }
    }
    
    if (mapping->use_dpad) {
        printf("Gamepad input: D-Pad\n");
    } else {
        printf("Gamepad input: Left Stick\n");
    }
    
    printf("\n");
}
