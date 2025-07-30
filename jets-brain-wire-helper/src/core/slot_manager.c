#include "../include/core/slot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <rpc.h>
#pragma comment(lib, "rpcrt4.lib")
#else
#include <uuid/uuid.h>
#endif

bool InitSlotManager(SlotManager* manager)
{
    if (!manager) return false;
    
    manager->count = 0;
    memset(manager->slots, 0, sizeof(manager->slots));
    
    // Create default slots
    CreateDefaultSlots(manager);
    
    return true;
}

void DestroySlotManager(SlotManager* manager)
{
    if (!manager) return;
    
    manager->count = 0;
    memset(manager->slots, 0, sizeof(manager->slots));
}

bool AddSlot(SlotManager* manager, const char* name, const char* type, const char* description)
{
    if (!manager || !name || !type || manager->count >= MAX_SLOTS) {
        return false;
    }
    
    Slot* slot = &manager->slots[manager->count];
    
    // Generate GUID for new slot
    GenerateGUID(slot->id);
    
    // Copy strings with bounds checking
    strncpy(slot->name, name, MAX_SLOT_NAME_LENGTH - 1);
    slot->name[MAX_SLOT_NAME_LENGTH - 1] = '\0';
    
    strncpy(slot->type, type, MAX_SLOT_TYPE_LENGTH - 1);
    slot->type[MAX_SLOT_TYPE_LENGTH - 1] = '\0';
    
    if (description) {
        strncpy(slot->description, description, MAX_SLOT_DESCRIPTION_LENGTH - 1);
        slot->description[MAX_SLOT_DESCRIPTION_LENGTH - 1] = '\0';
    } else {
        slot->description[0] = '\0';
    }
    
    manager->count++;
    return true;
}

Slot* FindSlotById(SlotManager* manager, const char* id)
{
    if (!manager || !id) return NULL;
    
    for (int i = 0; i < manager->count; i++) {
        if (strcmp(manager->slots[i].id, id) == 0) {
            return &manager->slots[i];
        }
    }
    
    return NULL;
}

Slot* FindSlotByName(SlotManager* manager, const char* name)
{
    if (!manager || !name) return NULL;
    
    for (int i = 0; i < manager->count; i++) {
        if (strcmp(manager->slots[i].name, name) == 0) {
            return &manager->slots[i];
        }
    }
    
    return NULL;
}

bool RemoveSlot(SlotManager* manager, const char* id)
{
    if (!manager || !id) return false;
    
    for (int i = 0; i < manager->count; i++) {
        if (strcmp(manager->slots[i].id, id) == 0) {
            // Shift remaining slots down
            for (int j = i; j < manager->count - 1; j++) {
                manager->slots[j] = manager->slots[j + 1];
            }
            manager->count--;
            return true;
        }
    }
    
    return false;
}

void CreateDefaultSlots(SlotManager* manager)
{
    if (!manager) return;
    
    // Default "Movements" slot
    AddSlot(manager, "Movements", "Mechanics", 
            "Basic movement techniques like KBD and Wave Dash");
}

bool SaveSlots(SlotManager* manager, const char* filename)
{
    if (!manager || !filename) return false;
    
    FILE* file = fopen(filename, "w");
    if (!file) return false;
    
    fprintf(file, "# KBD Trainer Slots Configuration\n");
    fprintf(file, "slot_count=%d\n\n", manager->count);
    
    for (int i = 0; i < manager->count; i++) {
        Slot* slot = &manager->slots[i];
        fprintf(file, "[slot_%d]\n", i);
        fprintf(file, "id=%s\n", slot->id);
        fprintf(file, "name=%s\n", slot->name);
        fprintf(file, "type=%s\n", slot->type);
        fprintf(file, "description=%s\n\n", slot->description);
    }
    
    fclose(file);
    return true;
}

bool LoadSlots(SlotManager* manager, const char* filename)
{
    if (!manager || !filename) return false;
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        // File doesn't exist, create default slots
        CreateDefaultSlots(manager);
        return true;
    }
    
    char line[512];
    manager->count = 0;
    
    // Simple parser - in a real implementation you'd want more robust parsing
    while (fgets(line, sizeof(line), file) && manager->count < MAX_SLOTS) {
        if (strncmp(line, "[slot_", 6) == 0) {
            Slot* slot = &manager->slots[manager->count];
            
            // Read slot properties
            if (fgets(line, sizeof(line), file)) { // id
                sscanf(line, "id=%36s", slot->id);
            }
            if (fgets(line, sizeof(line), file)) { // name
                sscanf(line, "name=%63[^\n]", slot->name);
            }
            if (fgets(line, sizeof(line), file)) { // type
                sscanf(line, "type=%31[^\n]", slot->type);
            }
            if (fgets(line, sizeof(line), file)) { // description
                sscanf(line, "description=%255[^\n]", slot->description);
            }
            
            manager->count++;
        }
    }
    
    fclose(file);
    
    // If no slots were loaded, create defaults
    if (manager->count == 0) {
        CreateDefaultSlots(manager);
    }
    
    return true;
}

void GenerateGUID(char* guid_str)
{
    if (!guid_str) return;
    
#ifdef _WIN32
    UUID uuid;
    UuidCreate(&uuid);
    RPC_CSTR uuid_str;
    UuidToStringA(&uuid, &uuid_str);
    strncpy(guid_str, (char*)uuid_str, GUID_STRING_LENGTH - 1);
    guid_str[GUID_STRING_LENGTH - 1] = '\0';
    RpcStringFreeA(&uuid_str);
#else
    uuid_t uuid;
    uuid_generate(uuid);
    uuid_unparse(uuid, guid_str);
#endif
}

const char* GetSlotTypeName(int index)
{
    static const char* slot_types[] = {
        "Mechanics",
        "Combos", 
        "Defensive",
        "Movement",
        "Advanced",
        "Custom"
    };
    
    int type_count = sizeof(slot_types) / sizeof(slot_types[0]);
    if (index >= 0 && index < type_count) {
        return slot_types[index];
    }
    
    return "Unknown";
}
