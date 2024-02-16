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

typedef struct {
    TerrainHeight height;
    TerrainType terrain;
    WaterBodyType waterBody;
    StructureType structure;
    SpecialFeatureType specialFeature;
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
TerrainType determineMediumTerrain(World* world, int x, int y, int southThreshold);
bool isAdjacentToLand(World* world, int x, int y);
void applySpecialFeatures(World* world);
bool isInNorthernHalf(World* world, int y);
void placeStructures(World* world);
void placeStructure(World* world, StructureType structure, TerrainType preferredTerrain, const char* location);
bool isValidLocationForStructure(World* world, int x, int y, TerrainType preferredTerrain, const char* location);

#endif
