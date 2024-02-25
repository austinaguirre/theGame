#include "input.h"
#include "player.h"
#include "world.h"
#include <SDL.h>
#include <stdbool.h>
#include "game.h"
#include "ui.h"
#include "config.h"
#include <stdio.h>

// Adjust these based on your game's window size and grid cell size
const int GRID_WIDTH = 20; // Assuming 25 grid cells horizontally
const int GRID_HEIGHT = 15; // Assuming 18 grid cells vertically

void handle_input(SDL_Event *event, bool *running, Player *player, World *world) {
    if (event->type == SDL_KEYDOWN) {
        int dx = 0, dy = 0;
        switch (event->key.keysym.sym) {
            case SDLK_UP:    dy = -1; break;
            case SDLK_DOWN:  dy = 1; break;
            case SDLK_LEFT:  dx = -1; break;
            case SDLK_RIGHT: dx = 1; break;
        }
        // Prepare the new potential position
        int newX = player->x + dx;
        int newY = player->y + dy;

        // Use world_can_move_to to check if the position is valid
        if (world_can_move_to(world, newX, newY)) {
            player->x = newX;
            player->y = newY;
        }
    }
}

void handle_gameplay_input(SDL_Event *event, bool *running, Player *player, World *world, GameState *currentGameState, bool *mapView){
    if (event->key.keysym.sym == SDLK_ESCAPE && event->type == SDL_KEYDOWN) {
        *currentGameState = PAUSE; // Toggle pause state with ESC
    } else if (event->key.keysym.sym == SDLK_m && event->type == SDL_KEYDOWN) {
        *mapView = !(*mapView); // Toggle map view
    } else if (event->key.keysym.sym == SDLK_RETURN && event->type == SDL_KEYDOWN) {
        // Handle interactions based on the current tile the player is standing on
        Tile currentTile = world->map[player->y][player->x];
        switch (currentTile.interactionType) {
            case INTERACTION_CITY:
                // Enter city interaction mode
                *currentGameState = CITY_INTERACTION;
                break;
            default:
                // Start an enemy encounter for all other types, except for special cases like water
                *currentGameState = ENEMY_ENCOUNTER;
                break;
        }
    } else {
        handle_input(event, running, player, world);
    }
}

bool checkButtonClick(SDL_Event* event, UIButton* button) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX > button->x && mouseX < button->x + button->width &&
            mouseY > button->y && mouseY < button->y + button->height) {
            return true;
        }
    }
    return false;
}


void handleInventoryClick(SDL_Event* event, Player* player) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // // Reset dragging state initially
        // player->inventory.isDragging = false;
        // Assume initially that no item is being dragged
        resetDraggingState(player);

        // First, check equipment slots
        int equipmentSlotIndex = getEquipmentSlotUnderMouse(mouseX, mouseY);
        if (equipmentSlotIndex != -1) {
            // Logic to start dragging an item from an equipment slot
            // You will need a way to access the specific item based on equipmentSlotIndex

            // Item* item = getItemFromEquipmentSlot(player, equipmentSlotIndex);

            Item* item = NULL;
            if (equipmentSlotIndex == 0){
                item = player->inventory.helmet;
            }else if (equipmentSlotIndex == 1){
                item = player->inventory.chest;
            }else if (equipmentSlotIndex == 2){
                item = player->inventory.leftArm;
            }else if (equipmentSlotIndex == 3){
                item = player->inventory.rightArm;
            }else if (equipmentSlotIndex == 4){
                item = player->inventory.leftLeg;
            }else if (equipmentSlotIndex == 5){
                item = player->inventory.rightLeg;
            }else if (equipmentSlotIndex == 6){
                item = player->inventory.weapon;
            }else if (equipmentSlotIndex == 7){
                item = player->inventory.secondaryWeapon;
            }

            if (item != NULL) {
                startDraggingItem(player, item, equipmentSlotIndex, ITEM_TYPE_EQUIPMENT, DRAGGED_FROM_EQUIPMENT);
                return; // Exit after setting up dragging for equipment
            }
        }

        // Then, check spell slots
        int spellSlotIndex = getSpellSlotUnderMouse(mouseX, mouseY);
        if (spellSlotIndex != -1) {
            // Logic to start dragging an item from a spell slot

            // Item* spell = getSpellFromSlot(player, spellSlotIndex);

            Item* spell = player->inventory.spellPouch[spellSlotIndex];
            if (spell != NULL) {
                startDraggingItem(player, spell, spellSlotIndex, ITEM_TYPE_SPELL, DRAGGED_FROM_SPELL_POUCH);
                return; // Exit after setting up dragging for a spell
            }
        }

        // Finally, check inventory slots if no equipment/spell slot was clicked
        int inventorySlotIndex = getInventorySlotUnderMouse(mouseX, mouseY);
        if (inventorySlotIndex != -1) {
            // Existing logic to drag from inventory slots
            Item* item = player->inventory.inventoryItems[inventorySlotIndex];
            if (item != NULL) {
                startDraggingItem(player, item, inventorySlotIndex, item->type, DRAGGED_FROM_INVENTORY);
            }
        }
    }
}

