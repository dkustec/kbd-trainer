#pragma once

#include <stdint.h>
#include <stdbool.h>

// Tekken 8 runs at locked 60 FPS
#define TEKKEN_FPS 60
#define FRAME_DURATION_NS (1000000000ULL / TEKKEN_FPS)  // ~16.67ms in nanoseconds
#define FRAME_DURATION_MS (1000.0 / TEKKEN_FPS)         // 16.67ms

// Timing validation functions
bool IsInputFramePerfect(uint64_t duration_ns);
uint64_t GetCurrentTimeNS(void);
int GetFramesSince(uint64_t start_time_ns);
bool HasFrameElapsed(uint64_t last_time_ns);
