#include "../include/game/game_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Miss pause duration (2 seconds in nanoseconds)
#define MISS_PAUSE_DURATION_NS 2000000000ULL

bool InitGameEngine(GameEngine* engine)
{
    if (!engine) return false;
    
    // Initialize all fields to safe defaults
    memset(engine, 0, sizeof(GameEngine));
    
    // Initialize managers
    engine->slot_manager = malloc(sizeof(SlotManager));
    engine->plug_manager = malloc(sizeof(PlugManager));
    
    if (!engine->slot_manager || !engine->plug_manager) {
        DestroyGameEngine(engine);
        return false;
    }
    
    if (!InitSlotManager(engine->slot_manager) || 
        !InitPlugManager(engine->plug_manager)) {
        DestroyGameEngine(engine);
        return false;
    }
    
    // Set initial state
    engine->state = GAME_STATE_MENU;
    engine->is_running = true;
    engine->player_side = PLAYER_1;  // Default to P1
    engine->miss_pause_duration_ns = MISS_PAUSE_DURATION_NS;
    
    // Initialize menu selections
    engine->selected_slot_index = 0;
    engine->selected_plug_index = 0;
    engine->selected_device_index = 0;
    
    printf("Game Engine initialized successfully\n");
    return true;
}

void DestroyGameEngine(GameEngine* engine)
{
    if (!engine) return;
    
    // Clean up converted pattern
    if (engine->converted_pattern) {
        free(engine->converted_pattern);
        engine->converted_pattern = NULL;
    }
    
    // Clean up managers
    if (engine->slot_manager) {
        DestroySlotManager(engine->slot_manager);
        free(engine->slot_manager);
        engine->slot_manager = NULL;
    }
    
    if (engine->plug_manager) {
        DestroyPlugManager(engine->plug_manager);
        free(engine->plug_manager);
        engine->plug_manager = NULL;
    }
    
    // Reset engine state
    memset(engine, 0, sizeof(GameEngine));
    
    printf("Game Engine destroyed\n");
}

void UpdateGameEngine(GameEngine* engine, float delta_time)
{
    if (!engine || !engine->is_running) return;
    
    switch (engine->state) {
        case GAME_STATE_PLAYING:
            // Handle miss pause
            if (engine->in_miss_pause) {
                uint64_t current_time = GetCurrentTimeNS();
                if (current_time - engine->miss_pause_start_time >= engine->miss_pause_duration_ns) {
                    // End miss pause and reset
                    engine->in_miss_pause = false;
                    ResetTrainingSession(engine);
                }
            }
            break;
            
        case GAME_STATE_MENU:
        case GAME_STATE_SLOT_SELECTION:
        case GAME_STATE_PLUG_SELECTION:
        case GAME_STATE_PLAYER_SELECTION:
        case GAME_STATE_DEVICE_SELECTION:
        case GAME_STATE_PAUSED:
        case GAME_STATE_RESULTS:
            // These states don't require continuous updates
            break;
    }
    
    // Update input device if available
    if (engine->input_device) {
        StandardInput* input = PollInputDevice(engine->input_device);
        if (input) {
            ProcessInput(engine, input);
        }
    }
}

void ChangeGameState(GameEngine* engine, GameStateType new_state)
{
    if (!engine) return;
    
    GameStateType old_state = engine->state;
    engine->state = new_state;
    
    printf("Game state changed: %s -> %s\n", 
           GetGameStateName(old_state), 
           GetGameStateName(new_state));
    
    // Handle state entry logic
    switch (new_state) {
        case GAME_STATE_PLAYING:
            if (engine->active_plug && engine->active_slot) {
                StartTrainingSession(engine);
            }
            break;
            
        case GAME_STATE_MENU:
            // Reset selections when returning to menu
            engine->selected_slot_index = 0;
            engine->selected_plug_index = 0;
            engine->selected_device_index = 0;
            break;
            
        default:
            break;
    }
}

const char* GetGameStateName(GameStateType state)
{
    switch (state) {
        case GAME_STATE_MENU: return "Menu";
        case GAME_STATE_SLOT_SELECTION: return "Slot Selection";
        case GAME_STATE_PLUG_SELECTION: return "Plug Selection";
        case GAME_STATE_PLAYER_SELECTION: return "Player Selection";
        case GAME_STATE_DEVICE_SELECTION: return "Device Selection";
        case GAME_STATE_PLAYING: return "Playing";
        case GAME_STATE_PAUSED: return "Paused";
        case GAME_STATE_RESULTS: return "Results";
        default: return "Unknown";
    }
}

