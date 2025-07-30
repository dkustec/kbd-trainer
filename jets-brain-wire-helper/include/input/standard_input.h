#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "../core/player.h"

// Standardized game buttons (fighting game convention)
typedef enum {
    L_PUNCH = 1,  // Top-Left face button (Square/X/etc.)
    R_PUNCH = 2,  // Top-Right face button (Triangle/Y/etc.)
    L_KICK = 3,   // Bottom-Left face button (Cross/A/etc.)
    R_KICK = 4,   // Bottom-Right face button (Circle/B/etc.)
    MENU_SELECT = 5,
    MENU_BACK = 6,
    MAX_GAME_BUTTONS = 7
} GameButton;

// Device-agnostic input state
typedef struct {
    GameDirection direction;
    bool buttons[MAX_GAME_BUTTONS];
    uint64_t timestamp_ns;
    uint64_t direction_duration_ns;  // How long current direction has been held
} StandardInput;

// Input validation
bool IsInputValid(StandardInput* input);
bool IsDirectionHeldLongEnough(StandardInput* input);
const char* GetButtonName(GameButton button);
