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

            // Calculate a latitude factor to favor mountains at the north and south
            float latitudeFactor = fabs(y - centerY) / centerY; // Ranges from 0 at the center to 1 at the edges
            
            // Adjust Perlin noise value based on distance to center and latitude factor
            float noiseValue = perlin(x * scale, y * scale) * (1.0 - normalizedDistance) * (1.0 + latitudeFactor * 0.5);

            // Threshold adjustments for more pronounced mountain regions in the north and south
            float veryLowThreshold = 0.1;
            float lowThreshold = 0.2;
            float mediumThreshold = 0.5 + latitudeFactor * 0.15; // Increase threshold for mountains with latitude
            float highThreshold = 0.7 + latitudeFactor * 0.1; // Slightly increase for very high regions
            
            // Map the adjusted noise value to a terrain height category
            if (noiseValue < veryLowThreshold) {
                world->map[y][x].height = TITLE_VERY_LOW; // Deep oceans at the edge
            } else if (noiseValue < lowThreshold) {
                world->map[y][x].height = TITLE_LOW; // Shallow waters, beaches
            } else if (noiseValue < mediumThreshold) {
                world->map[y][x].height = TITLE_MEDIUM; // Mainland, forests, grasslands
            } else if (noiseValue < highThreshold) {
                world->map[y][x].height = TITLE_HIGH; // Mountains and highlands
            } else {
                world->map[y][x].height = TITLE_VERY_HIGH; // Peaks, possibly snowy or icy regions
            }
        }
    }
}

void applyBiomesBasedOnHeightAndPosition(World* world) {
    // Constants for noise scale
    float biomeScale = 0.1;
    float moistureScale = 0.1; // Optional for more complexity

    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            // Compute latitude and longitude factors (0 at center, increasing towards edges)
            float latFactor = fabs(y - world->height / 2.0) / (world->height / 2.0);
            float longFactor = fabs(x - world->width / 2.0) / (world->width / 2.0);

            // Determine biome based on height and additional noise layers
            float biomeNoise = perlin(x * biomeScale, y * biomeScale);
            // Optional: float moistureNoise = perlin(x * moistureScale, y * moistureScale);

            if (world->map[y][x].height == TITLE_MEDIUM) {
                assignMediumBiome(&world->map[y][x], biomeNoise, latFactor, longFactor);
            } else if (world->map[y][x].height == TITLE_HIGH || world->map[y][x].height == TITLE_VERY_HIGH) {
                assignHighBiome(&world->map[y][x], biomeNoise, latFactor);
            } else if (world->map[y][x].height == TITLE_LOW) {
                assignLowBiome(&world->map[y][x], biomeNoise);
            } else if(world->map[y][x].height == TITLE_VERY_LOW){
                world->map[y][x].waterBody = TITLE_DEEP_OCEAN; // Very low regions become deep ocean
            }
        }
    }

    createBeaches(world);
    detectAndSetLakes(world);
    spawnRivers(world);
}

void spawnRivers(World* world) {
    int numberOfRivers = 5;

    for (int i = 0; i < numberOfRivers; i++) {
        // Select a random starting point for the river
        int startX, startY;
        do {
            startX = rand() % world->width;
            startY = rand() % world->height;
        } while (!isValidRiverSource(world, startX, startY));

        // Generate the river path
        generateRiverPath(world, startX, startY);
    }
}

bool isValidRiverSource(World* world, int x, int y) {
    // Check if the tile is suitable for being a river source
    return world->map[y][x].height == TITLE_MEDIUM &&
           world->map[y][x].waterBody == TITLE_DEFAULT_WATER &&
           world->map[y][x].structure == TITLE_DEFAULT_STRUCTURE;
}

