#pragma once

#include <stdint.h>

typedef enum {
    PLAYER_1 = 1,
    PLAYER_2 = 2
} PlayerSide;

typedef enum {
    NEUTRAL = 0,
    UP,
    UP_FORWARD,
    FORWARD,
    DOWN_FORWARD,
    DOWN,
    DOWN_BACK,
    BACK,
    UP_BACK,
    UNKNOWN,
    DISCONNECTED
} GameDirection;

// Convert direction for opposite player side
GameDirection ConvertDirectionForPlayer(GameDirection dir, PlayerSide player);

// Convert entire pattern for player side
GameDirection* ConvertPatternForPlayer(const GameDirection* pattern, int pattern_size, PlayerSide player);

// Helper functions
const char* GetPlayerSideName(PlayerSide player);
const char* GetDirectionName(GameDirection dir);
