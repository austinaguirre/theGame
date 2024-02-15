// src/world.c
#include "graphics.h"
#include "world.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "perlin_noise.h"


void world_init(World* world, int width, int height, unsigned int seed) {
    initPerlinNoise(seed); // Make sure this is implemented to initialize Perlin noise with the seed

    world->width = width;
    world->height = height;
    world->map = malloc(height * sizeof(Tile*));

    // Parameters for noise and island generation
    float biomeScale = 0.05; // Adjust for biome granularity
    float islandFactor = 1.25; // Controls "roundness" and size of the island

    // Calculate the center of the map
    float centerX = width / 2.0f;
    float centerY = height / 2.0f;
    float maxDistance = sqrt(centerX * centerX + centerY * centerY); // Max distance from center to corner

    for (int y = 0; y < height; y++) {
        world->map[y] = malloc(width * sizeof(Tile));

        for (int x = 0; x < width; x++) {
            // Calculate distance from the center of the map
            float distanceX = centerX - x;
            float distanceY = centerY - y;
            float distance = sqrt(distanceX * distanceX + distanceY * distanceY);

            // Normalize distance to range [0, 1]
            float normalizedDistance = distance / maxDistance;

            // Island shape and edge water
            float islandMask = 1.0f - pow(normalizedDistance * islandFactor, 1.5);

            // Generate biome using Perlin noise
            float biomeNoise = perlin(x * biomeScale, y * biomeScale);

            // Apply island mask to biome noise
            biomeNoise *= islandMask;

            // Assign tile types based on biome noise
            if (biomeNoise < 0.2) {
                world->map[y][x].type = TILE_WATER; // Deep ocean at the edges
            } else if (biomeNoise < 0.3) {
                world->map[y][x].type = TILE_SAND; // Beaches near the coast
            } else if (biomeNoise < 0.4) {
                world->map[y][x].type = TILE_GRASS; // Grasslands
            } else if (biomeNoise < 0.6) {
                world->map[y][x].type = TILE_FOREST; // Forests
            } else if (biomeNoise < 0.8) {
                world->map[y][x].type = TILE_MOUNTAIN; // Mountains
            } else {
                world->map[y][x].type = TILE_SNOW; // Snowy peaks at the highest elevations
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