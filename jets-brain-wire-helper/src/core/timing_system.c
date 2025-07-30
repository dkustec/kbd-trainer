#include "../include/core/timing.h"
#include <SDL3/SDL.h>

bool IsInputFramePerfect(uint64_t duration_ns)
{
    // Input must be held for at least 1 frame (16.67ms) to be valid
    return duration_ns >= FRAME_DURATION_NS;
}

uint64_t GetCurrentTimeNS(void)
{
    return SDL_GetTicksNS();
}

int GetFramesSince(uint64_t start_time_ns)
{
    uint64_t current_time = GetCurrentTimeNS();
    uint64_t elapsed_ns = current_time - start_time_ns;
    return (int)(elapsed_ns / FRAME_DURATION_NS);
}

bool HasFrameElapsed(uint64_t last_time_ns)
{
    uint64_t current_time = GetCurrentTimeNS();
    return (current_time - last_time_ns) >= FRAME_DURATION_NS;
}
