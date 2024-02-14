// src/graphics.c
#include "config.h"
#include "graphics.h"
#include <SDL_image.h>
#include "world.h"

void graphics_init(SDL_Window** window, SDL_Renderer** renderer) {
    SDL_Init(SDL_INIT_VIDEO);
    *window = SDL_CreateWindow("2D Adventure Game", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
}

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* filePath) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, filePath);
    if (!texture) {
        printf("Failed to load texture: %s\n", IMG_GetError());
    }
    return texture;
}

void render_texture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y) {
    SDL_Rect dest = {x, y, 50, 50}; // Modify as needed
    SDL_RenderCopy(renderer, texture, NULL, &dest);
}
