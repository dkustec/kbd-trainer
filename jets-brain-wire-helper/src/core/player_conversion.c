#include "../include/core/player.h"
#include <stdlib.h>
#include <string.h>

GameDirection ConvertDirectionForPlayer(GameDirection dir, PlayerSide player)
{
    if (player == PLAYER_1) {
        return dir;  // P1 uses directions as-is
    }
    
    // P2 mirrors left/right directions
    switch (dir) {
        case FORWARD:      return BACK;
        case BACK:         return FORWARD;
        case UP_FORWARD:   return UP_BACK;
        case UP_BACK:      return UP_FORWARD;
        case DOWN_FORWARD: return DOWN_BACK;
        case DOWN_BACK:    return DOWN_FORWARD;
        default:           return dir;  // UP, DOWN, NEUTRAL unchanged
    }
}

GameDirection* ConvertPatternForPlayer(const GameDirection* pattern, int pattern_size, PlayerSide player)
{
    if (player == PLAYER_1) {
        // P1 - return copy of original pattern
        GameDirection* result = malloc(pattern_size * sizeof(GameDirection));
        if (result) {
            memcpy(result, pattern, pattern_size * sizeof(GameDirection));
        }
        return result;
    }
    
    // P2 - convert each direction
    GameDirection* result = malloc(pattern_size * sizeof(GameDirection));
    if (!result) return NULL;
    
    for (int i = 0; i < pattern_size; i++) {
        result[i] = ConvertDirectionForPlayer(pattern[i], player);
    }
    
    return result;
}

const char* GetPlayerSideName(PlayerSide player)
{
    switch (player) {
        case PLAYER_1: return "P1";
        case PLAYER_2: return "P2";
        default:       return "Unknown";
    }
}

const char* GetDirectionName(GameDirection dir)
{
    switch (dir) {
        case NEUTRAL:      return "5";
        case UP:           return "8";
        case UP_FORWARD:   return "9";
        case FORWARD:      return "6";
        case DOWN_FORWARD: return "3";
        case DOWN:         return "2";
        case DOWN_BACK:    return "1";
        case BACK:         return "4";
        case UP_BACK:      return "7";
        case UNKNOWN:      return "?";
        case DISCONNECTED: return "X";
        default:           return "?";
    }
}