void startDraggingItem(Player* player, Item* item, int slotIndex, ItemType type, int source) {
    player->inventory.isDragging = true;
    player->inventory.draggedItem = item;
    player->inventory.draggedItemIndex = slotIndex;
    player->inventory.draggedItemType = type;
    player->inventory.draggedItemSource = source;
}



void handleMouseMovement(SDL_Event* event, Player* player) {
    if (player->inventory.isDragging && event->type == SDL_MOUSEMOTION) {
        player->inventory.draggedItemPos.x = event->motion.x;
        player->inventory.draggedItemPos.y = event->motion.y;
    }
}

void handleItemDrop(SDL_Event* event, Player* player) {
    if (!player->inventory.isDragging || event->type != SDL_MOUSEBUTTONUP) return;
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    int equipmentSlotIndex = getEquipmentSlotUnderMouse(mouseX, mouseY);
    int spellSlotIndex = getSpellSlotUnderMouse(mouseX, mouseY);
    int inventorySlotIndex = getInventorySlotUnderMouse(mouseX, mouseY);

    // Use the draggedItemSource to determine the original location of the item
    switch (player->inventory.draggedItemSource) {
        case DRAGGED_FROM_INVENTORY:
            if (equipmentSlotIndex != -1 && (
                player->inventory.draggedItemType == ITEM_TYPE_EQUIPMENT ||
                player->inventory.draggedItemType == ITEM_TYPE_WEAPON)) {
                // Attempt to equip the item to the appropriate equipment slot
                equipItemToSlot(player, player->inventory.draggedItem, equipmentSlotIndex);
            } else if (spellSlotIndex != -1 && player->inventory.draggedItemType == ITEM_TYPE_SPELL) {
                // Attempt to add the spell to the appropriate spell slot
                addSpellToSlot(player, player->inventory.draggedItem, spellSlotIndex);
            } else if (inventorySlotIndex != -1 && inventorySlotIndex != player->inventory.draggedItemIndex) {
                // Move the item within the inventory, swapping slots if necessary
                Item* temp = player->inventory.inventoryItems[inventorySlotIndex];
                player->inventory.inventoryItems[inventorySlotIndex] = player->inventory.draggedItem;
                player->inventory.inventoryItems[player->inventory.draggedItemIndex] = temp;
            }
            break;

        case DRAGGED_FROM_EQUIPMENT:
            // Ensure we are moving to a valid inventory slot
            if (inventorySlotIndex != -1) {
                unequipItemToInventory(player, player->inventory.draggedItemIndex, inventorySlotIndex);
            }
            break;

        case DRAGGED_FROM_SPELL_POUCH:
            // Ensure we are moving to a valid inventory slot
            if (inventorySlotIndex != -1) {
                removeSpellFromPouchToInventory(player, player->inventory.draggedItemIndex, inventorySlotIndex);
            }
            break;
    }

    resetDraggingState(player);
}

void resetDraggingState(Player* player) {
    player->inventory.isDragging = false;
    player->inventory.draggedItem = NULL;
    player->inventory.draggedItemIndex = -1;
    player->inventory.draggedItemType = ITEM_TYPE_NONE;
    player->inventory.draggedItemSource = 0;
}

void handle_inventory_input(SDL_Event *event, Player *player){
    handleInventoryClick(event, player); 
    handleMouseMovement(event, player);
    handleItemDrop(event, player);
}



