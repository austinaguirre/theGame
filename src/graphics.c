// src/graphics.c
#include "graphics.h"
#include "config.h"
#include <SDL_image.h>
#include "world.h"
#include "player.h"
#include "camera.h"

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

void player_render(Player* player, SDL_Renderer* renderer, const Camera* camera) {
    // Calculate the player's position relative to the camera's viewport
    SDL_Rect dstRect = {
        (player->x * CELL_WIDTH) - camera->x, // Adjust player's X position by camera offset
        (player->y * CELL_HEIGHT) - camera->y, // Adjust player's Y position by camera offset
        CELL_WIDTH,
        CELL_HEIGHT
    };
    SDL_RenderCopy(renderer, player->texture, NULL, &dstRect);
}


void world_render(const World* world, SDL_Renderer* renderer, const Camera* camera) {
    // Adjusted calculation for start and end tiles to include a buffer for partial tiles
    int startx = camera->x / TILE_SIZE - 1; // Add one tile buffer to start if not at the world's start
    int endx = (camera->x + camera->width) / TILE_SIZE + 1; // Add one tile buffer to end
    int starty = camera->y / TILE_SIZE - 1; // Same for Y
    int endy = (camera->y + camera->height) / TILE_SIZE + 1;

    // Ensure we don't try to render outside the world bounds
    startx = startx < 0 ? 0 : startx;
    starty = starty < 0 ? 0 : starty;
    endx = endx > world->width ? world->width : endx;
    endy = endy > world->height ? world->height : endy;

    for (int y = starty; y < endy; y++) {
        for (int x = startx; x < endx; x++) {
            // Ensure the loop only iterates over valid tile indices
            if (x >= 0 && y >= 0 && x < world->width && y < world->height) {
                SDL_Rect tile = {(x * TILE_SIZE) - camera->x, (y * TILE_SIZE) - camera->y, TILE_SIZE, TILE_SIZE};
                // Determine the color based on the tile type
                switch (world->map[y][x].type) {
                    case TILE_WALL:
                        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
                        break;
                    case TILE_WATER:
                        SDL_SetRenderDrawColor(renderer, 0, 105, 148, 255);
                        break;
                    case TILE_FOREST:
                        SDL_SetRenderDrawColor(renderer, 34, 139, 19, 255);
                        break;
                    case TILE_MOUNTAIN:
                        SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
                        break;
                    case TILE_DESERT:
                        SDL_SetRenderDrawColor(renderer, 210, 180, 140, 255);
                        break;
                    case TILE_GRASS:
                    default:
                        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
                        break;
                }
                SDL_RenderFillRect(renderer, &tile);
            }
        }
    }
}


