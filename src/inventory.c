#include "inventory.h"
#include "config.h"

void renderDraggedItem(SDL_Renderer* renderer, PlayerInventory* inventory){
    if (inventory->isDragging && inventory->draggedItem != NULL) {
        SDL_Rect draggedItemRect = {inventory->draggedItemPos.x - 12, inventory->draggedItemPos.y - 12, 25, 25}; // Center the item on the cursor
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Example color
        SDL_RenderFillRect(renderer, &draggedItemRect);
        // Optionally, render the item's texture or a placeholder here
    }
}

void renderInventoryScreen(SDL_Renderer* renderer, const PlayerInventory* inventory) {
    // Set background for the inventory screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Example: Render equipment slots
    renderEquipmentSlot(renderer, inventory->helmet, 250, 50); // Position and item as example
    renderEquipmentSlot(renderer, inventory->chest, 250, 200);
    renderEquipmentSlot(renderer, inventory->leftArm, 100, 200);
    renderEquipmentSlot(renderer, inventory->rightArm, 400, 200);
    renderEquipmentSlot(renderer, inventory->leftLeg, 200, 350);
    renderEquipmentSlot(renderer, inventory->rightLeg, 300, 350);
    renderEquipmentSlot(renderer, inventory->weapon, 350, 75);
    renderEquipmentSlot(renderer, inventory->secondaryWeapon, 450, 75);

    //render inv
    for (int i = 0; i < 180; i++) {
        SDL_Point position = calculateItemPosition(i);
        renderInventoryItem(renderer, inventory->inventoryItems[i], position);
    }

    // render spells
    for (int i = 0; i < 10; i++){
        SDL_Point position = calculateSpellPosition(i, WINDOW_WIDTH);
        renderSpell(renderer, inventory->spellPouch[i], position);
    }
}

void pickItemColor(SDL_Renderer* renderer, const Item* item) {
    if (item != NULL) {
        switch (item->type) {
            case ITEM_TYPE_EQUIPMENT:
                // Color code for other types of equipment
                switch (item->equipment.equipmentType) {
                    case EQUIPMENT_TYPE_HELMET:
                        SDL_SetRenderDrawColor(renderer, 99, 84, 0, 255); // Brown for armor
                        break;
                    case EQUIPMENT_TYPE_CHEST:
                        SDL_SetRenderDrawColor(renderer, 99, 84, 0, 255); // Brown for armor
                        break;
                    case EQUIPMENT_TYPE_ARM:
                        SDL_SetRenderDrawColor(renderer, 99, 84, 0, 255); // Brown for armor
                        break;
                    case EQUIPMENT_TYPE_LEG:
                        SDL_SetRenderDrawColor(renderer, 99, 84, 0, 255); // Brown for armor
                        break;
                    case EQUIPMENT_TYPE_SHIELD:
                        SDL_SetRenderDrawColor(renderer, 0, 76, 153, 255); // Blue for shield
                        break;
                    default:
                        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Grey for unspecified equipment
                        break;
                }
                break;
            case ITEM_TYPE_WEAPON:
                // Handle weapon-specific coloring
                switch (item->weapon.weaponType) {
                    case WEAPON_TYPE_SWORD:
                        SDL_SetRenderDrawColor(renderer, 97, 97, 97, 255); // Red for swords
                        break;
                    case WEAPON_TYPE_BOW:
                        SDL_SetRenderDrawColor(renderer, 200, 100, 0, 255); // Different color for bows
                        break;
                    case WEAPON_TYPE_STAFF:
                        SDL_SetRenderDrawColor(renderer, 100, 0, 200, 255); // Different color for staffs
                        break;
                    default:
                        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Grey for unspecified weapons
                        break;
                }
            case ITEM_TYPE_SPELL:
                // Handle coloring based on spell type
                switch (item->spell.spellType) {
                    case SPELL_TYPE_DAMAGE:
                        SDL_SetRenderDrawColor(renderer, 100, 37, 37, 255); // Red for damage spells
                        break;
                    case SPELL_TYPE_HEAL:
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for healing spells
                        break;
                    case SPELL_TYPE_BUFF:
                        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue for buff spells
                        break;
                    case SPELL_TYPE_DEBUFF:
                        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Orange for debuff spells
                        break;
                    case SPELL_TYPE_NONE:
                        break;
                    default:
                        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Grey for unspecified spells
                        break;
                }
                break;
            case ITEM_TYPE_GEM:
                SDL_SetRenderDrawColor(renderer, 64, 247, 229, 255); // Cyan for gems
                break;
            default:
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Grey for unspecified items
                break;
        }
    }else{
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    }
}

