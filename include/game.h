#pragma once

#define GAME_MODE_COUNT 4

#include <stdint.h>
#include "input.h"

typedef struct
{
    const char *mode_name;

    GameDirection *pattern;
    int pattern_size;
} GameMode;

extern int selected_mode;
extern GameMode gamemodes[GAME_MODE_COUNT];

typedef enum
{
    NONE = 0,
    SUCCESS,
    FAIL
} InputAccuracy;

typedef struct
{
    // current position in the pattern
    int player_pos;

    uint64_t score;
    uint64_t highscore;

    GameDirection curr_input;

    InputAccuracy last_input_acc;
    GameDirection last_input;

    // Timeout after failure (in nanoseconds)
    uint64_t failure_timeout_start;
    bool in_failure_timeout;

    GameMode *current_mode;
    bool run_game;
} GameState;

extern GameState gamestate;

void InitGame();
void DestroyGame();
void _startGame();

void Update(ControllerState *);

void _updateMenu(ControllerState *);
void _updateGame(ControllerState *);

void _initGameModes();
