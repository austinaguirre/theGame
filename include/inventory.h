// include/input.h
#ifndef INVENTORY_H
#define INVENTORY_H

#include <SDL.h>

typedef enum {
    ITEM_TYPE_EQUIPMENT,
    ITEM_TYPE_SPELL,
    ITEM_TYPE_GEM
} ItemType;

typedef struct {
    ItemType type;
    char* name; // Name of the item
    // Add more properties specific to items, such as stats, effects, etc.
} Item;

typedef struct {
    Item* helmet;
    Item* chest;
    Item* leftArm;
    Item* rightArm;
    Item* leftLeg;
    Item* rightLeg;
    Item* weapon;
    Item* secondaryWeapon; // or shield
    Item* gems[10]; // Array for gems, assuming fixed size for simplicity
    Item* spellPouch[10]; // Array for spells
    Item* inventoryItems[100]; // Array for general inventory items, adjust size as needed
} PlayerInventory;


void renderInventoryScreen(SDL_Renderer* renderer, const PlayerInventory* inventory);
void renderEquipmentSlot(SDL_Renderer* renderer, const Item* item, int x, int y);
SDL_Point calculateItemPosition(int index);
void renderInventoryItem(SDL_Renderer* renderer, const Item* item, SDL_Point position);

#endif
