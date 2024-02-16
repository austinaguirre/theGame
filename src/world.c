// src/world.c
#include "graphics.h"
#include "world.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "perlin_noise.h"


void generateHeightMap(World* world, float scale, unsigned int seed) {
    initPerlinNoise(seed); // Initialize Perlin noise with the given seed

    // Calculate the center of the map
    float centerX = world->width / 2.0;
    float centerY = world->height / 2.0;
    float maxDistance = sqrt(centerX * centerX + centerY * centerY); // Max distance from center to corner

    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            // Calculate distance from the current point to the center of the map
            float distanceX = centerX - x;
            float distanceY = centerY - y;
            float distanceToCenter = sqrt(distanceX * distanceX + distanceY * distanceY);
            
            // Normalize distance to a value between 0 and 1
            float normalizedDistance = distanceToCenter / maxDistance;
            
            // Adjust Perlin noise value based on distance to center
            // Closer to the edge, the noise value is reduced to create ocean
            float noiseValue = perlin(x * scale, y * scale) * (1.0 - normalizedDistance);

            // Map the adjusted noise value to a terrain height category
            if (noiseValue < 0.05) {
                world->map[y][x].height = TITLE_VERY_LOW; // Deep oceans at the edge
            } else if (noiseValue < 0.15) {
                world->map[y][x].height = TITLE_LOW; // Shallow waters, beaches
            } else if (noiseValue < 0.4) {
                world->map[y][x].height = TITLE_MEDIUM; // Mainland, forests, grasslands
            } else if (noiseValue < 0.6) {
                world->map[y][x].height = TITLE_HIGH; // Mountains and highlands
            } else {
                world->map[y][x].height = TITLE_VERY_HIGH; // Peaks, possibly snowy or icy regions
            }
        }
    }
}

void applyBiomesBasedOnHeightAndPosition(World* world) {
    // Constants for biome placement logic
    int northThreshold = world->height / 3;
    int southThreshold = 2 * world->height / 3;

    // Flags to ensure unique biomes spawn only once
    bool swampSpawned = false;
    bool deadLandSpawned = false;
    bool spiderLandSpawned = false;
    bool scorchedLandSpawned = false;

    // Loop through each cell in the world map
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            // Handle water bodies separately
            if (world->map[y][x].height == TITLE_VERY_LOW || world->map[y][x].height == TITLE_LOW) {
                world->map[y][x].terrain = (world->map[y][x].height == TITLE_VERY_LOW) ? TITLE_DEFAULT_TERRAIN : TITLE_DEFAULT_TERRAIN;
                world->map[y][x].waterBody = (world->map[y][x].height == TITLE_VERY_LOW) ? TITLE_DEEP_OCEAN : TITLE_OCEAN;
                if (isAdjacentToLand(world, x, y)) { // Pseudo-function to check adjacency
                    world->map[y][x].terrain = TITLE_BEACH;
                }
                continue;
            }

            // Generate terrain based on position and height
            if (y < northThreshold) { // Northern part of the map
                if (!scorchedLandSpawned && world->map[y][x].height == TITLE_MEDIUM && rand() % 100 < 5) { // 5% chance
                    world->map[y][x].terrain = TITLE_SCORCHED_LANDS;
                    scorchedLandSpawned = true;
                } else if (world->map[y][x].height == TITLE_VERY_HIGH) {
                    world->map[y][x].terrain = TITLE_TUNDRA;
                }
            } else if (y > southThreshold) { // Southern part of the map
                if (!swampSpawned && world->map[y][x].height == TITLE_MEDIUM && rand() % 100 < 10) { // 10% chance
                    world->map[y][x].terrain = TITLE_SWAMPLAND;
                    swampSpawned = true;
                } else if (!spiderLandSpawned && world->map[y][x].height == TITLE_HIGH && rand() % 100 < 10) { // 10% chance
                    world->map[y][x].terrain = TITLE_SPIDERLANDS;
                    spiderLandSpawned = true;
                }
            }

            // General terrain logic for all areas
            switch (world->map[y][x].height) {
                case TITLE_MEDIUM:
                    world->map[y][x].terrain = determineMediumTerrain(world, x, y, southThreshold); // Pseudo-function to decide terrain
                    break;
                case TITLE_HIGH:
                    world->map[y][x].terrain = TITLE_MOUNTAIN;
                    break;
                default:
                    world->map[y][x].terrain = TITLE_GRASSLANDS; // Default terrain
                    break;
            }
        }
    }

    // Further logic to handle unique biomes and special features, such as ensuring only one Dead Land and placing jungles, savannah, etc.
    // This could include iterating again with specific conditions to place these unique biomes or adjusting existing terrain based on surrounding conditions.
}

