// src/world.c
#include "world.h"
#include "graphics.h" // For rendering functions
#include <stdbool.h>


void world_init(World* world) {
    // Initialize your world map here, potentially loading from a file
    // For simplicity, we'll manually define a map with '1' as walls and '0' as walkable spaces
    for (int y = 0; y < 15; y++) {
        for (int x = 0; x < 20; x++) {
            world->map[y][x] = (x == 0 || y == 0 || x == 24 || y == 17) ? 1 : 0; // Simple boundary walls
        }
    }
}

void world_render(const World* world, SDL_Renderer* renderer) {
    for (int y = 0; y < 15; y++) {
        for (int x = 0; x < 20; x++) {
            SDL_Rect tile = {x * 40, y * 40, 40, 40}; // Tiles are 40x40 pixels
            if (world->map[y][x] == 1) {
                // Render a wall
                SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Brown color for walls
            } else {
                // Render a walkable tile
                SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); // Forest green for grass
            }
            SDL_RenderFillRect(renderer, &tile);
        }
    }
}

bool world_can_move_to(const World* world, int x, int y) {
    // Check bounds
    if (x < 0 || x >= 20 || y < 0 || y >= 15) return false;
    // Check if the tile is walkable
    return world->map[y][x] == 0;
}
