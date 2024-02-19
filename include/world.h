// include/world.h
#ifndef WORLD_H
#define WORLD_H

#include <SDL.h>
#include <stdbool.h>

struct Camera;

typedef enum {
    TITLE_VERY_LOW,
    TITLE_LOW,
    TITLE_MEDIUM,
    TITLE_HIGH,
    TITLE_VERY_HIGH,
} TerrainHeight;

typedef enum {
    TITLE_VOLCANIC_LANDS,
    TITLE_TUNDRA,
    TITLE_GRASSLANDS,
    TITLE_SWAMPLAND,
    TITLE_DEAD_LANDS,
    TITLE_FOREST,
    TITLE_SPIDERLANDS,
    TITLE_MOUNTAIN,
    TITLE_SCORCHED_LANDS,
    TITLE_SAVANNAH,
    TITLE_JUNGLE,
    TITLE_BEACH,
    TITLE_DESERT,
    TITLE_DEFAULT_TERRAIN,
} TerrainType;

typedef enum{
    TITLE_RIVER,
    TITLE_OCEAN,
    TITLE_DEEP_OCEAN,
    TITLE_LAKE,
    TITLE_DEFAULT_WATER
} WaterBodyType;

typedef enum{
    TITLE_BONE_BEACH,
    TITLE_TROPICAL_BEACH,
    TITLE_SNOWY_MOUNTAIN,
    TITLE_DEEP_OCEAN_TRENCH,
    TITLE_CORAL_REEF,
    TITLE_CRYSTAL_CAVE,
    TITLE_PARADISE_ISLES,
    TITLE_ICY_TUNDRA,
    TITLE_SULFURIC_SPRINGS,
    TITLE_CRYSTAL_FOREST,
    TITLE_BAMBOO_FOREST,
    TITLE_IRONWOOD_FOREST,
    TITLE_FUNGI_FOREST,
    TITLE_REDWOOD_FOREST,
    TITLE_BLOSSOM_FOREST,
    TITLE_CANYON,
    TITLE_SMOOTH_GRASSLANDS_GREEN_HILLS,
    TITLE_ACTIVE_VOLCANIC_LANDS,
    TITLE_DEFAULT_SPECIAL
} SpecialFeatureType;

typedef enum{
    TITLE_SHIP_WRECK,
    TITLE_FARMLAND,
    TITLE_CAVE,
    TITLE_ANCIENT_RUINS,
    TITLE_DOCK,
    TITLE_CITY_YANGSE,
    TITLE_CITY_RAMONDULL,
    TITLE_CITY_MYKE,
    TITLE_CITY_DORPORT,
    TITLE_ORC_CAMP,
    TITLE_ORC_CASTLE,
    TITLE_ELF_HIDEOUT,
    TITLE_DEFAULT_STRUCTURE
} StructureType;

typedef enum {
    INTERACTION_CITY,
    INTERACTION_COMBAT
} InteractionType;

typedef struct {
    TerrainHeight height;
    TerrainType terrain;
    WaterBodyType waterBody;
    StructureType structure;
    SpecialFeatureType specialFeature;
    InteractionType interactionType;
} Tile;

// World structure
typedef struct World {
    Tile** map;
    int width, height;
} World;

typedef struct TileRenderCount {
    int terrainCounts[15]; // Adjust the size based on the number of TerrainTypes
    int waterBodyCounts[5]; // Adjust the size based on the number of WaterBodyTypes
    int specialFeatureCounts[19]; // Adjust the size based on the number of SpecialFeatureTypes
    int structureCounts[14]; // Adjust the size based on the number of StructureTypes
    int heightCounts[5];
} TileRenderCount;


// Function declarations
void world_init(World* world, int width, int height, unsigned int seed);
bool world_can_move_to(const World* world, int x, int y);

void generateHeightMap(World* world, float scale, unsigned int seed);
void applyBiomesBasedOnHeightAndPosition(World* world);
void assignMediumBiome(Tile* tile, int x, int y, int width, int height);
void assignHighBiome(Tile* tile, float latFactor, int x, int y);
void assignLowBiome(Tile* tile, float biomeNoise);
void createBeaches(World* world);
bool isPotentialBeachTile(World* world, int x, int y);


void spawnRivers(World* world);
bool canContinueRiver(World* world, int x, int y);
void generateRiverPath(World* world, int x, int y);
bool isValidRiverSource(World* world, int x, int y);


void setLakeTiles(World* world, int x, int y);
bool checkEnclosure(World* world, int x, int y, bool* visited);
bool isEnclosedByBeach(World* world, int x, int y);
bool isPotentialLakeTile(World* world, int x, int y);
void detectAndSetLakes(World* world);
void floodFillSetLake(World* world, int x, int y, bool** visited);
void initializeVisited(bool*** visited, int width, int height);
void cleanupVisited(bool*** visited, int height);


void applySpecialFeatures(World* world);

void placeSpecialBeaches(World* world);
void createSpecialBeachStretch(World* world, bool isNorth, int minLength, int maxLength, SpecialFeatureType specialBeachType);

void placeSubBiomes(World* world);
bool isIsolatedWaterTile(World* world, int x, int y);

void placeDeepOceanTrenches(World* world);

void placeCoralReefs(World* world);
void spreadCoralReef(World* world, int x, int y);
void setTileAsCoralReef(World* world, int x, int y);
bool isNearCoast(World* world, int x, int y);


void placeCrystalCaves(World* world);
void placeIcyTundraPeaks(World* world);

void placeSulfuricSprings(World* world);
bool isNearVolcanicLands(World* world, int x, int y);

void placeForestSubBiomes(World* world);

void placeCanyons(World* world);
void createCanyon(World* world, int startX, int startY);

void placeSmoothGrasslandsGreenHills(World* world);

bool isInNorthernHalf(World* world, int y);
void placeStructures(World* world);
void placeStructure(World* world, StructureType structure, TerrainType preferredTerrain, const char* location, bool isCity);
bool isValidLocationForStructure(World* world, int x, int y, TerrainType preferredTerrain, const char* location);

#endif
