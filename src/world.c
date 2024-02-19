// src/world.c
#include "graphics.h"
#include "world.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "perlin_noise.h"
#include "config.h"


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
                assignMediumBiome(&world->map[y][x], x, y, world->width, world->height);
            } else if (world->map[y][x].height == TITLE_HIGH || world->map[y][x].height == TITLE_VERY_HIGH) {
                assignHighBiome(&world->map[y][x], latFactor, x, y);
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


void assignMediumBiome(Tile* tile, int x, int y, int width, int height) {
    float biomeNoiseScale = 0.05; // Scale for biome type noise
    float transitionNoiseScale = 0.1; // Scale for the transition modulation noise

    // Calculate distance from the center
    float centerX = width / 2.0;
    float centerY = height / 2.0;
    float distX = x - centerX;
    float distY = y - centerY;
    float distanceFromCenter = sqrt(distX * distX + distY * distY);
    float maxDistanceFromCenter = sqrt(centerX * centerX + centerY * centerY);
    float normalizedDistance = distanceFromCenter / maxDistanceFromCenter;

    // Generate biome and transition noises
    float biomeNoise = perlin(x * biomeNoiseScale, y * biomeNoiseScale);
    float transitionNoise = perlin(x * transitionNoiseScale, y * transitionNoiseScale);

    // Apply transition noise to the distance factor for organic transitions
    normalizedDistance += (transitionNoise - 0.5) * 0.2;

    // Biome selection based on distance and noise
    if (normalizedDistance < 0.2) {
        // Central areas: mostly forests and grasslands
        tile->terrain = (biomeNoise < 0.3) ? TITLE_FOREST : TITLE_GRASSLANDS;
    } else if (normalizedDistance < 0.6) {
        // Intermediate areas: mix in savannahs and jungles
        if (biomeNoise < 0.2) {
            tile->terrain = TITLE_GRASSLANDS;
        } else if (biomeNoise < 0.6) {
            tile->terrain = TITLE_FOREST;
        } else if (biomeNoise < 0.7) {
            tile->terrain = TITLE_SAVANNAH;
        } else {
            tile->terrain = TITLE_JUNGLE;
        }
    } else {
        // Outer areas: introduce unique and harsher biomes
        if (biomeNoise < 0.5) {
            tile->terrain = TITLE_SPIDERLANDS;
        } else if (biomeNoise < 0.6) {
            tile->terrain = TITLE_DESERT;
        } else if (biomeNoise < 0.8) {
            tile->terrain = TITLE_SCORCHED_LANDS;
        } else if (biomeNoise < 0.9) {
            tile->terrain = TITLE_DEAD_LANDS;
        } else {
            tile->terrain = TITLE_JUNGLE;
        }
    }
}






void assignHighBiome(Tile* tile, float latFactor, int x, int y) {
    float primaryNoiseScale = 0.05; // Scale for the primary mountain biome noise
    float subBiomeNoiseScale = 0.1; // Scale for the sub-biome transition noise
    float transitionNoiseScale = 0.08; // Scale for the transition modulation noise
    
    // Generate primary Perlin noise for mountain biome
    float primaryNoise = perlin(x * primaryNoiseScale, y * primaryNoiseScale);
    
    // Generate secondary Perlin noise for sub-biome transitions
    float subBiomeNoise = perlin(x * subBiomeNoiseScale, y * subBiomeNoiseScale);

    // Generate transition modulation noise
    float transitionNoise = perlin(x * transitionNoiseScale, y * transitionNoiseScale);

    // Define noise threshold for mountain biome
    const float mountainThreshold = 0.4;

    // Define sub-biome transition thresholds
    const float subBiomeTransitionThreshold = 0.55;

    // Modulate the latitude factor with transition noise to create natural transitions
    float modulatedLatFactor = latFactor + (transitionNoise - 0.5) * 0.25; // Adjust modulation strength with * 0.25

    // Check if the tile qualifies as mountain biome based on primary noise
    if (primaryNoise < mountainThreshold) {
        tile->terrain = TITLE_MOUNTAIN;
    } else {
        // Further determine sub-biome based on secondary noise and modulated latitude
        if (subBiomeNoise > subBiomeTransitionThreshold) {
            if (modulatedLatFactor < 0.4) {
                // Northern half favors tundra sub-biome with natural transition
                tile->terrain = TITLE_TUNDRA;
            } else {
                // Southern half favors volcanic lands sub-biome with natural transition
                tile->terrain = TITLE_VOLCANIC_LANDS;
            }
        } else {
            // Default to mountain if not within the sub-biome transition area
            tile->terrain = TITLE_MOUNTAIN;
        }
    }
}




void assignLowBiome(Tile* tile, float biomeNoise) {
    // Since very low regions and beaches are handled elsewhere, 
    // this function will focus on assigning ocean to the remaining low regions.
    // Future logic can be added here for more detailed biome assignments like rivers and lakes.

    // Adjust biomeNoise thresholds if necessary to accommodate your game's biome distribution.
    if (biomeNoise < 0.7) {
        tile->waterBody = TITLE_OCEAN; // Assign ocean to low regions
    } else if(biomeNoise < 0.9) {
        tile->terrain = TITLE_SWAMPLAND;
    } else{
        tile->terrain = TITLE_DEFAULT_TERRAIN; // Indicate these tiles need further biome assignment
    }

    // Note: The logic for rivers, lakes, and specific terrain types like swamps
    // will be implemented in separate functions or steps in your world generation process.
}


void applySpecialFeatures(World* world) {
    // Helper functions to determine placement eligibility
    // These would encapsulate logic specific to each feature's requirements
    placeSpecialBeaches(world);
    placeSubBiomes(world);
    placeDeepOceanTrenches(world);
    placeCoralReefs(world);
    placeCrystalCaves(world);
    placeIcyTundraPeaks(world);
    placeSulfuricSprings(world);
    placeForestSubBiomes(world); // For Crystal, Bamboo, Ironwood, Fungi, Redwood, Blossom Forests
    placeCanyons(world);
    placeSmoothGrasslandsGreenHills(world);
}

void placeSpecialBeaches(World* world) {
    // Define the length range for special beach stretches
    int minStretchLength = 20;
    int maxStretchLength = 40;

    // Western special beach
    createSpecialBeachStretch(world, true, minStretchLength, maxStretchLength, TITLE_BONE_BEACH);

    // Eastern special beach
    createSpecialBeachStretch(world, false, minStretchLength, maxStretchLength, TITLE_TROPICAL_BEACH);
}

void createSpecialBeachStretch(World* world, bool isWest, int minLength, int maxLength, SpecialFeatureType specialBeachType) {
    int stretchLength = rand() % (maxLength - minLength + 1) + minLength; // Random stretch length within range
    int startX = isWest ? 0 : world->width - stretchLength; // Starting X based on west or east
    int foundBeaches = 0;

    // Iterate over a section of the map to convert a stretch of beach
    for (int x = startX; (isWest && x < startX + stretchLength) || (!isWest && x > startX - stretchLength); isWest ? x++ : x--) {
        for (int y = 0; y < world->height; y++) {
            if (world->map[y][x].terrain == TITLE_BEACH) {
                world->map[y][x].specialFeature = specialBeachType; // Mark as special beach
                foundBeaches++;
                if (foundBeaches >= stretchLength) {
                    return; // Exit once we've converted enough beach tiles
                }
            }
        }
    }
}


void placeSubBiomes(World* world) {
    // Adjust the scale of noise to control the granularity of sub-biome distribution
    float noiseScale = 0.05; // Smaller values for broader, more gradual changes; larger for finer, more abrupt changes

    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            Tile* tile = &world->map[y][x];
            float noiseValue = perlin(x * noiseScale, y * noiseScale);

            if (tile->terrain == TITLE_VOLCANIC_LANDS){
                if (noiseValue > 0.9){
                    tile->specialFeature = TITLE_ACTIVE_VOLCANIC_LANDS;
                }
            }

            if (tile->terrain == TITLE_MOUNTAIN && tile->height == TITLE_VERY_HIGH) {
                if (isInNorthernHalf(world, y) && noiseValue > 0.7) { // Only a fraction becomes Snowy Mountains
                    tile->specialFeature = TITLE_SNOWY_MOUNTAIN;
                }
            }

            // Introduce randomness in the placement of Paradise Isles
            if (tile->waterBody == TITLE_OCEAN && isIsolatedWaterTile(world, x, y) && noiseValue - 0.03 > 0.55) { // A smaller fraction becomes Paradise Isles
                tile->specialFeature = TITLE_PARADISE_ISLES;
            }

            // Implement similar logic for other sub-biomes with noise to control their distribution
        }
    }
}


