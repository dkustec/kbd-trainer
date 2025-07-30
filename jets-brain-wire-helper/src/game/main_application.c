#include "../include/game/game_engine.h"
#include "../include/input/input_device.h"
#include "../include/core/slot.h"
#include "../include/core/plug.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_DEVICES 8

// Application state
typedef struct {
    GameEngine game_engine;
    InputDevice* available_devices[MAX_DEVICES];
    int device_count;
    bool should_quit;
} Application;

// Function prototypes
bool InitApplication(Application* app);
void DestroyApplication(Application* app);
void RunApplication(Application* app);
void HandleApplicationInput(Application* app);
void UpdateApplication(Application* app, float delta_time);
void ShowMainMenu(Application* app);
void ShowSlotSelection(Application* app);
void ShowPlugSelection(Application* app);
void ShowPlayerSelection(Application* app);
void ShowDeviceSelection(Application* app);
void SetupDefaultConfiguration(Application* app);

int main(int argc, char* argv[])
{
    printf("KBD Trainer - Modern Architecture\n");
    printf("==================================\n\n");
    
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK)) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    
    Application app;
    memset(&app, 0, sizeof(Application));
    
    if (!InitApplication(&app)) {
        printf("Failed to initialize application\n");
        DestroyApplication(&app);
        SDL_Quit();
        return 1;
    }
    
    // Main application loop
    RunApplication(&app);
    
    // Cleanup
    DestroyApplication(&app);
    SDL_Quit();
    
    printf("Application terminated successfully\n");
    return 0;
}

bool InitApplication(Application* app)
{
    if (!app) return false;
    
    // Initialize game engine
    if (!InitGameEngine(&app->game_engine)) {
        printf("Failed to initialize game engine\n");
        return false;
    }
    
    // Detect input devices
    app->device_count = DetectInputDevices(app->available_devices, MAX_DEVICES);
    printf("Detected %d input devices\n", app->device_count);
    
    if (app->device_count == 0) {
        printf("Warning: No input devices detected\n");
    }
    
    // Setup default configuration
    SetupDefaultConfiguration(app);
    
    app->should_quit = false;
    
    return true;
}

void DestroyApplication(Application* app)
{
    if (!app) return;
    
    // Cleanup input devices
    for (int i = 0; i < app->device_count; i++) {
        if (app->available_devices[i]) {
            DestroyInputDevice(app->available_devices[i]);
            free(app->available_devices[i]);
            app->available_devices[i] = NULL;
        }
    }
    
    // Cleanup game engine
    DestroyGameEngine(&app->game_engine);
}

void RunApplication(Application* app)
{
    if (!app) return;
    
    uint64_t last_time = GetCurrentTimeNS();
    const uint64_t target_frame_time = FRAME_DURATION_NS;
    
    printf("Starting main application loop...\n");
    ShowMainMenu(app);
    
    while (!app->should_quit) {
        uint64_t current_time = GetCurrentTimeNS();
        uint64_t delta_time_ns = current_time - last_time;
        float delta_time = delta_time_ns / 1000000000.0f;  // Convert to seconds
        
        // Handle SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                app->should_quit = true;
            }
        }
        
        // Update application
        UpdateApplication(app, delta_time);
        
        // Cap frame rate to 60 FPS
        if (delta_time_ns < target_frame_time) {
            uint64_t sleep_time_ns = target_frame_time - delta_time_ns;
            SDL_DelayNS(sleep_time_ns);
        }
        
        last_time = current_time;
    }
}

void UpdateApplication(Application* app, float delta_time)
{
    if (!app) return;
    
    // Update game engine
    UpdateGameEngine(&app->game_engine, delta_time);
    
    // Handle console input for menu navigation
    // In a real application, this would be handled by the input system
    HandleApplicationInput(app);
}

void HandleApplicationInput(Application* app)
{
    if (!app) return;
    
    // Simple console input handling for demonstration
    // In a real application, this would be integrated with the input system
    
    switch (app->game_engine.state) {
        case GAME_STATE_MENU:
            // Main menu is already shown, wait for user input
            break;
            
        case GAME_STATE_SLOT_SELECTION:
            ShowSlotSelection(app);
            break;
            
        case GAME_STATE_PLUG_SELECTION:
            ShowPlugSelection(app);
            break;
            
        case GAME_STATE_PLAYER_SELECTION:
            ShowPlayerSelection(app);
            break;
            
        case GAME_STATE_DEVICE_SELECTION:
            ShowDeviceSelection(app);
            break;
            
        case GAME_STATE_PLAYING:
            // Game engine handles input during gameplay
            break;
            
        case GAME_STATE_RESULTS:
            printf("Press Enter to return to menu...\n");
            getchar();
            ChangeGameState(&app->game_engine, GAME_STATE_MENU);
            ShowMainMenu(app);
            break;
            
        default:
            break;
    }
}

