#include "overlay.h"
#include <stdio.h>
#include <imgui.h>

// Forward declarations for DirectX hook functions
extern bool InitializeDirectXHook();
extern void ShutdownDirectXHook();

// Simplified game state for overlay (no SDL dependencies)
typedef struct {
    const char* mode_name;
    int pattern[8];
    int pattern_size;
} SimpleGameMode;

typedef struct {
    int player_pos;
    uint64_t score;
    uint64_t highscore;
    int last_input_acc; // 0=NONE, 1=SUCCESS, 2=FAIL
    int last_input;
    DWORD miss_time;
    bool in_miss_pause;
    SimpleGameMode* current_mode;
    bool is_active;
} SimpleGameState;

// Simple game modes for overlay
SimpleGameMode simple_modes[4] = {
    {"P1 KBD", {4, 0, 4, 6}, 4},       // Back, Neutral, Back, Down-Back
    {"P2 KBD", {2, 0, 2, 3}, 4},       // Forward, Neutral, Forward, Down-Forward
    {"P1 WD", {2, 0, 5, 3, 2, 0}, 6},  // Forward, Neutral, Down, Down-Forward, Forward, Neutral
    {"P2 WD", {4, 0, 5, 6, 4, 0}, 6}   // Back, Neutral, Down, Down-Back, Back, Neutral
};

SimpleGameState simple_gamestate = {0};
OverlayState g_overlay = {0};

void InitSimpleGame() {
    simple_gamestate.player_pos = 0;
    simple_gamestate.score = 0;
    simple_gamestate.highscore = 0;
    simple_gamestate.last_input_acc = 0;
    simple_gamestate.last_input = 0;
    simple_gamestate.miss_time = 0;
    simple_gamestate.in_miss_pause = false;
    simple_gamestate.current_mode = &simple_modes[0]; // Default to P1 KBD
    simple_gamestate.is_active = true;
}

bool InitializeOverlay(HWND target_window) {
    g_overlay.target_window = target_window;
    g_overlay.show_overlay = true;
    g_overlay.is_active = true;
    
    // Initialize the simple game state for overlay mode
    InitSimpleGame();
    
    return InitializeDirectXHook();
}

void ShutdownOverlay() {
    ShutdownDirectXHook();
    g_overlay.is_active = false;
}

void RenderOverlay() {
    if (!g_overlay.show_overlay || !g_overlay.is_active) {
        return;
    }
    
    // Create overlay window
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(320, 200), ImGuiCond_FirstUseEver);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | 
                                   ImGuiWindowFlags_AlwaysAutoResize |
                                   ImGuiWindowFlags_NoSavedSettings |
                                   ImGuiWindowFlags_NoFocusOnAppearing |
                                   ImGuiWindowFlags_NoNav |
                                   ImGuiWindowFlags_NoMove;
    
    // Custom title bar color
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.2f, 0.2f, 0.8f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.3f, 0.3f, 0.9f, 0.9f));
    
    if (ImGui::Begin("KBD Trainer Overlay", &g_overlay.show_overlay, window_flags)) {
        // Header with game info
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "🥊 KBD Trainer");
        ImGui::Separator();
        
        // Display current training mode
        if (simple_gamestate.current_mode) {
            ImGui::Text("Mode: %s", simple_gamestate.current_mode->mode_name);
            
            // Display input pattern
            ImGui::Text("Pattern:");
            ImGui::SameLine();
            
            const char* input_names[] = {"N", "U", "UF", "F", "DF", "D", "DB", "B", "UB"};
            for (int i = 0; i < simple_gamestate.current_mode->pattern_size; i++) {
                if (i > 0) ImGui::SameLine();
                
                int input = simple_gamestate.current_mode->pattern[i];
                if (i == simple_gamestate.player_pos % simple_gamestate.current_mode->pattern_size) {
                    // Highlight current input
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", input_names[input]);
                } else {
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", input_names[input]);
                }
                
                if (i < simple_gamestate.current_mode->pattern_size - 1) {
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "→");
                }
            }
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "No training mode selected");
        }
        
        ImGui::Separator();
        
        // Display score
        ImGui::Text("Score: %llu", simple_gamestate.score);
        ImGui::Text("High Score: %llu", simple_gamestate.highscore);
        
        // Display current input requirement
        if (simple_gamestate.current_mode && simple_gamestate.current_mode->pattern) {
            int next_input = simple_gamestate.current_mode->pattern[simple_gamestate.player_pos % simple_gamestate.current_mode->pattern_size];
            const char* input_names[] = {"N", "U", "UF", "F", "DF", "D", "DB", "B", "UB"};
            
            ImGui::Separator();
            ImGui::Text("Next Input:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "%s", input_names[next_input]);
        }
        
        // Display last result with animation
        if (simple_gamestate.last_input_acc == 1) { // SUCCESS
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "✓ GREAT!");
        } else if (simple_gamestate.last_input_acc == 2) { // FAIL
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "✗ MISS");
        }
        
        ImGui::Separator();
        
        // Control buttons
        if (ImGui::Button("Hide Overlay (F1)")) {
            g_overlay.show_overlay = false;
        }
        
        ImGui::SameLine();
        
        // Mode selection
        static int current_mode_idx = 0;
        const char* mode_names[4] = {
            simple_modes[0].mode_name,
            simple_modes[1].mode_name,
            simple_modes[2].mode_name,
            simple_modes[3].mode_name
        };
        
        if (ImGui::Combo("Mode", &current_mode_idx, mode_names, 4)) {
            simple_gamestate.current_mode = &simple_modes[current_mode_idx];
            simple_gamestate.player_pos = 0;
            simple_gamestate.last_input_acc = 0;
        }
        
        // Progress bar for current pattern
        if (simple_gamestate.current_mode) {
            float progress = (float)simple_gamestate.player_pos / simple_gamestate.current_mode->pattern_size;
            ImGui::ProgressBar(progress, ImVec2(-1.0f, 0.0f), "");
        }
    }
    ImGui::End();
    
    ImGui::PopStyleColor(2);
}

