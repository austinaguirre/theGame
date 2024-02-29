// include/city.h
#ifndef CITY_H
#define CITY_H

#include <SDL.h>
#include "config.h"
#include "inventory.h"
#include "shop.h"
#include "game.h"

typedef struct {
    Shop* shops; // Pointer to an array of shops in the city
    int numShops; // Number of shops in the city
    char* name;
} City;

void renderCityInteraction(SDL_Renderer* renderer, City* city);
void handle_city_interaction(SDL_Event* event, City* city, GameState *currentGameState);
void initCity(City* city, const char* cityName);
void freeCity(City* city);

#endif