void renderInventoryItem(SDL_Renderer* renderer, const Item* item, SDL_Point position) {
    SDL_Rect itemRect = {position.x, position.y, 25, 25}; // Smaller item size for compact view
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for item rectangle
    SDL_RenderDrawRect(renderer, &itemRect);

    // if (item != NULL) {
    //     logItemDetails(item);
    // }


    if (item != NULL) {
        pickItemColor(renderer, item);
        SDL_RenderFillRect(renderer, &itemRect);
    }
}



void renderEquipmentSlot(SDL_Renderer* renderer, const Item* item, int x, int y) {
    SDL_Rect slotRect = {x, y, 50, 50}; // Example size, adjust as needed
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for the slot
    SDL_RenderDrawRect(renderer, &slotRect);

    if (item != NULL) {
        // Render item icon within the slot
        // Placeholder: Use a colored rectangle or load an image
        pickItemColor(renderer, item);
        SDL_RenderFillRect(renderer, &slotRect);
    }
}

void renderSpell(SDL_Renderer* renderer, const Item* spell, SDL_Point position) {
    SDL_Rect spellRect = {position.x, position.y, 50, 50}; // Spell icon size

    // Set a distinct color for spell icons, e.g., blue
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for the slot
    SDL_RenderDrawRect(renderer, &spellRect);

    if(spell != NULL){
        pickItemColor(renderer, spell);
        SDL_RenderFillRect(renderer, &spellRect);
    }
    // If you have textures for spells, you would render the texture here instead.
    // For example: SDL_RenderCopy(renderer, spell->texture, NULL, &spellRect);
}

SDL_Point calculateItemPosition(int index) {
    int screenWidth = WINDOW_WIDTH;
    int itemsPerRow = 10; // Number of items per row in the grid
    int itemSize = 25; // Smaller item size for compact view
    int padding = 4; // Smaller padding between items
    int gridWidth = itemsPerRow * (itemSize + padding);
    int startX = screenWidth - gridWidth; // Right align the grid with some margin
    int startY = 50; // Start from top with some margin

    SDL_Point position;
    position.x = startX + (index % itemsPerRow) * (itemSize + padding);
    position.y = startY + (index / itemsPerRow) * (itemSize + padding);
    return position;
}

SDL_Point calculateSpellPosition(int index, int windowWidth) {
    SDL_Point position;
    int spellsPerRow = 5; // Spells per row
    int row = index / spellsPerRow; // Determine row based on index
    int indexInRow = index % spellsPerRow; // Index within the row

    int startX = 130;

    position.x = startX + indexInRow * (50 + 10); // Each spell is 50px wide, with 10px spacing
    position.y = 450 + row * (50 + 10); // Adjust y-coordinate based on row, assuming 50px height and 10px spacing between rows

    return position;
}

void logItemDetails(const Item* item) {
    printf("Rendering item: %s, Type: %s", item->name, itemTypeToString(item->type));

    if (item->type == ITEM_TYPE_EQUIPMENT) {
        printf(", EquipmentType: %s", equipmentTypeToString(item->equipment.equipmentType));
        printf(", Defense: %d", item->equipment.defense);
    } else if (item->type == ITEM_TYPE_SPELL) {
        printf(", SpellType: %s, Power: %d, Duration: %d", spellTypeToString(item->spell.spellType), item->spell.power, item->spell.duration);
    } else if (item->type == ITEM_TYPE_GEM) {
        printf(", Value: %d", item->value);
    } else if (item->type == ITEM_TYPE_WEAPON) {
        printf(", attack: %d", item->weapon.damage);
        printf(", weapon type: %s", weaponTypeToString(item->weapon.weaponType));
    }
    printf("\n");
}


const char* itemTypeToString(ItemType type) {
    switch (type) {
        case ITEM_TYPE_NONE: return "None";
        case ITEM_TYPE_EQUIPMENT: return "Equipment";
        case ITEM_TYPE_SPELL: return "Spell";
        case ITEM_TYPE_GEM: return "Gem";
        case ITEM_TYPE_WEAPON: return "Weapon";
        default: return "Unknown";
    }
}