void StartTrainingSession(GameEngine* engine)
{
    if (!engine || !engine->active_plug) return;
    
    // Convert pattern for selected player side
    if (engine->converted_pattern) {
        free(engine->converted_pattern);
    }
    
    engine->converted_pattern = ConvertPatternForPlayer(
        engine->active_plug->pattern, 
        engine->active_plug->pattern_size, 
        engine->player_side
    );
    
    if (!engine->converted_pattern) {
        printf("Failed to convert pattern for player %s\n", GetPlayerSideName(engine->player_side));
        return;
    }
    
    engine->pattern_size = engine->active_plug->pattern_size;
    engine->current_position = 0;
    
    // Reset session
    ResetSessionStats(engine);
    engine->session.session_start_time = GetCurrentTimeNS();
    
    // Reset game state
    engine->score = 0;
    engine->last_input_result = INPUT_RESULT_NONE;
    engine->last_input_direction = NEUTRAL;
    engine->in_miss_pause = false;
    
    printf("Training session started: %s (%s)\n", 
           engine->active_plug->name, 
           GetPlayerSideName(engine->player_side));
    printf("Pattern: ");
    for (int i = 0; i < engine->pattern_size; i++) {
        printf("%s ", GetDirectionName(engine->converted_pattern[i]));
    }
    printf("\n");
}

void PauseTrainingSession(GameEngine* engine)
{
    if (!engine) return;
    
    if (engine->state == GAME_STATE_PLAYING) {
        ChangeGameState(engine, GAME_STATE_PAUSED);
    }
}

void ResumeTrainingSession(GameEngine* engine)
{
    if (!engine) return;
    
    if (engine->state == GAME_STATE_PAUSED) {
        ChangeGameState(engine, GAME_STATE_PLAYING);
    }
}

void EndTrainingSession(GameEngine* engine)
{
    if (!engine) return;
    
    // Update high score
    if (engine->score > engine->high_score) {
        engine->high_score = engine->score;
        printf("New high score: %llu!\n", (unsigned long long)engine->high_score);
    }
    
    // Print session report
    PrintSessionReport(engine);
    
    ChangeGameState(engine, GAME_STATE_RESULTS);
}

void ResetTrainingSession(GameEngine* engine)
{
    if (!engine) return;
    
    engine->current_position = 0;
    engine->score = 0;
    engine->last_input_result = INPUT_RESULT_NONE;
    engine->last_input_direction = NEUTRAL;
    engine->in_miss_pause = false;
    
    printf("Training session reset\n");
}

void ProcessInput(GameEngine* engine, StandardInput* input)
{
    if (!engine || !input) return;
    
    switch (engine->state) {
        case GAME_STATE_PLAYING:
            if (!engine->in_miss_pause) {
                // Only process direction input during gameplay
                if (input->direction != engine->last_input_direction) {
                    InputResult result = ValidateInput(engine, input->direction);
                    UpdateSessionStats(engine, result);
                    engine->last_input_direction = input->direction;
                    engine->last_input_time = input->timestamp_ns;
                }
            }
            
            // Handle pause/menu buttons
            if (input->buttons[MENU_BACK]) {
                EndTrainingSession(engine);
            }
            break;
            
        case GAME_STATE_MENU:
        case GAME_STATE_SLOT_SELECTION:
        case GAME_STATE_PLUG_SELECTION:
        case GAME_STATE_PLAYER_SELECTION:
        case GAME_STATE_DEVICE_SELECTION:
            // Handle menu navigation (simplified for now)
            if (input->buttons[MENU_SELECT]) {
                // Advance to next state or start game
                // This would be more sophisticated in a full implementation
            }
            break;
            
        case GAME_STATE_PAUSED:
            if (input->buttons[MENU_SELECT]) {
                ResumeTrainingSession(engine);
            } else if (input->buttons[MENU_BACK]) {
                EndTrainingSession(engine);
            }
            break;
            
        case GAME_STATE_RESULTS:
            if (input->buttons[MENU_SELECT] || input->buttons[MENU_BACK]) {
                ChangeGameState(engine, GAME_STATE_MENU);
            }
            break;
    }
}

InputResult ValidateInput(GameEngine* engine, GameDirection input_direction)
{
    if (!engine || !engine->converted_pattern) return INPUT_RESULT_NONE;
    
    GameDirection expected = engine->converted_pattern[engine->current_position];
    
    // Check if input matches expected direction
    if (input_direction == expected) {
        // Correct input
        engine->score += 50;  // Base score per correct input
        
        // Advance to next position
        engine->current_position = (engine->current_position + 1) % engine->pattern_size;
        
        engine->last_input_result = INPUT_RESULT_SUCCESS;
        printf("✓ Correct: %s\n", GetDirectionName(input_direction));
        
        return INPUT_RESULT_SUCCESS;
    } else {
        // Incorrect input
        if (engine->current_position != 0) {
            // Only fail if not at the start of pattern
            engine->last_input_result = INPUT_RESULT_FAIL;
            
            // Start miss pause
            engine->in_miss_pause = true;
            engine->miss_pause_start_time = GetCurrentTimeNS();
            
            printf("✗ Failed: got %s, expected %s\n", 
                   GetDirectionName(input_direction), 
                   GetDirectionName(expected));
            
            return INPUT_RESULT_FAIL;
        } else {
            // At start of pattern, just ignore wrong input
            printf("- Ignored: %s (expected %s)\n", 
                   GetDirectionName(input_direction), 
                   GetDirectionName(expected));
            
            return INPUT_RESULT_NONE;
        }
    }
}