// Input processing function for overlay
void ProcessInput(int input) {
    if (!simple_gamestate.current_mode) return;
    
    // Handle miss pause - wait 2 seconds before resetting
    if (simple_gamestate.in_miss_pause) {
        DWORD current_time = GetTickCount();
        DWORD pause_duration = 2000; // 2 seconds in milliseconds
        
        if (current_time - simple_gamestate.miss_time >= pause_duration) {
            // Reset after pause
            simple_gamestate.player_pos = 0;
            simple_gamestate.score = 0;
            simple_gamestate.last_input_acc = 0;
            simple_gamestate.in_miss_pause = false;
        }
        return;
    }
    
    if (simple_gamestate.last_input_acc == 2) { // FAIL
        // Start miss pause timer
        simple_gamestate.miss_time = GetTickCount();
        simple_gamestate.in_miss_pause = true;
        return;
    }
    
    if (input == simple_gamestate.current_mode->pattern[simple_gamestate.player_pos % simple_gamestate.current_mode->pattern_size]) {
        // Correct input
        simple_gamestate.score += 50;
        if (simple_gamestate.score > simple_gamestate.highscore)
            simple_gamestate.highscore = simple_gamestate.score;
        
        if (simple_gamestate.player_pos == simple_gamestate.current_mode->pattern_size - 1)
            simple_gamestate.player_pos = 0;
        else
            simple_gamestate.player_pos += 1;
        
        simple_gamestate.last_input = input;
        simple_gamestate.last_input_acc = 1; // SUCCESS
    } else {
        // Incorrect input
        simple_gamestate.last_input = input;
        simple_gamestate.last_input_acc = 2; // FAIL
    }
}

// Input monitoring implementation
static int input_buffer[32] = {0};
static int buffer_pos = 0;
static DWORD last_input_time = 0;

void MonitorGameInputs() {
    // This is a simplified implementation
    // In a real overlay, you'd hook the game's input system directly
    
    DWORD current_time = GetTickCount();
    
    // Reset buffer if too much time has passed
    if (current_time - last_input_time > 1000) { // 1 second timeout
        buffer_pos = 0;
    }
    
    // Check for directional inputs (this is a placeholder)
    // Real implementation would hook the game's input processing
    
    bool input_detected = false;
    int detected_input = 0;
    
    // Simulate input detection for demonstration
    // This would be replaced with actual game input monitoring
    static int demo_inputs[] = {4, 0, 4, 6}; // Down-Back example
    static int demo_pos = 0;
    static DWORD last_demo_time = 0;
    
    if (current_time - last_demo_time > 500) { // Demo input every 500ms
        detected_input = demo_inputs[demo_pos % 4];
        demo_pos++;
        input_detected = true;
        last_demo_time = current_time;
    }
    
    if (input_detected) {
        input_buffer[buffer_pos] = detected_input;
        buffer_pos++;
        last_input_time = current_time;
        
        // Process the input
        if (simple_gamestate.current_mode) {
            ProcessInput(detected_input);
        }
        
        // Keep buffer size manageable
        if (buffer_pos >= 32) {
            buffer_pos = 0;
        }
    }
}

bool IsKBDInput(int input_sequence[], int length) {
    // Check if the input sequence matches KBD pattern
    // P1 KBD: ← N ← ↙ (4, 0, 4, 6)
    // P2 KBD: → N → ↘ (2, 0, 2, 3)
    
    if (length < 4) return false;
    
    // Check P1 KBD pattern
    if (input_sequence[length-4] == 4 && // Back
        input_sequence[length-3] == 0 && // Neutral  
        input_sequence[length-2] == 4 && // Back
        input_sequence[length-1] == 6) { // Down-Back
        return true;
    }
    
    // Check P2 KBD pattern
    if (input_sequence[length-4] == 2 && // Forward
        input_sequence[length-3] == 0 && // Neutral
        input_sequence[length-2] == 2 && // Forward
        input_sequence[length-1] == 3) { // Down-Forward
        return true;
    }
    
    return false;
}

bool IsWavedashInput(int input_sequence[], int length) {
    // Check if the input sequence matches Wavedash pattern
    // P1 WD: → N ↓ ↘ → N (2, 0, 5, 3, 2, 0)
    // P2 WD: ← N ↓ ↙ ← N (4, 0, 5, 6, 4, 0)
    
    if (length < 6) return false;
    
    // Check P1 Wavedash pattern
    if (input_sequence[length-6] == 2 && // Forward
        input_sequence[length-5] == 0 && // Neutral
        input_sequence[length-4] == 5 && // Down
        input_sequence[length-3] == 3 && // Down-Forward
        input_sequence[length-2] == 2 && // Forward
        input_sequence[length-1] == 0) { // Neutral
        return true;
    }
    
    // Check P2 Wavedash pattern
    if (input_sequence[length-6] == 4 && // Back
        input_sequence[length-5] == 0 && // Neutral
        input_sequence[length-4] == 5 && // Down
        input_sequence[length-3] == 6 && // Down-Back
        input_sequence[length-2] == 4 && // Back
        input_sequence[length-1] == 0) { // Neutral
        return true;
    }
    
    return false;
}

// DLL Main Entry Point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            // Initialize overlay when injected
            DisableThreadLibraryCalls(hModule);
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitializeOverlay, NULL, 0, NULL);
            break;
        case DLL_PROCESS_DETACH:
            ShutdownOverlay();
            break;
    }
    return TRUE;
}
