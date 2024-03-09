// src/player.c
#include "graphics.h"
#include "config.h"
// #include "world.h"
#include "player.h"
#include <SDL_image.h>
#include "inventory.h"

void player_init(Player* player, SDL_Renderer* renderer) {
    player->x = 100; // Initial position
    player->y = 100;
    player_init_inventory(player);
    // Use the load_texture function from graphics.c
    player->texture = load_texture(renderer, "assets/images/player.png");
    if (!player->texture) {
        printf("Failed to load player texture\n");
    }
}

void player_init_inventory(Player* player) { 
    // Set player class
    player->characterClass = CLASS_KNIGHT;

    // Set starting level and experience
    player->level = 4;
    player->experience = 10;

    // Initialize stats
    player->stats.melee.strength = 10;
    player->stats.melee.attackSpeed = 10;
    player->stats.range.accuracy = 2;
    player->stats.range.reloadSpeed = 2;
    player->stats.defence.health = 120;
    player->stats.defence.healthRecovery = 12;
    player->stats.defence.magicResistance = 5;
    player->stats.defence.meleeResistance = 6;
    player->stats.magic.spellPower = 0;
    player->stats.magic.mana = 10;
    player->stats.magic.manaRecovery = 1;
    player->stats.agility.dodgeChance = 10;
    player->stats.agility.movementSpeed = 10;

    // Initialize equipment slots with NULL or placeholder items
    player->inventory.helmet = createEquipmentItem(EQUIPMENT_TYPE_HELMET, "Helmet", 100, 5);
    player->inventory.chest = createEquipmentItem(EQUIPMENT_TYPE_CHEST, "Chest", 150, 10);
    player->inventory.leftArm = NULL;
    player->inventory.rightArm = NULL;
    player->inventory.leftLeg = NULL;
    player->inventory.rightLeg = NULL;
    player->inventory.weapon = createWeaponItem(WEAPON_TYPE_SWORD, "Sword", 200, 15, 1);
    player->inventory.secondaryWeapon = NULL;
    player->inventory.coins = 100;

    // Initialize the spell pouch with placeholder spells and the other half as NULL
    for (int i = 0; i < 10; i++) {
        if (i < 5) { // First half filled with spells
            char spellName[30];
            sprintf(spellName, "Spell %d", i + 1);
            player->inventory.spellPouch[i] = createSpellItem(SPELL_TYPE_DAMAGE, spellName, 50, 10, 5);
        } else { // Second half set to NULL
            player->inventory.spellPouch[i] = NULL;
        }
    }

    // Initialize inventory items with a mix of items, the other half as NULL
    for (int i = 0; i < 180; i++) {
        if (i <= 90) { // First half filled
            char itemName[30];
            if (i <= 10) {
                sprintf(itemName, "Gem %d", i);
                player->inventory.inventoryItems[i] = createItem(ITEM_TYPE_GEM, itemName, 25);
            } else if (i <= 20){
                sprintf(itemName, "Equipment %d", i);
                player->inventory.inventoryItems[i] = createEquipmentItem(EQUIPMENT_TYPE_ARM, itemName, 50, 2);
            }else if (i <= 30){
                sprintf(itemName, "Spell %d", i + 1);
                player->inventory.inventoryItems[i] = createSpellItem(SPELL_TYPE_DAMAGE, itemName, 50, 10, 5);
            }else if (i <= 40){
                sprintf(itemName, "Equipment %d", i);
                player->inventory.inventoryItems[i] = createEquipmentItem(EQUIPMENT_TYPE_ARM, itemName, 50, 2);
            }else if (i <= 50){
                sprintf(itemName, "staff %d", i);
                player->inventory.inventoryItems[i] = createWeaponItem(WEAPON_TYPE_STAFF, itemName, 50, 2, 1);
            }else if (i <= 60){
                sprintf(itemName, "shield %d", i);
                player->inventory.inventoryItems[i] = createEquipmentItem(EQUIPMENT_TYPE_SHIELD, itemName, 50, 2);
            }else if (i <= 70){
                sprintf(itemName, "sword %d", i);
                player->inventory.inventoryItems[i] = createWeaponItem(WEAPON_TYPE_SWORD, itemName, 50, 3, 3);
            }else if (i <= 80){
                sprintf(itemName, "spell %d", i);
                player->inventory.inventoryItems[i] = createSpellItem(SPELL_TYPE_DEBUFF, itemName, 50, 10, 5);
            }else{
                sprintf(itemName, "spell %d", i);
                player->inventory.inventoryItems[i] = createSpellItem(SPELL_TYPE_HEAL, itemName, 50, 10, 5);
            }
        } else { // Second half set to NULL
            player->inventory.inventoryItems[i] = NULL;
        }
    }
}