// Determines the medium terrain type based on additional logic
TerrainType determineMediumTerrain(World* world, int x, int y, int southThreshold) {
    float noiseValue = perlin(x * 0.05, y * 0.05); // Use Perlin noise for variability

    // Adjust these thresholds to control biome distribution
    if (noiseValue < 0.3) {
        return TITLE_FOREST;
    } else if (noiseValue < 0.6) {
        if (y > southThreshold) {
            return TITLE_SAVANNAH; // More likely to occur in the southern part
        } else {
            return TITLE_GRASSLANDS;
        }
    } else {
        if (!isAdjacentToLand(world, x, y)) {
            return TITLE_JUNGLE; // Jungles not directly next to water
        } else {
            return TITLE_GRASSLANDS; // Fallback to grasslands near water
        }
    }
}

// Checks if a given water tile at (x, y) is adjacent to a land tile
bool isAdjacentToLand(World* world, int x, int y) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue; // Skip the center tile

            int checkX = x + i;
            int checkY = y + j;

            // Ensure we're not checking out of bounds
            if (checkX >= 0 && checkX < world->width && checkY >= 0 && checkY < world->height) {
                // Check if the adjacent tile is not water
                if (world->map[checkY][checkX].height > TITLE_LOW) {
                    return true; // Adjacent land found
                }
            }
        }
    }
    return false; // No adjacent land tiles found
}

void applySpecialFeatures(World* world) {
    // Flags to control the one-time features
    bool icyTundraPlaced = false;
    int snowyMountainCount = 0;
    int deepOceanTrenchCount = 0;

    // Iterate through the map to apply special features
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            Tile* tile = &world->map[y][x];

            // Example: Convert beaches on the north and south edges to special beaches
            if (tile->terrain == TITLE_BEACH) {
                if (y < 10) { // North edge for Bone Beach
                    tile->specialFeature = TITLE_BONE_BEACH;
                } else if (world->height - y <= 10) { // South edge for Tropical Beach
                    tile->specialFeature = TITLE_TROPICAL_BEACH;
                }
            }

            // Example: Apply special forest types based on location and existing forest terrain
            if (tile->terrain == TITLE_FOREST) {
                if (rand() % 100 < 10) { // 10% chance to convert a forest tile to a special forest
                    // Assuming functions to check north/south position
                    if (isInNorthernHalf(world, y)) {
                        tile->specialFeature = TITLE_REDWOOD_FOREST;
                    } else {
                        tile->specialFeature = TITLE_BLOSSOM_FOREST;
                    }
                }
            }

            // Place Snowy Mountains (only 2 allowed)
            if (snowyMountainCount < 2 && tile->terrain == TITLE_MOUNTAIN && tile->height == TITLE_VERY_HIGH) {
                tile->specialFeature = TITLE_SNOWY_MOUNTAIN;
                snowyMountainCount++;
            }

            // Deep Ocean Trench within Deep Ocean
            if (deepOceanTrenchCount < 3 && tile->waterBody == TITLE_DEEP_OCEAN) {
                // Additional logic to ensure it forms a line and it's rare
                if (rand() % 1000 < 5) { // Very rare
                    tile->specialFeature = TITLE_DEEP_OCEAN_TRENCH;
                    deepOceanTrenchCount++;
                }
            }

            // Other special features follow similar logic, with conditions based on their unique requirements
            // Some features may require scanning neighboring tiles or checking global conditions
        }
    }

    // Additional passes for features requiring more complex conditions, such as adjacency or count checks
}

// Example helper function to determine if a tile is in the northern half of the map
bool isInNorthernHalf(World* world, int y) {
    return y < world->height / 2;
}