bool isIsolatedWaterTile(World* world, int x, int y) {
    if (!(world->map[y][x].waterBody == TITLE_OCEAN || world->map[y][x].waterBody == TITLE_DEEP_OCEAN)) {
        return false; // The tile is not a water tile, so it cannot be isolated
    }

    int adjacentNonWaterCount = 0;
    // Examine a 3x3 grid centered on the tile
    for (int offsetY = -5; offsetY <= 5; offsetY++) {
        for (int offsetX = -5; offsetX <= 5; offsetX++) {
            // Skip the center tile (the water tile itself)
            if (offsetX == 0 && offsetY == 0) continue;

            int adjacentX = x + offsetX;
            int adjacentY = y + offsetY;

            // Check map boundaries
            if (adjacentX < 0 || adjacentX >= world->width || adjacentY < 0 || adjacentY >= world->height) {
                continue; // Skip checking outside the map bounds
            }

            // Count adjacent tiles that are not water
            if (world->map[adjacentY][adjacentX].waterBody != TITLE_OCEAN && world->map[adjacentY][adjacentX].waterBody != TITLE_DEEP_OCEAN) {
                adjacentNonWaterCount++;
            }
        }
    }

    // Define "isolated" based on the number of adjacent non-water tiles. Adjust the threshold as needed.
    return adjacentNonWaterCount < 1;
}

