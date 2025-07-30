#pragma once

#include "../core/slot.h"
#include "../core/plug.h" 
#include "../core/player.h"
#include "../core/timing.h"
#include "../input/input_device.h"

#define MAX_SCORE_HISTORY 100

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_SLOT_SELECTION,
    GAME_STATE_PLUG_SELECTION,
    GAME_STATE_PLAYER_SELECTION,
    GAME_STATE_DEVICE_SELECTION,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_RESULTS
} GameStateType;

typedef enum {
    INPUT_RESULT_NONE = 0,
    INPUT_RESULT_SUCCESS,
    INPUT_RESULT_FAIL,
    INPUT_RESULT_TOO_FAST,
    INPUT_RESULT_TOO_SLOW
} InputResult;

typedef struct {
    uint64_t score;
    uint64_t attempts;
    uint64_t successes;
    uint64_t failures;
    float accuracy_percentage;
    uint64_t session_start_time;
} SessionStats;

typedef struct {
    // Current game configuration
    Slot* active_slot;
    Plug* active_plug;
    PlayerSide player_side;
    InputDevice* input_device;
    
    // Current pattern state
    GameDirection* converted_pattern;  // Pattern converted for selected player
    int pattern_size;
    int current_position;
    
    // Game state
    GameStateType state;
    bool is_running;
    
    // Scoring and timing
    uint64_t score;
    uint64_t high_score;
    SessionStats session;
    
    // Input tracking
    InputResult last_input_result;
    GameDirection last_input_direction;
    uint64_t last_input_time;
    
    // Miss pause system
    bool in_miss_pause;
    uint64_t miss_pause_start_time;
    uint64_t miss_pause_duration_ns;
    
    // Managers
    SlotManager* slot_manager;
    PlugManager* plug_manager;
    
    // Menu navigation
    int selected_slot_index;
    int selected_plug_index;
    int selected_device_index;
    
} GameEngine;

// Core game engine functions
bool InitGameEngine(GameEngine* engine);
void DestroyGameEngine(GameEngine* engine);
void UpdateGameEngine(GameEngine* engine, float delta_time);

// State management
void ChangeGameState(GameEngine* engine, GameStateType new_state);
const char* GetGameStateName(GameStateType state);

// Game flow
void StartTrainingSession(GameEngine* engine);
void PauseTrainingSession(GameEngine* engine);
void ResumeTrainingSession(GameEngine* engine);
void EndTrainingSession(GameEngine* engine);
void ResetTrainingSession(GameEngine* engine);

// Input processing
void ProcessInput(GameEngine* engine, StandardInput* input);
InputResult ValidateInput(GameEngine* engine, GameDirection input_direction);

// Configuration
bool SelectSlot(GameEngine* engine, int slot_index);
bool SelectPlug(GameEngine* engine, int plug_index); 
bool SelectPlayerSide(GameEngine* engine, PlayerSide player);
bool SelectInputDevice(GameEngine* engine, InputDevice* device);

// Statistics
void UpdateSessionStats(GameEngine* engine, InputResult result);
void ResetSessionStats(GameEngine* engine);
float GetCurrentAccuracy(GameEngine* engine);
uint64_t GetSessionDuration(GameEngine* engine);

// Save/Load
bool SaveGameConfiguration(GameEngine* engine, const char* filename);
bool LoadGameConfiguration(GameEngine* engine, const char* filename);
bool SaveHighScores(GameEngine* engine, const char* filename);
bool LoadHighScores(GameEngine* engine, const char* filename);

// Utility
void PrintGameStatus(GameEngine* engine);
void PrintSessionReport(GameEngine* engine);
GameDirection GetExpectedInput(GameEngine* engine);
GameDirection GetNextExpectedInput(GameEngine* engine);
