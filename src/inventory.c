#include "inventory.h"
#include "config.h"


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
    // i still need to do the gems below the equipemnt??????

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

void renderInventoryItem(SDL_Renderer* renderer, const Item* item, SDL_Point position) {
    SDL_Rect itemRect = {position.x, position.y, 25, 25}; // Smaller item size for compact view
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for item rectangle
    SDL_RenderDrawRect(renderer, &itemRect);

    if (item != NULL){
        if (item->type == ITEM_TYPE_EQUIPMENT){
            SDL_SetRenderDrawColor(renderer, 99, 84, 0, 255); 
        }else if (item->type == ITEM_TYPE_SPELL){
            SDL_SetRenderDrawColor(renderer, 122, 102, 223, 255);
        }else if (item->type == ITEM_TYPE_GEM){
            SDL_SetRenderDrawColor(renderer, 64, 247, 229, 255);
        }
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
        SDL_SetRenderDrawColor(renderer, 99, 84, 0, 255); // Example item color
        SDL_RenderFillRect(renderer, &slotRect);
    }
}

void renderSpell(SDL_Renderer* renderer, const Item* spell, SDL_Point position) {
    SDL_Rect spellRect = {position.x, position.y, 50, 50}; // Spell icon size

    // Set a distinct color for spell icons, e.g., blue
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for the slot
    SDL_RenderDrawRect(renderer, &spellRect);

    if(spell != NULL){
        SDL_SetRenderDrawColor(renderer, 122, 102, 223, 255); // Example item color
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


// Function to create a new item. For simplicity, it just assigns a type and a name.
Item* createItem(ItemType type, const char* name) {
    Item* newItem = malloc(sizeof(Item));
    newItem->type = type;
    newItem->name = strdup(name); // Make a copy of the name string
    // Initialize other item properties as needed
    return newItem;
}



