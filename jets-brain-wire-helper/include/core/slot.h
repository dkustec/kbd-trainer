#pragma once

#include <stdbool.h>

#define MAX_SLOT_NAME_LENGTH 64
#define MAX_SLOT_DESCRIPTION_LENGTH 256
#define MAX_SLOT_TYPE_LENGTH 32
#define GUID_STRING_LENGTH 37  // 36 chars + null terminator
#define MAX_SLOTS 16

typedef struct {
    char name[MAX_SLOT_NAME_LENGTH];
    char id[GUID_STRING_LENGTH];
    char description[MAX_SLOT_DESCRIPTION_LENGTH];
    char type[MAX_SLOT_TYPE_LENGTH];
} Slot;

typedef struct {
    Slot slots[MAX_SLOTS];
    int count;
} SlotManager;

// Slot management functions
bool InitSlotManager(SlotManager* manager);
void DestroySlotManager(SlotManager* manager);

// Slot operations
bool AddSlot(SlotManager* manager, const char* name, const char* type, const char* description);
Slot* FindSlotById(SlotManager* manager, const char* id);
Slot* FindSlotByName(SlotManager* manager, const char* name);
bool RemoveSlot(SlotManager* manager, const char* id);

// Default slots
void CreateDefaultSlots(SlotManager* manager);

// Persistence
bool SaveSlots(SlotManager* manager, const char* filename);
bool LoadSlots(SlotManager* manager, const char* filename);

// Utility
void GenerateGUID(char* guid_str);
const char* GetSlotTypeName(int index);