void placeDeepOceanTrenches(World* world) {
    // Parameters for trench placement
    int maxTrenches = 5; // Maximum number of trenches
    int trenchLength = 5; // Length of each trench
    int attempts = 100; // Attempts to place a trench

    int trenchCount = 0;

    while (trenchCount < maxTrenches && attempts > 0) {
        int startX = rand() % world->width;
        int startY = rand() % world->height;

        // Ensure starting point is in very deep ocean
        if (world->map[startY][startX].waterBody == TITLE_DEEP_OCEAN) {
            // Decide a direction for the trench to extend: 0=right, 1=down, 2=left, 3=up
            int direction = rand() % 4;
            bool canPlaceTrench = true;

            // Check if the trench can be placed without interruption
            for (int step = 0; step < trenchLength; step++) {
                int currentX = startX, currentY = startY;

                // Adjust current position based on direction
                switch (direction) {
                    case 0: currentX += step; break;
                    case 1: currentY += step; break;
                    case 2: currentX -= step; break;
                    case 3: currentY -= step; break;
                }

                // Check bounds and water body type
                if (currentX < 0 || currentX >= world->width || currentY < 0 || currentY >= world->height ||
                    world->map[currentY][currentX].waterBody != TITLE_DEEP_OCEAN) {
                    canPlaceTrench = false;
                    break;
                }
            }

            // Place the trench if possible
            if (canPlaceTrench) {
                for (int step = 0; step < trenchLength; step++) {
                    int currentX = startX, currentY = startY;

                    switch (direction) {
                        case 0: currentX += step; break;
                        case 1: currentY += step; break;
                        case 2: currentX -= step; break;
                        case 3: currentY -= step; break;
                    }

                    // Mark the tile as part of a deep ocean trench
                    world->map[currentY][currentX].specialFeature = TITLE_DEEP_OCEAN_TRENCH;
                }
                trenchCount++;
            }
        }

        attempts--;
    }
}


void placeCoralReefs(World* world) {
    int coralReefCount = 0; // Track the number of coral reefs placed
    int maxCoralReefs = 10; // Maximum number of coral reefs you want in the world

    for (int attempt = 0; attempt < 1000 && coralReefCount < maxCoralReefs; attempt++) {
        // Randomly select a tile
        int x = rand() % world->width;
        int y = rand() % world->height;

        // Check if the selected tile is suitable for a coral reef
        if (world->map[y][x].waterBody == TITLE_OCEAN && isNearCoast(world, x, y)) {
            // Place a coral reef in this tile and adjacent tiles to form a small blob
            setTileAsCoralReef(world, x, y);
            coralReefCount++;

            // Optionally, spread to adjacent tiles to form a small blob
            spreadCoralReef(world, x, y);
        }
    }
}

bool isNearCoast(World* world, int x, int y) {
    // Check tiles around the selected point to find if it's near the coast
    for (int offsetY = -3; offsetY <= 3; offsetY++) {
        for (int offsetX = -3; offsetX <= 3; offsetX++) {
            int checkX = x + offsetX;
            int checkY = y + offsetY;

            // Ensure we're not checking outside the map boundaries
            if (checkX >= 0 && checkX < world->width && checkY >= 0 && checkY < world->height) {
                // Check if the adjacent tile is a beach
                if (world->map[checkY][checkX].terrain == TITLE_BEACH) {
                    return true; // The tile is near the coast
                }
            }
        }
    }
    return false; // Not near the coast
}