void ShowMainMenu(Application* app)
{
    if (!app) return;
    
    printf("\n=== KBD Trainer - Main Menu ===\n");
    printf("1. Start Training\n");
    printf("2. Configure Input Device\n");
    printf("3. View Statistics\n");
    printf("4. Exit\n");
    printf("===============================\n");
    printf("Choice: ");
    
    int choice;
    if (scanf("%d", &choice) == 1) {
        switch (choice) {
            case 1:
                ChangeGameState(&app->game_engine, GAME_STATE_SLOT_SELECTION);
                break;
            case 2:
                ChangeGameState(&app->game_engine, GAME_STATE_DEVICE_SELECTION);
                break;
            case 3:
                PrintGameStatus(&app->game_engine);
                ShowMainMenu(app);
                break;
            case 4:
                app->should_quit = true;
                break;
            default:
                printf("Invalid choice\n");
                ShowMainMenu(app);
                break;
        }
    }
}

void ShowSlotSelection(Application* app)
{
    if (!app || !app->game_engine.slot_manager) return;
    
    SlotManager* sm = app->game_engine.slot_manager;
    
    printf("\n=== Select Technique Category ===\n");
    for (int i = 0; i < sm->count; i++) {
        printf("%d. %s - %s\n", i + 1, sm->slots[i].name, sm->slots[i].description);
    }
    printf("=================================\n");
    printf("Choice: ");
    
    int choice;
    if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= sm->count) {
        SelectSlot(&app->game_engine, choice - 1);
        ChangeGameState(&app->game_engine, GAME_STATE_PLUG_SELECTION);
    } else {
        printf("Invalid choice\n");
        ShowSlotSelection(app);
    }
}

void ShowPlugSelection(Application* app)
{
    if (!app || !app->game_engine.plug_manager) return;
    
    PlugManager* pm = app->game_engine.plug_manager;
    
    printf("\n=== Select Technique ===\n");
    for (int i = 0; i < pm->count; i++) {
        printf("%d. %s - %s\n", i + 1, pm->plugs[i].name, pm->plugs[i].description);
    }
    printf("========================\n");
    printf("Choice: ");
    
    int choice;
    if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= pm->count) {
        SelectPlug(&app->game_engine, choice - 1);
        ChangeGameState(&app->game_engine, GAME_STATE_PLAYER_SELECTION);
    } else {
        printf("Invalid choice\n");
        ShowPlugSelection(app);
    }
}

void ShowPlayerSelection(Application* app)
{
    if (!app) return;
    
    printf("\n=== Select Player Side ===\n");
    printf("1. Player 1 (P1)\n");
    printf("2. Player 2 (P2)\n");
    printf("==========================\n");
    printf("Choice: ");
    
    int choice;
    if (scanf("%d", &choice) == 1) {
        PlayerSide player = (choice == 2) ? PLAYER_2 : PLAYER_1;
        SelectPlayerSide(&app->game_engine, player);
        
        if (app->game_engine.input_device) {
            // Already have input device, start training
            ChangeGameState(&app->game_engine, GAME_STATE_PLAYING);
        } else {
            ChangeGameState(&app->game_engine, GAME_STATE_DEVICE_SELECTION);
        }
    } else {
        printf("Invalid choice\n");
        ShowPlayerSelection(app);
    }
}

void ShowDeviceSelection(Application* app)
{
    if (!app) return;
    
    printf("\n=== Select Input Device ===\n");
    for (int i = 0; i < app->device_count; i++) {
        printf("%d. %s (%s)\n", i + 1, 
               app->available_devices[i]->info.device_name,
               GetConnectionTypeName(app->available_devices[i]->info.connection_type));
        
        if (app->available_devices[i]->info.battery_level >= 0) {
            printf("   Battery: %d%%\n", app->available_devices[i]->info.battery_level);
        }
    }
    printf("===========================\n");
    printf("Choice: ");
    
    int choice;
    if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= app->device_count) {
        SelectInputDevice(&app->game_engine, app->available_devices[choice - 1]);
        
        // Show device info
        PrintDeviceInfo(&app->available_devices[choice - 1]->info);
        
        if (app->game_engine.state == GAME_STATE_DEVICE_SELECTION && 
            app->game_engine.active_plug) {
            // We have everything needed, start training
            ChangeGameState(&app->game_engine, GAME_STATE_PLAYING);
        } else {
            // Return to main menu
            ChangeGameState(&app->game_engine, GAME_STATE_MENU);
            ShowMainMenu(app);
        }
    } else {
        printf("Invalid choice\n");
        ShowDeviceSelection(app);
    }
}

void SetupDefaultConfiguration(Application* app)
{
    if (!app) return;
    
    GameEngine* engine = &app->game_engine;
    
    // Create default plugs if slot manager has the movement slot
    if (engine->slot_manager && engine->slot_manager->count > 0) {
        Slot* movement_slot = &engine->slot_manager->slots[0];  // First slot should be "Movements"
        CreateDefaultPlugs(engine->plug_manager, movement_slot->id);
        
        printf("Created default plugs for slot: %s\n", movement_slot->name);
    }
    
    // Auto-select first available input device if any
    if (app->device_count > 0) {
        SelectInputDevice(engine, app->available_devices[0]);
        printf("Auto-selected input device: %s\n", 
               app->available_devices[0]->info.device_name);
    }
    
    printf("Default configuration complete\n");
}
