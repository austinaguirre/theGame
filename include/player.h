// include/player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include "inventory.h"

struct Camera; 
// struct World;

// Player structure
typedef struct {
    int x, y; // Player position
    SDL_Texture* texture; // Player texture
    PlayerInventory inventory;
} Player;

// Function declarations
void player_init(Player* player, SDL_Renderer* renderer);
void player_init_inventory(Player* player);
void equipItemToSlot(Player* player, Item* draggedItem, int slotIndex);
int getEquipmentSlotUnderMouse(int mouseX, int mouseY);
int getSpellSlotUnderMouse(int mouseX, int mouseY);
void addSpellToSlot(Player* player, Item* draggedItem, int slotIndex);
int getInventorySlotUnderMouse(int mouseX, int mouseY);
void unequipItemToInventory(Player* player, int equipmentSlotIndex, int inventorySlotIndex);
void removeSpellFromPouchToInventory(Player* player, int spellSlotIndex, int inventorySlotIndex);


#endif
