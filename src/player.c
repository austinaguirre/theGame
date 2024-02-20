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
    // Initialize equipment slots with NULL or placeholder items
    player->inventory.helmet = createItem(ITEM_TYPE_EQUIPMENT, "Helmet");
    player->inventory.chest = createItem(ITEM_TYPE_EQUIPMENT, "Chest");
    player->inventory.leftArm = NULL;
    player->inventory.rightArm = createItem(ITEM_TYPE_EQUIPMENT, "Right Arm");
    player->inventory.leftLeg = createItem(ITEM_TYPE_EQUIPMENT, "Left Leg");
    player->inventory.rightLeg = NULL;
    player->inventory.weapon = createItem(ITEM_TYPE_EQUIPMENT, "Weapon");
    player->inventory.secondaryWeapon = NULL;

    // Initialize half the spell pouch with placeholder spells and the other half as NULL
    for (int i = 0; i < 10; i++) {
        if (i < 6) { // First half filled
            char spellName[30];
            sprintf(spellName, "Spell %d", i + 1); // Create a unique name for each spell
            player->inventory.spellPouch[i] = createItem(ITEM_TYPE_SPELL, spellName);
        } else { // Second half set to NULL
            player->inventory.spellPouch[i] = NULL;
        }
    }

    // Initialize half the inventory items with a mix of items, the other half as NULL
    for (int i = 0; i < 180; i++) {
        if (i < 100) { // First half filled
            if (i <= 40) {
                char itemName[30];
                sprintf(itemName, "spell %d", i / 5 + 1);
                player->inventory.inventoryItems[i] = createItem(ITEM_TYPE_SPELL, itemName);
            } else if (i <= 60){
                char itemName[30];
                sprintf(itemName, "gem %d", i);
                player->inventory.inventoryItems[i] = createItem(ITEM_TYPE_GEM, itemName);
            }else {
                char itemName[30];
                sprintf(itemName, "Equipment %d", i);
                player->inventory.inventoryItems[i] = createItem(ITEM_TYPE_EQUIPMENT, itemName);
            }
        } else { // Second half set to NULL
            player->inventory.inventoryItems[i] = NULL;
        }
    }
}

void equipItemToSlot(Player* player, Item* draggedItem, int slotIndex) {
    // Assuming the order of equipment slots in PlayerInventory matches equipmentSlotPositions
    if (draggedItem->type == ITEM_TYPE_EQUIPMENT) {
        // Find the appropriate slot for the item based on slotIndex
        switch (slotIndex) {
            case 0: // Helmet slot
                player->inventory.helmet = draggedItem;
                break;
            case 1: // Chest slot
                player->inventory.chest = draggedItem;
                break;
            case 2: // Left Arm slot
                player->inventory.leftArm = draggedItem;
                break;
            case 3: // Right Arm slot
                player->inventory.rightArm = draggedItem;
                break;
            case 4: // Left Leg slot
                player->inventory.leftLeg = draggedItem;
                break;
            case 5: // Right Leg slot
                player->inventory.rightLeg = draggedItem;
                break;
            case 6: // Weapon slot
                player->inventory.weapon = draggedItem;
                break;
            case 7: // Secondary Weapon slot
                player->inventory.secondaryWeapon = draggedItem;
                break;
            default:
                // This case should not occur if slotIndex is valid
                break;
        }
        // After equipping, remove the item from the draggedItemIndex in inventory
        // Note: Ensure draggedItemIndex is correctly set when beginning the drag operation
        if (player->inventory.draggedItemIndex >= 0) {
            player->inventory.inventoryItems[player->inventory.draggedItemIndex] = NULL;
        }
    } else if (draggedItem->type == ITEM_TYPE_SPELL) {
        // Assuming spells can only be equipped to the spell pouch
        for (int i = 0; i < 10; i++) {
            if (player->inventory.spellPouch[i] == NULL) {
                player->inventory.spellPouch[i] = draggedItem;
                player->inventory.inventoryItems[player->inventory.draggedItemIndex] = NULL; // Remove from inventory
                break;
            }
        }
    }
    // Reset dragged item info
    player->inventory.draggedItem = NULL;
    player->inventory.draggedItemIndex = -1;
}

void addSpellToSlot(Player* player, Item* draggedItem, int slotIndex) {
    // Ensure the item is a spell before trying to add it to a spell slot
    if (draggedItem->type == ITEM_TYPE_SPELL) {
        // Check if the slotIndex is within the bounds of the spell pouch
        if (slotIndex >= 0 && slotIndex < 10) {
            // Place the spell in the specified slot, if that slot is empty
            if (player->inventory.spellPouch[slotIndex] == NULL) {
                player->inventory.spellPouch[slotIndex] = draggedItem;
                // Remove the spell from its original position in the inventory
                player->inventory.inventoryItems[player->inventory.draggedItemIndex] = NULL;
            }
        }
    }
    // Reset dragged item info
    player->inventory.draggedItem = NULL;
    player->inventory.draggedItemIndex = -1;
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

// This function checks if the mouse is over any equipment slot and returns the slot index or -1 if none.
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

// This function checks if the mouse is over any spell slot and returns the slot index or -1 if none.
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


