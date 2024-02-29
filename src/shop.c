//shop.c
#include "shop.h"

Shop* currentShop = NULL;

void initShop(Shop* shop, const char* name, Item* items, int numItems) {
    shop->name = strdup(name);
    shop->numItems = numItems;
    shop->items = malloc(sizeof(Item) * numItems);
    if (!shop->items) return;

    for (int i = 0; i < numItems; i++) {
        shop->items[i] = items[i]; // Assume deep copy if necessary
    }
}

void fillShopWithItems(Item* items, int numItems, int shopIndex, const char* cityName) {
    char itemName[50];
    for (int i = 0; i < numItems; i++) {
        switch (shopIndex) {
            case 0: // Weapons
                if (strcmp(cityName, "Yangse") == 0) {
                    // Myke might have a variety of weapon types
                    sprintf(itemName, "yangse weapon %d-%d", shopIndex, i);
                    items[i] = *createWeaponItem(i % 3, itemName, i * 25 + 10, i * 2 + 5, i % 3 + 1); // Cycle through weapon types
                } else if (strcmp(cityName, "Ramondull") == 0) {
                    // Myke might have a variety of weapon types
                    sprintf(itemName, "ramondull weapon %d-%d", shopIndex, i);
                    items[i] = *createWeaponItem(i % 3, itemName, i * 25 + 10, i * 2 + 5, i % 3 + 1); // Cycle through weapon types
                } else if (strcmp(cityName, "Myke") == 0) {
                    // Myke might have a variety of weapon types
                    sprintf(itemName, "myke weapon %d-%d", shopIndex, i);
                    items[i] = *createWeaponItem(i % 3, itemName, i * 25 + 10, i * 2 + 5, i % 3 + 1); // Cycle through weapon types
                } else if (strcmp(cityName, "Dorport") == 0) {
                    // Myke might have a variety of weapon types
                    sprintf(itemName, "dorport weapon %d-%d", shopIndex, i);
                    items[i] = *createWeaponItem(i % 3, itemName, i * 25 + 10, i * 2 + 5, i % 3 + 1); // Cycle through weapon types
                } else {

                }
                break;
            case 1: // Armor and Shields
                if (strcmp(cityName, "Yangse") == 0) {
                    // Dorport might specialize in defensive equipment
                    sprintf(itemName, "yangse armour %d-%d", shopIndex, i);
                    items[i] = *createEquipmentItem(EQUIPMENT_TYPE_ARM, itemName, i * 30 + 15, i * 4 + 10); // Armour with increasing defense
                } else if (strcmp(cityName, "Ramondull") == 0) {
                    // Dorport might specialize in defensive equipment
                    sprintf(itemName, "ramondull armour %d-%d", shopIndex, i);
                    items[i] = *createEquipmentItem(EQUIPMENT_TYPE_ARM, itemName, i * 30 + 15, i * 4 + 10); // Armour with increasing defense
                } else if (strcmp(cityName, "Myke") == 0) {
                    // Dorport might specialize in defensive equipment
                    sprintf(itemName, "myke armour %d-%d", shopIndex, i);
                    items[i] = *createEquipmentItem(EQUIPMENT_TYPE_ARM, itemName, i * 30 + 15, i * 4 + 10); // Armour with increasing defense
                } else if (strcmp(cityName, "Dorport") == 0) {
                    // Dorport might specialize in defensive equipment
                    sprintf(itemName, "dorport armour %d-%d", shopIndex, i);
                    items[i] = *createEquipmentItem(EQUIPMENT_TYPE_ARM, itemName, i * 30 + 15, i * 4 + 10); // Armour with increasing defense
                } else {

                }
                break;
            case 2: // Spells
                if (strcmp(cityName, "Yangse") == 0) {
                    // Ramondull might favor magical items
                    sprintf(itemName, "yangse magic %d-%d", shopIndex, i);
                    items[i] = *createSpellItem(i % 2 ? SPELL_TYPE_DAMAGE : SPELL_TYPE_HEAL, itemName, i * 20 + 5, i * 3 + 10, i * 2 + 5); // Alternating spell types
                } else if (strcmp(cityName, "Ramondull") == 0) {
                    // Ramondull might favor magical items
                    sprintf(itemName, "ramondull magic %d-%d", shopIndex, i);
                    items[i] = *createSpellItem(i % 2 ? SPELL_TYPE_DAMAGE : SPELL_TYPE_HEAL, itemName, i * 20 + 5, i * 3 + 10, i * 2 + 5); // Alternating spell types
                } else if (strcmp(cityName, "Myke") == 0) {
                    // Ramondull might favor magical items
                    sprintf(itemName, "myke magic %d-%d", shopIndex, i);
                    items[i] = *createSpellItem(i % 2 ? SPELL_TYPE_DAMAGE : SPELL_TYPE_HEAL, itemName, i * 20 + 5, i * 3 + 10, i * 2 + 5); // Alternating spell types
                } else if (strcmp(cityName, "Dorport") == 0) {
                    // Ramondull might favor magical items
                    sprintf(itemName, "dorport magic %d-%d", shopIndex, i);
                    items[i] = *createSpellItem(i % 2 ? SPELL_TYPE_DAMAGE : SPELL_TYPE_HEAL, itemName, i * 20 + 5, i * 3 + 10, i * 2 + 5); // Alternating spell types
                } else {

                }
                break;
            case 3: // Other (general merchandise)
                sprintf(itemName, "%s misc %d-%d", strdupToLower(cityName), shopIndex, i);
                items[i] = *createItem(ITEM_TYPE_GEM, itemName, rand() % 100 + 50);
            case 4: // Other (more specialized or rare items)
                sprintf(itemName, "%s misc %d-%d", strdupToLower(cityName), shopIndex, i);
                items[i] = *createItem(ITEM_TYPE_GEM, itemName, rand() % 100 + 50);
                break;
        }
    }
}

