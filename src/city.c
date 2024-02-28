//city.c
#include "city.h"


void renderCityInteraction(SDL_Renderer* renderer, City* city) {
    if (!city) return; // Ensure city is valid

    // Render city street background
    SDL_Rect backgroundRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255); // Dark gray for streets
    SDL_RenderFillRect(renderer, &backgroundRect);

    for (int i = 0; i < city->numShops; i++) {
        SDL_Rect shopArea = city->shops[i].area;
        SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255); // Lighter gray for shops
        SDL_RenderFillRect(renderer, &shopArea);

        // Here add your text rendering for shop names if applicable
    }
}



void handle_city_interaction(SDL_Event* event, City* city) {
    if (!city || event->type != SDL_MOUSEBUTTONDOWN) return;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    for (int i = 0; i < city->numShops; i++) {
        SDL_Rect shopArea = city->shops[i].area;
        if (mouseX >= shopArea.x && mouseX < (shopArea.x + shopArea.w) &&
            mouseY >= shopArea.y && mouseY < (shopArea.y + shopArea.h)) {
            printf("Shop %d clicked\n", i);
            // Add functionality for opening the shop here
            break;
        }
    }
}

void initCity(City* city, const char* cityName) {
    city->name = strdup(cityName); // Set city name

    city->numShops = 3;
    city->shops = malloc(sizeof(Shop) * city->numShops);
    if (!city->shops) return;

    for (int i = 0; i < city->numShops; i++) {
        city->shops[i].name = malloc(sizeof(char) * 30);
        sprintf(city->shops[i].name, "Shop %d in %s", i + 1, city->name);

        int numItems = 5;
        city->shops[i].inventory = malloc(sizeof(Item) * numItems);
        city->shops[i].numItems = numItems;
        city->shops[i].area = (SDL_Rect){100 + i * 150, 100, 100, 100};

        for (int j = 0; j < numItems; j++) {
            city->shops[i].inventory[j].name = malloc(sizeof(char) * 50);
            sprintf(city->shops[i].inventory[j].name, "Item %d-%d", i + 1, j + 1);
            city->shops[i].inventory[j].value = (j + 1) * 10;
        }
    }
}

void freeCity(City* city) {
    for (int i = 0; i < city->numShops; i++) {
        for (int j = 0; j < city->shops[i].numItems; j++) {
            free(city->shops[i].inventory[j].name); // Free each item's name
        }
        free(city->shops[i].inventory); // Free the inventory array
        free(city->shops[i].name); // Free the shop's name
    }
    free(city->shops); // Finally, free the shops array
}



