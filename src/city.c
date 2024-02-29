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



void handle_city_interaction(SDL_Event* event, City* city, GameState *currentGameState) {
    if (!city || event->type != SDL_MOUSEBUTTONDOWN) return;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    for (int i = 0; i < city->numShops; i++) {
        SDL_Rect shopArea = city->shops[i].area;
        if (mouseX >= shopArea.x && mouseX < (shopArea.x + shopArea.w) &&
            mouseY >= shopArea.y && mouseY < (shopArea.y + shopArea.h)) {
            currentShop = &city->shops[i]; // Shop was clicked, set as currentShop
            // Change game state or signal to render and interact with this shop
            *currentGameState = INSIDE_SHOP;
            break;
        }
    }
}

void initCity(City* city, const char* cityName) {
    city->name = strdup(cityName);
    city->numShops = 5;
    city->shops = malloc(sizeof(Shop) * city->numShops);
    if (!city->shops) return;

    // Sample items for shops - In a real scenario, these would be more dynamic
    Item sampleItems[5]; // Assuming you have a way to create sample items

    for (int i = 0; i < city->numShops; i++) {
        city->shops[i].name = malloc(sizeof(char) * 30);
        sprintf(city->shops[i].name, "shop %d in %s", i + 1, strdupToLower(city->name));

        // Assuming a function to fill sampleItems for each shop differently
        fillShopWithItems(sampleItems, 5, i, cityName); // Pseudo-function to initialize items

        // Use the new shop initialization function here
        initShop(&city->shops[i], city->shops[i].name, sampleItems, 5);

        // Define the clickable area for each shop in the city
        city->shops[i].area = (SDL_Rect){50 + i * 150, 100, 100, 100};
    }
}

void freeCity(City* city) {
    if (city == NULL) return;
    
    // Free each shop
    for (int i = 0; i < city->numShops; i++) {
        freeShop(&city->shops[i]);
    }
    
    // Free the shops array
    free(city->shops);
    
    // Free the city's name
    free(city->name);
    
    // Optionally, if City itself was dynamically allocated, free the city here
    // free(city);
}



