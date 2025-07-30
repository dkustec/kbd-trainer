#pragma once

#include <stdbool.h>
#include "player.h"
#include "slot.h"  // For GUID_STRING_LENGTH

#define MAX_PLUG_NAME_LENGTH 64
#define MAX_PLUG_DESCRIPTION_LENGTH 256
#define MAX_PATTERN_SIZE 32
#define MAX_PLUGS 64

typedef struct {
    char name[MAX_PLUG_NAME_LENGTH];
    char id[GUID_STRING_LENGTH];
    char description[MAX_PLUG_DESCRIPTION_LENGTH];
    char slot_id[GUID_STRING_LENGTH];  // Which slot this plug fits into
    
    GameDirection pattern[MAX_PATTERN_SIZE];
    int pattern_size;
} Plug;

typedef struct {
    Plug plugs[MAX_PLUGS];
    int count;
} PlugManager;

// Plug management functions
bool InitPlugManager(PlugManager* manager);
void DestroyPlugManager(PlugManager* manager);

// Plug operations
bool AddPlug(PlugManager* manager, const char* name, const char* slot_id, 
             const char* description, const GameDirection* pattern, int pattern_size);
Plug* FindPlugById(PlugManager* manager, const char* id);
Plug* FindPlugByName(PlugManager* manager, const char* name);
bool RemovePlug(PlugManager* manager, const char* id);

// Get plugs for specific slot
int GetPlugsForSlot(PlugManager* manager, const char* slot_id, Plug** plugs_out);

// Pattern operations
GameDirection* GetPlugPatternForPlayer(Plug* plug, PlayerSide player);
bool ValidatePlugPattern(const GameDirection* pattern, int pattern_size);

// Default plugs
void CreateDefaultPlugs(PlugManager* manager, const char* movement_slot_id);

// Persistence
bool SavePlugs(PlugManager* manager, const char* filename);
bool LoadPlugs(PlugManager* manager, const char* filename);

// Utility (from slot.h)
extern void GenerateGUID(char* guid_str);
