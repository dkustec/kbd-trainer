#include "../include/core/plug.h"
#include "../include/core/player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool InitPlugManager(PlugManager* manager)
{
    if (!manager) return false;
    
    manager->count = 0;
    memset(manager->plugs, 0, sizeof(manager->plugs));
    
    return true;
}

void DestroyPlugManager(PlugManager* manager)
{
    if (!manager) return;
    
    manager->count = 0;
    memset(manager->plugs, 0, sizeof(manager->plugs));
}

bool AddPlug(PlugManager* manager, const char* name, const char* slot_id, 
             const char* description, const GameDirection* pattern, int pattern_size)
{
    if (!manager || !name || !slot_id || !pattern || 
        pattern_size <= 0 || pattern_size > MAX_PATTERN_SIZE ||
        manager->count >= MAX_PLUGS) {
        return false;
    }
    
    // Validate pattern
    if (!ValidatePlugPattern(pattern, pattern_size)) {
        return false;
    }
    
    Plug* plug = &manager->plugs[manager->count];
    
    // Generate GUID for new plug
    GenerateGUID(plug->id);
    
    // Copy strings with bounds checking
    strncpy(plug->name, name, MAX_PLUG_NAME_LENGTH - 1);
    plug->name[MAX_PLUG_NAME_LENGTH - 1] = '\0';
    
    strncpy(plug->slot_id, slot_id, GUID_STRING_LENGTH - 1);
    plug->slot_id[GUID_STRING_LENGTH - 1] = '\0';
    
    if (description) {
        strncpy(plug->description, description, MAX_PLUG_DESCRIPTION_LENGTH - 1);
        plug->description[MAX_PLUG_DESCRIPTION_LENGTH - 1] = '\0';
    } else {
        plug->description[0] = '\0';
    }
    
    // Copy pattern
    memcpy(plug->pattern, pattern, pattern_size * sizeof(GameDirection));
    plug->pattern_size = pattern_size;
    
    manager->count++;
    return true;
}

Plug* FindPlugById(PlugManager* manager, const char* id)
{
    if (!manager || !id) return NULL;
    
    for (int i = 0; i < manager->count; i++) {
        if (strcmp(manager->plugs[i].id, id) == 0) {
            return &manager->plugs[i];
        }
    }
    
    return NULL;
}

Plug* FindPlugByName(PlugManager* manager, const char* name)
{
    if (!manager || !name) return NULL;
    
    for (int i = 0; i < manager->count; i++) {
        if (strcmp(manager->plugs[i].name, name) == 0) {
            return &manager->plugs[i];
        }
    }
    
    return NULL;
}

bool RemovePlug(PlugManager* manager, const char* id)
{
    if (!manager || !id) return false;
    
    for (int i = 0; i < manager->count; i++) {
        if (strcmp(manager->plugs[i].id, id) == 0) {
            // Shift remaining plugs down
            for (int j = i; j < manager->count - 1; j++) {
                manager->plugs[j] = manager->plugs[j + 1];
            }
            manager->count--;
            return true;
        }
    }
    
    return false;
}

int GetPlugsForSlot(PlugManager* manager, const char* slot_id, Plug** plugs_out)
{
    if (!manager || !slot_id || !plugs_out) return 0;
    
    int found_count = 0;
    for (int i = 0; i < manager->count; i++) {
        if (strcmp(manager->plugs[i].slot_id, slot_id) == 0) {
            plugs_out[found_count] = &manager->plugs[i];
            found_count++;
        }
    }
    
    return found_count;
}

GameDirection* GetPlugPatternForPlayer(Plug* plug, PlayerSide player)
{
    if (!plug) return NULL;
    
    return ConvertPatternForPlayer(plug->pattern, plug->pattern_size, player);
}

bool ValidatePlugPattern(const GameDirection* pattern, int pattern_size)
{
    if (!pattern || pattern_size <= 0 || pattern_size > MAX_PATTERN_SIZE) {
        return false;
    }
    
    // Check for valid directions
    for (int i = 0; i < pattern_size; i++) {
        if (pattern[i] < NEUTRAL || pattern[i] >= UNKNOWN) {
            return false;
        }
    }
    
    return true;
}