void generateRiverPath(World* world, int startX, int startY) {
    int currentX = startX, currentY = startY;
    bool continueRiver = true;
    int retries; // Number of retries for finding a new direction

    while (continueRiver) {
        world->map[currentY][currentX].waterBody = TITLE_RIVER; // Mark the tile as part of a river

        bool directionFound = false;
        retries = 4; // Try each direction once before giving up

        while (!directionFound && retries > 0) {
            float noise = perlin(currentX * 0.1, currentY * 0.1); // Adjust scale as needed
            int direction = (int)(noise * 8) % 4; // Convert noise to a direction (0-3)

            int nextX = currentX, nextY = currentY;

            // Determine potential next position based on direction
            switch (direction) {
                case 0: nextY--; break; // Up
                case 1: nextX++; break; // Right
                case 2: nextY++; break; // Down
                case 3: nextX--; break; // Left
            }

            // Check if the next position is a valid continuation for the river
            if (canContinueRiver(world, nextX, nextY)) {
                currentX = nextX;
                currentY = nextY;
                directionFound = true;
            } else {
                retries--;
                noise += 0.1; // Adjust noise to try a different direction next time
            }
        }

        if (!directionFound) {
            // No valid direction found after retries, stop the river
            continueRiver = false;
        }
    }
}

bool canContinueRiver(World* world, int x, int y) {
    // Check bounds
    if (x < 0 || x >= world->width || y < 0 || y >= world->height) {
        return false; // Stop if out of bounds
    }

    // Stop if the tile is not suitable for the river to continue
    return world->map[y][x].height == TITLE_MEDIUM &&
           world->map[y][x].waterBody == TITLE_DEFAULT_WATER &&
           world->map[y][x].structure == TITLE_DEFAULT_STRUCTURE;
}




bool isPotentialBeachTile(World* world, int x, int y) {
    // Only consider medium tiles for potential beach conversion
    if (world->map[y][x].height != TITLE_MEDIUM) {
        return false;
    }

    // Check all adjacent tiles for water
    for (int offsetY = -1; offsetY <= 1; offsetY++) {
        for (int offsetX = -1; offsetX <= 1; offsetX++) {
            // Skip the center tile
            if (offsetX == 0 && offsetY == 0) continue;

            int adjacentX = x + offsetX;
            int adjacentY = y + offsetY;

            // Ensure we're not checking outside the map boundaries
            if (adjacentX >= 0 && adjacentX < world->width && adjacentY >= 0 && adjacentY < world->height) {
                // Check if the adjacent tile is water
                if (world->map[adjacentY][adjacentX].height == TITLE_VERY_LOW || world->map[adjacentY][adjacentX].height == TITLE_LOW) {
                    return true; // This tile is a potential beach
                }
            }
        }
    }

    // Not adjacent to water or not the right height
    return false;
}


void createBeaches(World* world) {
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            if (isPotentialBeachTile(world, x, y)) {
                world->map[y][x].terrain = TITLE_BEACH; // Convert to beach
            }
        }
    }
}



void detectAndSetLakes(World* world) {
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            // Check for potential lake tile
            if (isPotentialLakeTile(world, x, y)) {
                // Perform a flood fill to determine if it's enclosed by beach tiles
                if (isEnclosedByBeach(world, x, y)) {
                    setLakeTiles(world, x, y);
                }
            }
        }
    }
}

bool isPotentialLakeTile(World* world, int x, int y) {
    Tile tile = world->map[y][x];
    return tile.height == TITLE_LOW && tile.terrain == TITLE_DEFAULT_TERRAIN && tile.waterBody == TITLE_OCEAN;
}

bool checkEnclosure(World* world, int x, int y, bool* visited) {
    int index = y * world->width + x; // Calculate the index in the single-dimension array

    // Check bounds
    if (x < 0 || x >= world->width || y < 0 || y >= world->height) {
        return false; // Touching the edge means it's not enclosed
    }

    // If already visited or not a potential lake tile, skip
    if (visited[index] || !isPotentialLakeTile(world, x, y)) {
        return true; // Assume true to continue checking other tiles
    }

    visited[index] = true; // Mark as visited

    // Check surrounding tiles to verify enclosure by beach tiles
    // Directly adjacent tiles (4-way check)
    bool up = checkEnclosure(world, x, y - 1, visited);
    bool down = checkEnclosure(world, x, y + 1, visited);
    bool left = checkEnclosure(world, x - 1, y, visited);
    bool right = checkEnclosure(world, x + 1, y, visited);

    return up && down && left && right;
}