void renderShopScreen(SDL_Renderer* renderer, SDL_Point mousePos) {
    if (!currentShop) return;

    // Set background for the shop screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Render shop name
    render_text(renderer, currentShop->name, 20, 20, textColor, textScale * 0.3);

    int startX = 20; // Starting X position for the grid
    int startY = 60; // Starting Y position for the grid
    int gap = 10; // Gap between items
    int size = 50; // Size of the square representing an item
    int itemsPerRow = 5; // Number of items per row
    bool itemHovered = false;

    for (int i = 0; i < currentShop->numItems; i++) {
        int row = i / itemsPerRow;
        int col = i % itemsPerRow;
        SDL_Rect rect = {startX + col * (size + gap), startY + row * (size + gap), size, size};

        // Use the item's type to pick the color
        pickItemColor(renderer, &currentShop->items[i]);
        SDL_RenderFillRect(renderer, &rect);

        if (SDL_PointInRect(&mousePos, &rect)) {
            itemHovered = true;

            int detailStartY = mousePos.y + 50; // Start 50 pixels below the mouse Y position
            int lineHeight = 20; // Height of each line

            // Background for item details
            // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
            // SDL_Rect backgroundRect = {mousePos.x + 5, mousePos.y, 500, 200}; // Adjust size as needed
            // SDL_RenderFillRect(renderer, &backgroundRect);

            // Render item details
            char itemName[256], itemType[256], itemValue[256];
            sprintf(itemName, "name: %s", strdupToLower(currentShop->items[i].name));
            sprintf(itemType, "type: %s", strdupToLower(itemTypeToString(currentShop->items[i].type)));
            sprintf(itemValue, "value: $%d", currentShop->items[i].value);

            render_text(renderer, itemName, mousePos.x + 3, detailStartY, textColor, textScale * 0.28);
            render_text(renderer, itemType, mousePos.x + 3, detailStartY + lineHeight, textColor, textScale * 0.28);
            render_text(renderer, itemValue, mousePos.x + 3, detailStartY + 2 * lineHeight, textColor, textScale * 0.28);
        }
    }

    if (!itemHovered) {
        // Optionally, show a default message or clear the previous detail display
    }
}




void handleShopInput(SDL_Event* event, PlayerInventory* playerInventory) {
    if (!currentShop || !event) return;

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Assuming each item is rendered in a 30px height block starting from Y=50
        int clickedItemIndex = (mouseY - 50) / 30;
        if (clickedItemIndex >= 0 && clickedItemIndex < currentShop->numItems) {
            // Add the clicked item to the player's inventory if there's space
            addItemToPlayerInventory(playerInventory, &currentShop->items[clickedItemIndex]);
        }
    }
}


void freeShop(Shop* shop) {
    if (shop == NULL) return;
    
    // Free each item's name and then the item itself if needed
    for (int j = 0; j < shop->numItems; j++) {
        // Assuming each item's name was dynamically allocated
        free(shop->items[j].name);
        // If the Item structure contains other dynamically allocated fields, free them here
    }
    
    // Free the items array
    free(shop->items);
    
    // Free the shop's name
    free(shop->name);
}