void CreateDefaultPlugs(PlugManager* manager, const char* movement_slot_id)
{
    if (!manager || !movement_slot_id) return;
    
    // Create KBD plug (Korean Back Dash)
    GameDirection kbd_pattern[] = {BACK, NEUTRAL, BACK, DOWN_BACK};
    AddPlug(manager, "KBD (Korean Back Dash)", movement_slot_id,
            "Korean Back Dash - Essential defensive movement technique",
            kbd_pattern, 4);
    
    // Create Wave Dash plug
    GameDirection wd_pattern[] = {FORWARD, NEUTRAL, DOWN, DOWN_FORWARD, FORWARD, NEUTRAL};
    AddPlug(manager, "WD (Wave Dash)", movement_slot_id,
            "Wave Dash - Advanced movement technique for quick approach",
            wd_pattern, 6);
}

bool SavePlugs(PlugManager* manager, const char* filename)
{
    if (!manager || !filename) return false;
    
    FILE* file = fopen(filename, "w");
    if (!file) return false;
    
    fprintf(file, "# KBD Trainer Plugs Configuration\n");
    fprintf(file, "plug_count=%d\n\n", manager->count);
    
    for (int i = 0; i < manager->count; i++) {
        Plug* plug = &manager->plugs[i];
        fprintf(file, "[plug_%d]\n", i);
        fprintf(file, "id=%s\n", plug->id);
        fprintf(file, "name=%s\n", plug->name);
        fprintf(file, "slot_id=%s\n", plug->slot_id);
        fprintf(file, "description=%s\n", plug->description);
        fprintf(file, "pattern_size=%d\n", plug->pattern_size);
        fprintf(file, "pattern=");
        for (int j = 0; j < plug->pattern_size; j++) {
            fprintf(file, "%d", (int)plug->pattern[j]);
            if (j < plug->pattern_size - 1) fprintf(file, ",");
        }
        fprintf(file, "\n\n");
    }
    
    fclose(file);
    return true;
}

bool LoadPlugs(PlugManager* manager, const char* filename)
{
    if (!manager || !filename) return false;
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        return true; // File doesn't exist, that's OK
    }
    
    char line[512];
    manager->count = 0;
    
    // Simple parser
    while (fgets(line, sizeof(line), file) && manager->count < MAX_PLUGS) {
        if (strncmp(line, "[plug_", 6) == 0) {
            Plug* plug = &manager->plugs[manager->count];
            
            // Read plug properties
            if (fgets(line, sizeof(line), file)) { // id
                sscanf(line, "id=%36s", plug->id);
            }
            if (fgets(line, sizeof(line), file)) { // name
                sscanf(line, "name=%63[^\n]", plug->name);
            }
            if (fgets(line, sizeof(line), file)) { // slot_id
                sscanf(line, "slot_id=%36s", plug->slot_id);
            }
            if (fgets(line, sizeof(line), file)) { // description
                sscanf(line, "description=%255[^\n]", plug->description);
            }
            if (fgets(line, sizeof(line), file)) { // pattern_size
                sscanf(line, "pattern_size=%d", &plug->pattern_size);
            }
            if (fgets(line, sizeof(line), file)) { // pattern
                char pattern_str[256];
                sscanf(line, "pattern=%255[^\n]", pattern_str);
                
                // Parse comma-separated pattern
                char* token = strtok(pattern_str, ",");
                int pattern_idx = 0;
                while (token && pattern_idx < plug->pattern_size && pattern_idx < MAX_PATTERN_SIZE) {
                    plug->pattern[pattern_idx] = (GameDirection)atoi(token);
                    token = strtok(NULL, ",");
                    pattern_idx++;
                }
            }
            
            manager->count++;
        }
    }
    
    fclose(file);
    return true;
}
