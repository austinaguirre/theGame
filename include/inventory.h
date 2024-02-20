// include/input.h
#ifndef INVENTORY_H
#define INVENTORY_H

#include <SDL.h>
#include <stdbool.h>
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
    Item* spellPouch[10]; // Array for spells
    Item* inventoryItems[180]; // Array for general inventory items, adjust size as needed
    Item* draggedItem; // Item being dragged
    int draggedItemIndex; // Index of the item being dragged in the inventory array
    ItemType draggedItemType; // Type of the dragged item
    bool isDragging; // Flag to indicate if an item is currently being dragged
    SDL_Point draggedItemPos; // Position of the dragged item (relative to the mouse cursor)
} PlayerInventory;


void renderInventoryScreen(SDL_Renderer* renderer, const PlayerInventory* inventory);
void renderEquipmentSlot(SDL_Renderer* renderer, const Item* item, int x, int y);
SDL_Point calculateItemPosition(int index, int screenWidth, int screenHeight);
void renderInventoryItem(SDL_Renderer* renderer, const Item* item, SDL_Point position);
Item* createItem(ItemType type, const char* name);
SDL_Point calculateSpellPosition(int index, int windowWidth);
void renderSpell(SDL_Renderer* renderer, const Item* spell, SDL_Point position);



#endif
