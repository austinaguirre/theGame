// src/world.c
#include "graphics.h"
#include "world.h"
#include <stdbool.h>
#include <stdlib.h> // For rand()
#include <time.h>   // For time()

void world_init(World* world, int width, int height) {
    srand(time(NULL)); // Seed the random number generator for biome generation

    world->width = width;
    world->height = height;
    world->map = malloc(height * sizeof(Tile*));
    for (int y = 0; y < height; y++) {
        world->map[y] = malloc(width * sizeof(Tile));
        for (int x = 0; x < width; x++) {
            // Randomly assign biomes based on simple conditions
            int randValue = rand() % 100; // Generate a random value between 0 and 99
            if (randValue < 10) {
                world->map[y][x].type = TILE_WATER;
            } else if (randValue < 20) {
                world->map[y][x].type = TILE_FOREST;
            } else if (randValue < 30) {
                world->map[y][x].type = TILE_MOUNTAIN;
            } else if (randValue < 40) {
                world->map[y][x].type = TILE_DESERT;
            } else {
                world->map[y][x].type = TILE_GRASS;
            }
        }
    }
}




bool world_can_move_to(const World* world, int x, int y) {
    if (x < 0 || x >= world->width || y < 0 || y >= world->height) return false;

    TileType type = world->map[y][x].type;
    switch (type) {
        case TILE_GRASS:
        case TILE_EMPTY:
        case TILE_FOREST:
        case TILE_MOUNTAIN:
        case TILE_DESERT:
            return true; // Assume these tiles are walkable
        case TILE_WATER:
            // Future: Check if player has a boat
            return false; // Currently, assume water is not walkable without a boat
        case TILE_WALL:
        default:
            return false; // Walls are not walkable
    }
}


void world_free(World* world) {
    for (int y = 0; y < world->height; y++) {
        free(world->map[y]);
    }
    free(world->map);
}