bool isEnclosedByBeach(World* world, int x, int y) {
    // Allocate a flat visited array
    bool* visited = calloc(world->height * world->width, sizeof(bool));

    bool result = checkEnclosure(world, x, y, visited);

    free(visited); // Clean up the dynamically allocated memory

    return result;
}

void initializeVisited(bool*** visited, int width, int height) {
    *visited = malloc(height * sizeof(bool*));
    for (int i = 0; i < height; i++) {
        (*visited)[i] = calloc(width, sizeof(bool)); // Use calloc for automatic initialization to false
    }
}

void cleanupVisited(bool*** visited, int height) {
    for (int i = 0; i < height; i++) {
        free((*visited)[i]);
    }
    free(*visited);
}


void floodFillSetLake(World* world, int x, int y, bool** visited) {
    // Check bounds and whether the tile has already been visited or is not a potential lake tile
    if (x < 0 || x >= world->width || y < 0 || y >= world->height || visited[y][x] || world->map[y][x].waterBody != TITLE_OCEAN) {
        return;
    }

    // Set current tile to lake
    world->map[y][x].waterBody = TITLE_LAKE;
    visited[y][x] = true;

    // Recursively fill adjacent tiles
    floodFillSetLake(world, x + 1, y, visited);
    floodFillSetLake(world, x - 1, y, visited);
    floodFillSetLake(world, x, y + 1, visited);
    floodFillSetLake(world, x, y - 1, visited);
}

void setLakeTiles(World* world, int x, int y) {
    // Initialize a visited array
    bool** visited = malloc(world->height * sizeof(bool*));
    for (int i = 0; i < world->height; i++) {
        visited[i] = malloc(world->width * sizeof(bool));
        for (int j = 0; j < world->width; j++) {
            visited[i][j] = false;
        }
    }

    // Start the flood fill from the specified point
    floodFillSetLake(world, x, y, visited);

    // Cleanup
    for (int i = 0; i < world->height; i++) {
        free(visited[i]);
    }
    free(visited);
}





void assignMediumBiome(Tile* tile, float biomeNoise, float latFactor, float longFactor) {
    // Example biome assignment based on noise and geographic factors
    if (biomeNoise < 0.3) {
        tile->terrain = (latFactor < 0.5) ? TITLE_FOREST : TITLE_SAVANNAH;
    } else if (biomeNoise < 0.6) {
        tile->terrain = (longFactor < 0.5) ? TITLE_GRASSLANDS : TITLE_JUNGLE;
    } else {
        tile->terrain = TITLE_DESERT;
    }
}

void assignHighBiome(Tile* tile, float biomeNoise, float latFactor) {
    // Assign high altitude biomes, possibly influenced by latitude
    tile->terrain = (latFactor < 0.5) ? TITLE_MOUNTAIN : TITLE_TUNDRA;
}

void assignLowBiome(Tile* tile, float biomeNoise) {
    // Since very low regions and beaches are handled elsewhere, 
    // this function will focus on assigning ocean to the remaining low regions.
    // Future logic can be added here for more detailed biome assignments like rivers and lakes.

    // Adjust biomeNoise thresholds if necessary to accommodate your game's biome distribution.
    if       (biomeNoise < 0.7) {
        tile->waterBody = TITLE_OCEAN; // Assign ocean to low regions
    } else if(biomeNoise < 0.9) {
        tile->terrain = TITLE_SWAMPLAND;
    } else{
        // Placeholder for future river or lake assignment logic
        // For now, we'll mark these tiles with a default terrain to indicate they need further processing.
        // This placeholder will be replaced with actual logic for rivers, lakes, or other specific biomes.
        tile->terrain = TITLE_DEFAULT_TERRAIN; // Indicate these tiles need further biome assignment
    }

    // Note: The logic for rivers, lakes, and specific terrain types like swamps
    // will be implemented in separate functions or steps in your world generation process.
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
            if (tile->terrain == TITLE_BEACH && tile->height == TITLE_MEDIUM) {
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