const char* equipmentTypeToString(EquipmentType type) {
    switch (type) {
        case EQUIPMENT_TYPE_NONE: return "None";
        case EQUIPMENT_TYPE_HELMET: return "Helmet";
        case EQUIPMENT_TYPE_CHEST: return "Chest";
        case EQUIPMENT_TYPE_ARM: return "Arm";
        case EQUIPMENT_TYPE_LEG: return "Leg";
        case EQUIPMENT_TYPE_SHIELD: return "Shield";
        default: return "Unknown";
    }
}

const char* weaponTypeToString(WeaponType type) {
    switch (type) {
        case WEAPON_TYPE_NONE: return "None";
        case WEAPON_TYPE_SWORD: return "Sword";
        case WEAPON_TYPE_BOW: return "Bow";
        case WEAPON_TYPE_STAFF: return "Staff";
        default: return "Unknown";
    }
}

const char* spellTypeToString(SpellType type) {
    switch (type) {
        case SPELL_TYPE_NONE: return "None";
        case SPELL_TYPE_DAMAGE: return "Damage";
        case SPELL_TYPE_HEAL: return "Heal";
        case SPELL_TYPE_BUFF: return "Buff";
        case SPELL_TYPE_DEBUFF: return "Debuff";
        default: return "Unknown";
    }
}

// Function to create a general item (for gems or general purposes)
Item* createItem(ItemType type, const char* name, int value) {
    Item* newItem = malloc(sizeof(Item));
    newItem->type = type;
    newItem->name = strdup(name);
    newItem->value = value;
    printf("Creating Item: %s, Type: %s, Value: %d\n", name, itemTypeToString(type), value);
    return newItem;
}

// Function to create an equipment item
Item* createEquipmentItem(EquipmentType equipmentType, const char* name, int value, int defense) {
    Item* newItem = malloc(sizeof(Item));
    newItem->type = ITEM_TYPE_EQUIPMENT;
    newItem->name = strdup(name);
    newItem->value = value;
    newItem->equipment.equipmentType = equipmentType;
    newItem->equipment.defense = defense;
    newItem->weapon.weaponType = WEAPON_TYPE_NONE;
    newItem->spell.spellType = SPELL_TYPE_NONE;
    printf("Creating Equipment: %s, Equipment.equipmentType: %s, Type: %s, Defense: %d, Value: %d\n", 
            name, equipmentTypeToString(equipmentType),itemTypeToString(newItem->type), defense, value);
    return newItem;
}

// Function to create a weapon item
Item* createWeaponItem(WeaponType weaponType, const char* name, int value, int damage, int attackRate) {
    Item* newItem = malloc(sizeof(Item));
    newItem->type = ITEM_TYPE_WEAPON;
    newItem->name = strdup(name);
    newItem->value = value;
    newItem->weapon.weaponType = weaponType;
    newItem->weapon.damage = damage;
    newItem->weapon.attackRate = attackRate;
    newItem->type = ITEM_TYPE_WEAPON;
    newItem->spell.spellType = SPELL_TYPE_NONE;
    newItem->equipment.equipmentType = EQUIPMENT_TYPE_NONE;
    printf("Creating Weapon: %s, weaponType: %s, type: %s, Damage: %d, Attack Rate: %d, Value: %d\n",
                name, weaponTypeToString(weaponType), itemTypeToString(newItem->type), damage, attackRate, value);
    return newItem;
}

// Function to create a spell item
Item* createSpellItem(SpellType spellType, const char* name, int value, int power, int duration) {
    Item* newItem = malloc(sizeof(Item));
    newItem->type = ITEM_TYPE_SPELL;
    newItem->name = strdup(name);
    newItem->value = value;
    newItem->spell.spellType = spellType;
    newItem->spell.power = power;
    newItem->spell.duration = duration;
    newItem->equipment.equipmentType = EQUIPMENT_TYPE_NONE;
    newItem->weapon.weaponType = WEAPON_TYPE_NONE;
    printf("Creating Spell: %s, SpellType: %s, type: %s, Power: %d, Duration: %d, Value: %d\n",
           name, spellTypeToString(spellType), itemTypeToString(newItem->type), power, duration, value);
    return newItem;
}