void equipItemToSlot(Player* player, Item* draggedItem, int slotIndex) {
    // Assuming the order of equipment slots in PlayerInventory matches equipmentSlotPositions
    Item **targetSlot = NULL;

    // Determine the target equipment slot based on slotIndex
    switch (slotIndex) {
        case 0: targetSlot = &player->inventory.helmet; break;
        case 1: targetSlot = &player->inventory.chest; break;
        case 2: targetSlot = &player->inventory.leftArm; break;
        case 3: targetSlot = &player->inventory.rightArm; break;
        case 4: targetSlot = &player->inventory.leftLeg; break;
        case 5: targetSlot = &player->inventory.rightLeg; break;
        case 6: targetSlot = &player->inventory.weapon; break;
        case 7: targetSlot = &player->inventory.secondaryWeapon; break;
        default:
            // Handle invalid slotIndex if necessary
            return;
    }

    if (!isWeaponTypeValidForSlot(slotIndex, draggedItem->weapon.weaponType, draggedItem->equipment.equipmentType)) {
        // Reset dragged item info and exit if not valid
        player->inventory.draggedItem = NULL;
        player->inventory.draggedItemIndex = -1;
        return;
    }

    // Perform the swap if the slot is already occupied
    if (*targetSlot != NULL) {
        // Swap the items between the equipment slot and the inventory slot
        Item* temp = *targetSlot;
        *targetSlot = draggedItem; // Equip the dragged item

        // Place the previously equipped item back into the inventory slot from which the dragged item came
        player->inventory.inventoryItems[player->inventory.draggedItemIndex] = temp;
    } else {
        // If the slot was not occupied, just equip the item directly
        *targetSlot = draggedItem;

        // Since the item was equipped successfully, clear the inventory slot
        player->inventory.inventoryItems[player->inventory.draggedItemIndex] = NULL;
    }

    // Reset dragged item info
    player->inventory.draggedItem = NULL;
    player->inventory.draggedItemIndex = -1;
}

bool isWeaponTypeValidForSlot(int slotIndex, WeaponType weaponType, EquipmentType equipmentType) {

    //something about levels here

    // Check for weapon slots
    if (slotIndex == 6 || slotIndex == 7) {
        return weaponType == WEAPON_TYPE_BOW ||
               weaponType == WEAPON_TYPE_STAFF ||
               weaponType == WEAPON_TYPE_SWORD ||
               equipmentType == EQUIPMENT_TYPE_SHIELD;
    }
    // Check for armor slots
    else if (slotIndex <= 5) {
        return equipmentType == EQUIPMENT_TYPE_ARM ||
               equipmentType == EQUIPMENT_TYPE_CHEST ||
               equipmentType == EQUIPMENT_TYPE_HELMET ||
               equipmentType == EQUIPMENT_TYPE_LEG;
    }
    // Default case if slotIndex is not recognized
    return false;
}

void addSpellToSlot(Player* player, Item* draggedItem, int slotIndex) {
    // Ensure the item is a spell before trying to add it to a spell slot
    if (draggedItem->type == ITEM_TYPE_SPELL) {
        // Check if the slotIndex is within the bounds of the spell pouch
        if (slotIndex >= 0 && slotIndex < 10) {
            // If the targeted spell slot is already occupied, swap the spells
            if (player->inventory.spellPouch[slotIndex] != NULL) {
                // Store the spell that's currently in the targeted slot
                Item* tempSpell = player->inventory.spellPouch[slotIndex];
                
                // Place the dragged spell in the targeted slot
                player->inventory.spellPouch[slotIndex] = draggedItem;
                
                // Move the previously slotted spell back to the inventory slot from which the dragged spell came
                player->inventory.inventoryItems[player->inventory.draggedItemIndex] = tempSpell;
            } else {
                // If the targeted slot is empty, just place the dragged spell in the slot
                player->inventory.spellPouch[slotIndex] = draggedItem;
                
                // Since the spell was successfully placed in the spell pouch, clear the originating inventory slot
                player->inventory.inventoryItems[player->inventory.draggedItemIndex] = NULL;
            }
        }
    }
}


