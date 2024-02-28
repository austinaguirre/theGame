// include/camera.h
#ifndef CITY_H
#define CITY_H

#include <SDL.h>
#include "config.h"
#include "inventory.h"

typedef struct {
    char* name;
    Item* inventory; // Pointer to an array of items
    int numItems; // Number of items in the shop
    SDL_Rect area; // Rectangle defining the clickable area for the shop
} Shop;

typedef struct {
    Shop* shops; // Pointer to an array of shops in the city
    int numShops; // Number of shops in the city
    char* name;
} City;

void renderCityInteraction(SDL_Renderer* renderer, City* city);
void handle_city_interaction(SDL_Event* event, City* city);
void initCity(City* city, const char* cityName);
void freeCity(City* city);

#endif