bool SelectSlot(GameEngine* engine, int slot_index)
{
    if (!engine || !engine->slot_manager) return false;
    
    if (slot_index >= 0 && slot_index < engine->slot_manager->count) {
        engine->active_slot = &engine->slot_manager->slots[slot_index];
        engine->selected_slot_index = slot_index;
        
        printf("Selected slot: %s\n", engine->active_slot->name);
        return true;
    }
    
    return false;
}

bool SelectPlug(GameEngine* engine, int plug_index)
{
    if (!engine || !engine->plug_manager) return false;
    
    if (plug_index >= 0 && plug_index < engine->plug_manager->count) {
        engine->active_plug = &engine->plug_manager->plugs[plug_index];
        engine->selected_plug_index = plug_index;
        
        printf("Selected plug: %s\n", engine->active_plug->name);
        return true;
    }
    
    return false;
}

bool SelectPlayerSide(GameEngine* engine, PlayerSide player)
{
    if (!engine) return false;
    
    engine->player_side = player;
    printf("Selected player side: %s\n", GetPlayerSideName(player));
    
    return true;
}

bool SelectInputDevice(GameEngine* engine, InputDevice* device)
{
    if (!engine || !device) return false;
    
    engine->input_device = device;
    printf("Selected input device: %s\n", device->info.device_name);
    
    return true;
}

void UpdateSessionStats(GameEngine* engine, InputResult result)
{
    if (!engine) return;
    
    engine->session.attempts++;
    
    switch (result) {
        case INPUT_RESULT_SUCCESS:
            engine->session.successes++;
            break;
        case INPUT_RESULT_FAIL:
        case INPUT_RESULT_TOO_FAST:
        case INPUT_RESULT_TOO_SLOW:
            engine->session.failures++;
            break;
        default:
            break;
    }
    
    // Calculate accuracy
    if (engine->session.attempts > 0) {
        engine->session.accuracy_percentage = 
            (float)engine->session.successes / engine->session.attempts * 100.0f;
    }
}

void ResetSessionStats(GameEngine* engine)
{
    if (!engine) return;
    
    memset(&engine->session, 0, sizeof(SessionStats));
}

float GetCurrentAccuracy(GameEngine* engine)
{
    if (!engine) return 0.0f;
    
    return engine->session.accuracy_percentage;
}

uint64_t GetSessionDuration(GameEngine* engine)
{
    if (!engine || engine->session.session_start_time == 0) return 0;
    
    uint64_t current_time = GetCurrentTimeNS();
    return current_time - engine->session.session_start_time;
}

void PrintGameStatus(GameEngine* engine)
{
    if (!engine) return;
    
    printf("\n=== Game Status ===\n");
    printf("State: %s\n", GetGameStateName(engine->state));
    
    if (engine->active_slot) {
        printf("Slot: %s\n", engine->active_slot->name);
    }
    
    if (engine->active_plug) {
        printf("Plug: %s\n", engine->active_plug->name);
        printf("Player: %s\n", GetPlayerSideName(engine->player_side));
        printf("Position: %d/%d\n", engine->current_position + 1, engine->pattern_size);
        
        if (engine->converted_pattern) {
            printf("Next input: %s\n", 
                   GetDirectionName(engine->converted_pattern[engine->current_position]));
        }
    }
    
    if (engine->input_device) {
        printf("Input: %s\n", engine->input_device->info.device_name);
    }
    
    printf("Score: %llu (High: %llu)\n", 
           (unsigned long long)engine->score, 
           (unsigned long long)engine->high_score);
    
    printf("Accuracy: %.1f%% (%llu/%llu)\n", 
           engine->session.accuracy_percentage,
           (unsigned long long)engine->session.successes,
           (unsigned long long)engine->session.attempts);
    
    printf("==================\n\n");
}