void unequipItemToInventory(Player* player, int equipmentSlotIndex, int inventorySlotIndex) {
    Item* itemToMove = NULL;

    // Determine which equipment slot is being unequipped based on equipmentSlotIndex
    switch (equipmentSlotIndex) {
        case 0: itemToMove = player->inventory.helmet; player->inventory.helmet = NULL; break;
        case 1: itemToMove = player->inventory.chest; player->inventory.chest = NULL; break;
        case 2: itemToMove = player->inventory.leftArm; player->inventory.leftArm = NULL; break;
        case 3: itemToMove = player->inventory.rightArm; player->inventory.rightArm = NULL; break;
        case 4: itemToMove = player->inventory.leftLeg; player->inventory.leftLeg = NULL; break;
        case 5: itemToMove = player->inventory.rightLeg; player->inventory.rightLeg = NULL; break;
        case 6: itemToMove = player->inventory.weapon; player->inventory.weapon = NULL; break;
        case 7: itemToMove = player->inventory.secondaryWeapon; player->inventory.secondaryWeapon = NULL; break;
    }

    // Check if the target inventory slot is occupied
    if (player->inventory.inventoryItems[inventorySlotIndex] != NULL) {
        // Here you could swap items, or handle according to your game logic
        // For simplicity, we're just cancelling the move if the slot is occupied
        return;
    } else {
        // Place the unequipped item into the target inventory slot
        player->inventory.inventoryItems[inventorySlotIndex] = itemToMove;
    }
}

void removeSpellFromPouchToInventory(Player* player, int spellSlotIndex, int inventorySlotIndex) {
    // Check if the target inventory slot is occupied
    if (player->inventory.inventoryItems[inventorySlotIndex] != NULL) {
        // Handle according to your game logic, such as canceling the move
        return;
    } else {
        // Move the spell from the spell pouch to the target inventory slot
        player->inventory.inventoryItems[inventorySlotIndex] = player->inventory.spellPouch[spellSlotIndex];
        player->inventory.spellPouch[spellSlotIndex] = NULL; // Clear the spell slot
    }
}


// Define the positions of equipment slots relative to the window/screen.
// These positions should match those used in renderEquipmentSlot.
const SDL_Rect equipmentSlotPositions[] = {
    {250, 50, 50, 50},   // Helmet
    {250, 200, 50, 50},  // Chest
    {100, 200, 50, 50},  // Left Arm
    {400, 200, 50, 50},  // Right Arm
    {200, 350, 50, 50},  // Left Leg
    {300, 350, 50, 50},  // Right Leg
    {350, 75, 50, 50},   // Weapon
    {450, 75, 50, 50}    // Secondary Weapon
};

int getEquipmentSlotUnderMouse(int mouseX, int mouseY) {
    for (int i = 0; i < sizeof(equipmentSlotPositions) / sizeof(equipmentSlotPositions[0]); i++) {
        const SDL_Rect* slotRect = &equipmentSlotPositions[i];
        if (mouseX >= slotRect->x && mouseX < slotRect->x + slotRect->w &&
            mouseY >= slotRect->y && mouseY < slotRect->y + slotRect->h) {
            return i; // Return the index of the slot under the mouse
        }
    }
    return -1; // No equipment slot under the mouse
}


// Define the positions of spell slots relative to the window/screen.
const SDL_Rect spellSlotPositions[] = {
    // Assuming spells are laid out horizontally starting at x=130, y=500 for example
    // Update these positions to match your actual spell slot positions
    {130, 450, 50, 50},  // Spell slot 1
    {190, 450, 50, 50},  // Spell slot 2
    {250, 450, 50, 50},  // Spell slot 3
    {310, 450, 50, 50},  // Spell slot 4
    {370, 450, 50, 50},  // Spell slot 5
    {130, 510, 50, 50},  // Spell slot 1
    {190, 510, 50, 50},  // Spell slot 2
    {250, 510, 50, 50},  // Spell slot 3
    {310, 510, 50, 50},  // Spell slot 4
    {370, 510, 50, 50},  // Spell slot 5
    // Add more slots if you have more than 5 spells in a row
};

int getSpellSlotUnderMouse(int mouseX, int mouseY) {
    for (int i = 0; i < sizeof(spellSlotPositions) / sizeof(spellSlotPositions[0]); i++) {
        const SDL_Rect* slotRect = &spellSlotPositions[i];
        if (mouseX >= slotRect->x && mouseX < slotRect->x + slotRect->w &&
            mouseY >= slotRect->y && mouseY < slotRect->y + slotRect->h) {
            return i; // Return the index of the slot under the mouse
        }
    }
    return -1; // No spell slot under the mouse
}

int getInventorySlotUnderMouse(int mouseX, int mouseY) {
    // Calculate the position and dimensions of the inventory grid

    for (int i = 0; i < 180; i++) { // Assuming a total of 180 inventory slots
        SDL_Point slotPosition = calculateItemPosition(i);
        SDL_Rect slotRect = {slotPosition.x, slotPosition.y, 25, 25};

        if (mouseX >= slotRect.x && mouseX < (slotRect.x + slotRect.w) &&
            mouseY >= slotRect.y && mouseY < (slotRect.y + slotRect.h)) {
            return i; // Return the index of the inventory slot under the mouse
        }
    }

    return -1; // No inventory slot under the mouse
}