void setTileAsCoralReef(World* world, int x, int y) {
    world->map[y][x].specialFeature = TITLE_CORAL_REEF;
}

void spreadCoralReef(World* world, int x, int y) {
    // Spread the coral reef to a small number of adjacent ocean tiles
    for (int offsetY = -1; offsetY <= 1; offsetY++) {
        for (int offsetX = -1; offsetX <= 1; offsetX++) {
            if (offsetX == 0 && offsetY == 0) continue; // Skip the original tile

            int adjacentX = x + offsetX;
            int adjacentY = y + offsetY;

            // Check bounds and ensure it's an ocean tile
            if (adjacentX >= 0 && adjacentX < world->width && adjacentY >= 0 && adjacentY < world->height &&
                world->map[adjacentY][adjacentX].waterBody == TITLE_OCEAN) {
                // Randomly decide to spread to this tile
                if (rand() % 2) {
                    setTileAsCoralReef(world, adjacentX, adjacentY);
                }
            }
        }
    }
}


void placeCrystalCaves(World* world) {
    int crystalCaveCount = 0; // Track the number of crystal caves placed
    int maxCrystalCaves = 5; // Maximum number of crystal caves you want in the world

    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            // Check if the current tile is a mountain
            if (world->map[y][x].terrain == TITLE_MOUNTAIN) {
                // Apply a random chance to convert this mountain tile into a Crystal Cave
                if (rand() % 100 < 5) { // 5% chance to convert a mountain tile into a Crystal Cave
                    world->map[y][x].specialFeature = TITLE_CRYSTAL_CAVE;
                    crystalCaveCount++;

                    if (crystalCaveCount >= maxCrystalCaves) {
                        return; // Stop searching if we've reached the maximum number of Crystal Caves
                    }
                }
            }
        }
    }
}


void placeIcyTundraPeaks(World* world) {
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            // Check if the current tile is part of a Tundra biome
            if (world->map[y][x].terrain == TITLE_TUNDRA) {
                // Use Perlin noise to add variation and select peaks
                float noiseValue = perlin(x * 0.05, y * 0.05); // Adjust scale for desired granularity

                // Determine if the current tundra tile should be converted into an Icy Tundra Peak
                if (noiseValue > 0.7) { // Adjust threshold for frequency of icy peaks
                    world->map[y][x].specialFeature = TITLE_ICY_TUNDRA;
                }
            }
        }
    }
}


void placeSulfuricSprings(World* world) {
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            // Check if the current tile is a lake
            if (world->map[y][x].waterBody == TITLE_LAKE) {
                // Check proximity to Volcanic Lands
                if (isNearVolcanicLands(world, x, y)) {
                    // Use a random chance to convert some lakes near Volcanic Lands into Sulfuric Springs
                    if (rand() % 100 < 80) { // 10% chance to convert
                        world->map[y][x].specialFeature = TITLE_SULFURIC_SPRINGS;
                    }
                }
            }
        }
    }
}

bool isNearVolcanicLands(World* world, int x, int y) {
    // Check the surrounding tiles for volcanic lands
    for (int offsetY = -10; offsetY <= 10; offsetY++) {
        for (int offsetX = -10; offsetX <= 10; offsetX++) {
            int checkX = x + offsetX;
            int checkY = y + offsetY;
            // Ensure we're not checking outside the map boundaries
            if (checkX >= 0 && checkX < world->width && checkY >= 0 && checkY < world->height) {
                if (world->map[checkY][checkX].terrain == TITLE_SCORCHED_LANDS) {
                    return true;
                }
            }
        }
    }
    return false;
}