void PrintSessionReport(GameEngine* engine)
{
    if (!engine) return;
    
    uint64_t duration_ns = GetSessionDuration(engine);
    float duration_seconds = duration_ns / 1000000000.0f;
    
    printf("\n=== Session Report ===\n");
    printf("Duration: %.1f seconds\n", duration_seconds);
    printf("Total attempts: %llu\n", (unsigned long long)engine->session.attempts);
    printf("Successful inputs: %llu\n", (unsigned long long)engine->session.successes);
    printf("Failed inputs: %llu\n", (unsigned long long)engine->session.failures);
    printf("Accuracy: %.1f%%\n", engine->session.accuracy_percentage);
    printf("Final score: %llu\n", (unsigned long long)engine->score);
    
    if (engine->session.attempts > 0 && duration_seconds > 0) {
        float inputs_per_second = engine->session.attempts / duration_seconds;
        printf("Input rate: %.1f inputs/second\n", inputs_per_second);
    }
    
    printf("=====================\n\n");
}

GameDirection GetExpectedInput(GameEngine* engine)
{
    if (!engine || !engine->converted_pattern) return NEUTRAL;
    
    return engine->converted_pattern[engine->current_position];
}

GameDirection GetNextExpectedInput(GameEngine* engine)
{
    if (!engine || !engine->converted_pattern) return NEUTRAL;
    
    int next_pos = (engine->current_position + 1) % engine->pattern_size;
    return engine->converted_pattern[next_pos];
}

bool SaveGameConfiguration(GameEngine* engine, const char* filename)
{
    if (!engine || !filename) return false;
    
    FILE* file = fopen(filename, "w");
    if (!file) return false;
    
    fprintf(file, "# KBD Trainer Game Configuration\n");
    fprintf(file, "player_side=%d\n", engine->player_side);
    fprintf(file, "high_score=%llu\n", (unsigned long long)engine->high_score);
    
    if (engine->active_slot) {
        fprintf(file, "active_slot_id=%s\n", engine->active_slot->id);
    }
    
    if (engine->active_plug) {
        fprintf(file, "active_plug_id=%s\n", engine->active_plug->id);
    }
    
    if (engine->input_device) {
        fprintf(file, "input_device_name=%s\n", engine->input_device->info.device_name);
    }
    
    fclose(file);
    printf("Game configuration saved to %s\n", filename);
    return true;
}

bool LoadGameConfiguration(GameEngine* engine, const char* filename)
{
    if (!engine || !filename) return false;
    
    FILE* file = fopen(filename, "r");
    if (!file) return false;
    
    char line[256];
    char slot_id[GUID_STRING_LENGTH] = {0};
    char plug_id[GUID_STRING_LENGTH] = {0};
    
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "player_side=", 12) == 0) {
            int player;
            sscanf(line, "player_side=%d", &player);
            engine->player_side = (PlayerSide)player;
        } else if (strncmp(line, "high_score=", 11) == 0) {
            sscanf(line, "high_score=%llu", (unsigned long long*)&engine->high_score);
        } else if (strncmp(line, "active_slot_id=", 15) == 0) {
            sscanf(line, "active_slot_id=%36s", slot_id);
        } else if (strncmp(line, "active_plug_id=", 15) == 0) {
            sscanf(line, "active_plug_id=%36s", plug_id);
        }
    }
    
    fclose(file);
    
    // Try to restore slot and plug selections
    if (strlen(slot_id) > 0) {
        Slot* slot = FindSlotById(engine->slot_manager, slot_id);
        if (slot) {
            engine->active_slot = slot;
            // Find index for menu navigation
            for (int i = 0; i < engine->slot_manager->count; i++) {
                if (&engine->slot_manager->slots[i] == slot) {
                    engine->selected_slot_index = i;
                    break;
                }
            }
        }
    }
    
    if (strlen(plug_id) > 0) {
        Plug* plug = FindPlugById(engine->plug_manager, plug_id);
        if (plug) {
            engine->active_plug = plug;
            // Find index for menu navigation
            for (int i = 0; i < engine->plug_manager->count; i++) {
                if (&engine->plug_manager->plugs[i] == plug) {
                    engine->selected_plug_index = i;
                    break;
                }
            }
        }
    }
    
    printf("Game configuration loaded from %s\n", filename);
    return true;
}

bool SaveHighScores(GameEngine* engine, const char* filename)
{
    if (!engine || !filename) return false;
    
    FILE* file = fopen(filename, "w");
    if (!file) return false;
    
    fprintf(file, "# KBD Trainer High Scores\n");
    fprintf(file, "global_high_score=%llu\n", (unsigned long long)engine->high_score);
    
    // In a more complete implementation, we would save per-technique high scores
    // For now, just save the current high score
    
    fclose(file);
    printf("High scores saved to %s\n", filename);
    return true;
}

bool LoadHighScores(GameEngine* engine, const char* filename)
{
    if (!engine || !filename) return false;
    
    FILE* file = fopen(filename, "r");
    if (!file) return false;
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "global_high_score=", 18) == 0) {
            sscanf(line, "global_high_score=%llu", (unsigned long long*)&engine->high_score);
        }
    }
    
    fclose(file);
    printf("High scores loaded from %s\n", filename);
    return true;
}