void placeStructures(World* world) {
    // Seed for random placement
    srand(time(NULL));

    // Place each structure based on its specific conditions
    placeStructure(world, TITLE_SHIP_WRECK, TITLE_BEACH, "north");
    placeStructure(world, TITLE_FARMLAND, TITLE_GRASSLANDS, "center");
    placeStructure(world, TITLE_CAVE, TITLE_MOUNTAIN, "north");
    placeStructure(world, TITLE_ANCIENT_RUINS, TITLE_DESERT, "south");
    placeStructure(world, TITLE_DOCK, TITLE_BEACH, "east");
    placeStructure(world, TITLE_CITY_YANGSE, TITLE_GRASSLANDS, "northeast");
    placeStructure(world, TITLE_CITY_RAMONDULL, TITLE_GRASSLANDS, "northwest");
    placeStructure(world, TITLE_CITY_MYKE, TITLE_GRASSLANDS, "southwest");
    placeStructure(world, TITLE_CITY_DORPORT, TITLE_GRASSLANDS, "southeast");
    placeStructure(world, TITLE_ORC_CAMP, TITLE_GRASSLANDS, "north");
    placeStructure(world, TITLE_ORC_CASTLE, TITLE_TUNDRA, "north");
    placeStructure(world, TITLE_ELF_HIDEOUT, TITLE_JUNGLE, "any");
}

void placeStructure(World* world, StructureType structure, TerrainType preferredTerrain, const char* location) {
    bool placed = false;
    int attempts = 0;
    int maxAttempts = 1000; // Prevent infinite loops

    while (!placed && attempts < maxAttempts) {
        int x = rand() % world->width;
        int y = rand() % world->height;

        // Check if the selected location meets the criteria
        if (isValidLocationForStructure(world, x, y, preferredTerrain, location)) {
            world->map[y][x].structure = structure;
            placed = true;
        }

        attempts++;
    }
}

bool isValidLocationForStructure(World* world, int x, int y, TerrainType preferredTerrain, const char* location) {
    // Check if the tile's terrain matches the preferred terrain for the structure
    if (world->map[y][x].terrain != preferredTerrain) {
        return false; // Terrain type does not match
    }

    // Define thresholds for different locations on the map
    int northThreshold = world->height * 0.25; // Top 25% of the map
    int southThreshold = world->height * 0.75; // Bottom 25% of the map
    int eastThreshold = world->width * 0.75; // Right 25% of the map
    int westThreshold = world->width * 0.25; // Left 25% of the map
    int centerThresholdX = world->width / 2;
    int centerThresholdY = world->height / 2;

    // Location criteria
    if (strcmp(location, "north") == 0 && y > northThreshold) {
        return false; // Not in the northern part
    }
    if (strcmp(location, "south") == 0 && y < southThreshold) {
        return false; // Not in the southern part
    }
    if (strcmp(location, "east") == 0 && x < eastThreshold) {
        return false; // Not on the eastern side
    }
    if (strcmp(location, "west") == 0 && x > westThreshold) {
        return false; // Not on the western side
    }
    if (strcmp(location, "center") == 0 && (abs(x - centerThresholdX) > world->width * 0.2 || abs(y - centerThresholdY) > world->height * 0.2)) {
        return false; // Not in the center region
    }

    // If none of the conditions are met, the location is valid
    return true;
}



void world_init(World* world, int width, int height, unsigned int seed) {
    // Initialize Perlin noise with the seed
    initPerlinNoise(seed);

    world->width = width;
    world->height = height;
    world->map = malloc(height * sizeof(Tile*));

    for (int y = 0; y < height; y++) {
        world->map[y] = malloc(width * sizeof(Tile));
        for (int x = 0; x < width; x++) {
            // Initialize each tile
            world->map[y][x].terrain = TITLE_DEFAULT_TERRAIN; // Default to empty, will be set in the generation process
            world->map[y][x].waterBody = TITLE_DEFAULT_WATER; // Default to ocean, will be refined
            world->map[y][x].specialFeature = TITLE_DEFAULT_SPECIAL; // Default, will be updated
            world->map[y][x].structure = TITLE_DEFAULT_STRUCTURE; // Default, no structure
        }
    }

    // Step 1: Generate basic height map
    generateHeightMap(world, 0.05, seed); // Adjust scale as needed

    // Step 2: Apply biomes based on height and position
    applyBiomesBasedOnHeightAndPosition(world);

    // Step 3: Apply special features to the map
    applySpecialFeatures(world);

    // Step 4: Place structures across the map
    placeStructures(world);
}

bool world_can_move_to(const World* world, int x, int y) {
    // For now, make everything walkable as requested
    return true;
}

void world_free(World* world) {
    for (int y = 0; y < world->height; y++) {
        free(world->map[y]);
    }
    free(world->map);
}