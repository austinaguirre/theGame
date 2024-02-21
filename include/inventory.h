// include/input.h
#ifndef INVENTORY_H
#define INVENTORY_H

#include <SDL.h>
#include <stdbool.h>
typedef enum {
    ITEM_TYPE_NONE = -1,
    ITEM_TYPE_EQUIPMENT,
    ITEM_TYPE_SPELL,
    ITEM_TYPE_GEM
} ItemType;

typedef enum {
    EQUIPMENT_TYPE_NONE = -1,
    EQUIPMENT_TYPE_HELMET,
    EQUIPMENT_TYPE_CHEST,
    EQUIPMENT_TYPE_ARM,
    EQUIPMENT_TYPE_LEG,
    EQUIPMENT_TYPE_WEAPON,
    EQUIPMENT_TYPE_SHIELD,
    // Add more specific equipment types as needed
} EquipmentType;

typedef enum {
    WEAPON_TYPE_NONE = -1,
    WEAPON_TYPE_SWORD,
    WEAPON_TYPE_BOW,
    WEAPON_TYPE_STAFF,
    // Add more weapon types as needed
} WeaponType;

typedef enum {
    SPELL_TYPE_NONE = -1,
    SPELL_TYPE_DAMAGE,
    SPELL_TYPE_HEAL,
    SPELL_TYPE_BUFF,
    SPELL_TYPE_DEBUFF,
    // Add more spell types as needed
} SpellType;

typedef struct {
    ItemType type;
    char* name; // Name of the item
    int value; // General value for the item, applicable to all types
    union {
        struct {
            EquipmentType equipmentType;
            int defense; // For armor items
        } equipment;

        struct {
            WeaponType weaponType;
            int damage; // For weapon items
            int attackRate; // For items that affect attack rate
        } weapon;

        struct {
            SpellType spellType;
            int power; // For spells, indicates the strength or healing amount
            int duration; // For spells with lasting effects
        } spell;
    };
} Item;

#define DRAGGED_FROM_INVENTORY 1
#define DRAGGED_FROM_EQUIPMENT 2
#define DRAGGED_FROM_SPELL_POUCH 3

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
    int draggedItemSource;
} PlayerInventory;


void renderInventoryScreen(SDL_Renderer* renderer, const PlayerInventory* inventory);
void renderEquipmentSlot(SDL_Renderer* renderer, const Item* item, int x, int y);
SDL_Point calculateItemPosition(int index);
void renderInventoryItem(SDL_Renderer* renderer, const Item* item, SDL_Point position);

Item* createItem(ItemType type, const char* name, int value);
Item* createEquipmentItem(EquipmentType equipmentType, const char* name, int value, int defense);
Item* createWeaponItem(WeaponType weaponType, const char* name, int value, int damage, int attackRate);
Item* createSpellItem(SpellType spellType, const char* name, int value, int power, int duration);

SDL_Point calculateSpellPosition(int index, int windowWidth);
void renderSpell(SDL_Renderer* renderer, const Item* spell, SDL_Point position);



#endif
