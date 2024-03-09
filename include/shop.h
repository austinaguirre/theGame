// shop.h
#ifndef SHOP_H
#define SHOP_H

#include <SDL.h>
#include "inventory.h"
#include "config.h"
#include "text.h"


typedef struct {
    Item* items; // Array of items in the shop
    int numItems; // Number of items in the shop
    char* name; // Name of the shop
    SDL_Rect area;
} Shop;

extern Shop* currentShop;

void initShop(Shop* shop, const char* name, Item* items, int numItems);
void renderShopScreen(SDL_Renderer* renderer, SDL_Point mousePos, const PlayerInventory* inventory);
void handleShopInput(SDL_Event* event, PlayerInventory* playerInventory);
void freeShop(Shop* shop);
void fillShopWithItems(Item* items, int numItems, int shopIndex, const char* cityName);
void renderInventoryScreenShop(SDL_Renderer* renderer, const PlayerInventory* inventory);
SDL_Point calculateShopItemPosition(int index);
int getShopItemUnderMouse(int mouseX, int mouseY);

#endif // SHOP_H

