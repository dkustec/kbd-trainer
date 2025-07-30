#include "../include/input/standard_input.h"
#include "../include/core/timing.h"

bool IsInputValid(StandardInput* input)
{
    if (!input) return false;
    
    // Check if direction is valid
    if (input->direction < NEUTRAL || input->direction >= UNKNOWN) {
        return false;
    }
    
    return true;
}

bool IsDirectionHeldLongEnough(StandardInput* input)
{
    if (!input) return false;
    
    return IsInputFramePerfect(input->direction_duration_ns);
}

const char* GetButtonName(GameButton button)
{
    switch (button) {
        case L_PUNCH:    return "L_PUNCH";
        case R_PUNCH:    return "R_PUNCH";
        case L_KICK:     return "L_KICK";
        case R_KICK:     return "R_KICK";
        case MENU_SELECT: return "SELECT";
        case MENU_BACK:   return "BACK";
        default:         return "UNKNOWN";
    }
}
