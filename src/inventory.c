#include "inventory.h"


void renderInventoryScreen(SDL_Renderer* renderer, const PlayerInventory* inventory) {
    // Set background for the inventory screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Example: Render equipment slots
    renderEquipmentSlot(renderer, inventory->helmet, 100, 100); // Position and item as example

    // Render inventory grid
    for (int i = 0; i < 100; i++) {
        if (inventory->inventoryItems[i] != NULL) {
            renderInventoryItem(renderer, inventory->inventoryItems[i], calculateItemPosition(i));
        }
    }

    // Render spell pouch and gems similarly
}

void renderInventoryItem(SDL_Renderer* renderer, const Item* item, SDL_Point position) {
    SDL_Rect itemRect = {position.x, position.y, 50, 50}; // Example item size
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for item rectangle
    SDL_RenderFillRect(renderer, &itemRect);

    // Assuming you have a function to render text
    // renderText(renderer, item->name, position.x, position.y + 55);
}


void renderEquipmentSlot(SDL_Renderer* renderer, const Item* item, int x, int y) {
    SDL_Rect slotRect = {x, y, 50, 50}; // Example size, adjust as needed
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for the slot
    SDL_RenderDrawRect(renderer, &slotRect);

    if (item != NULL) {
        // Render item icon within the slot
        // Placeholder: Use a colored rectangle or load an image
        SDL_SetRenderDrawColor(renderer, 100, 100, 250, 255); // Example item color
        SDL_RenderFillRect(renderer, &slotRect);
    }
}

SDL_Point calculateItemPosition(int index) {
    int itemsPerRow = 10; // Example: 10 items per row
    int itemSize = 50; // Matching the size used in `renderInventoryItem`
    int padding = 10; // Space between items

    SDL_Point position;
    position.x = (index % itemsPerRow) * (itemSize + padding) + 100; // Start at 100 for some margin
    position.y = (index / itemsPerRow) * (itemSize + padding) + 150; // Start at 150 for some margin
    return position;
}