void placeForestSubBiomes(World* world) {
    const int clusterRadius = 20; // Max radius for sub-biome clusters
    const float noiseThreshold = 0.5; // Noise threshold for cluster expansion

    // Array of sub-biome types to iterate over
    int subBiomeTypes[] = {
        TITLE_CRYSTAL_FOREST,
        TITLE_BAMBOO_FOREST,
        TITLE_IRONWOOD_FOREST,
        TITLE_FUNGI_FOREST,
        TITLE_REDWOOD_FOREST,
        TITLE_BLOSSOM_FOREST
    };
    int subBiomeCount = sizeof(subBiomeTypes) / sizeof(subBiomeTypes[0]);

    // Iterate over each sub-biome type
    for (int i = 0; i < subBiomeCount; i++) {
        int subBiomeType = subBiomeTypes[i];

        // Attempt to place a few clusters for each sub-biome type
        for (int cluster = 0; cluster < 3; cluster++) { // Try to place 3 clusters per sub-biome
            int centerX = rand() % world->width;
            int centerY = rand() % world->height;

            while (world->map[centerY][centerX].terrain != TITLE_FOREST){
                centerX = rand() % world->width;
                centerY = rand() % world->height;
            }
            // Ensure the center point is within a forest
            if (world->map[centerY][centerX].terrain != TITLE_FOREST) continue;

            // Spread the sub-biome around the center point
            for (int y = centerY - clusterRadius; y <= centerY + clusterRadius; y++) {
                for (int x = centerX - clusterRadius; x <= centerX + clusterRadius; x++) {
                    // Check bounds and ensure within a forest
                    if (x < 0 || x >= world->width || y < 0 || y >= world->height || world->map[y][x].terrain != TITLE_FOREST) continue;

                    // Calculate distance from the center and apply noise for organic shape
                    float distance = sqrt(pow(x - centerX, 2) + pow(y - centerY, 2));
                    float noise = perlin(x * 0.1, y * 0.1); // Adjust scale as needed

                    // If within radius and noise threshold, set sub-biome
                    if (distance <= clusterRadius && noise > noiseThreshold) {
                        world->map[y][x].specialFeature = subBiomeType;
                    }
                }
            }
        }
    }
}





void placeCanyons(World* world) {
    int numberOfCanyons = 5; // Desired number of canyons
    for (int i = 0; i < numberOfCanyons; i++) {
        // Randomly select a starting point in a desert
        int startX = rand() % world->width;
        int startY = rand() % world->height;
        
        while (world->map[startY][startX].terrain != TITLE_DESERT) {
            startX = rand() % world->width;
            startY = rand() % world->height;
        }

        // Create a canyon from this starting point
        createCanyon(world, startX, startY);
    }
}

void createCanyon(World* world, int startX, int startY) {
    int length = 3 + (rand() % 5);
    int currentX = startX;
    int currentY = startY;

    for (int i = 0; i < length; i++) {
        // Ensure current position is within world bounds
        if (currentX < 0 || currentX >= world->width || currentY < 0 || currentY >= world->height) {
            break; // Stop if the canyon would go out of bounds
        }
        
        // Set the current tile to canyon
        world->map[currentY][currentX].specialFeature = TITLE_CANYON;

        // Decide the next direction randomly
        int direction = rand() % 4; // 0=up, 1=right, 2=down, 3=left
        switch (direction) {
            case 0: currentY = max(currentY - 1, 0); break; // Move up
            case 1: currentX = min(currentX + 1, world->width - 1); break; // Move right
            case 2: currentY = min(currentY + 1, world->height - 1); break; // Move down
            case 3: currentX = max(currentX - 1, 0); break; // Move left
        }
    }
}

void placeSmoothGrasslandsGreenHills(World* world) {
    float detailScale = 0.1; // Adjust for finer detail or broader features
    float threshold = 0.5; // Threshold for determining hill presence

    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            if (world->map[y][x].terrain == TITLE_GRASSLANDS) {
                // Use Perlin noise to determine if this part of the grasslands should be a green hill
                float noiseValue = perlin(x * detailScale, y * detailScale);
                if (noiseValue > threshold) {
                    // This tile is part of the Smooth Grasslands Green Hills
                    world->map[y][x].specialFeature = TITLE_SMOOTH_GRASSLANDS_GREEN_HILLS;
                }
            }
        }
    }
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
    placeStructure(world, TITLE_CAVE, TITLE_MOUNTAIN, "center");
    placeStructure(world, TITLE_ANCIENT_RUINS, TITLE_DESERT, "south");
    placeStructure(world, TITLE_DOCK, TITLE_BEACH, "east");
    placeStructure(world, TITLE_CITY_YANGSE, TITLE_GRASSLANDS, "northeast");
    placeStructure(world, TITLE_CITY_RAMONDULL, TITLE_GRASSLANDS, "northwest");
    placeStructure(world, TITLE_CITY_MYKE, TITLE_GRASSLANDS, "southwest");
    placeStructure(world, TITLE_CITY_DORPORT, TITLE_GRASSLANDS, "southeast");
    placeStructure(world, TITLE_ORC_CAMP, TITLE_GRASSLANDS, "center");
    placeStructure(world, TITLE_ORC_CASTLE, TITLE_TUNDRA, "any");
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