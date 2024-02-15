// include/world.h
#ifndef WORLD_H
#define WORLD_H

#include <SDL.h>
#include <stdbool.h>

struct Camera;

typedef enum {
    TILE_EMPTY,
    TILE_WALL,
    TILE_WATER,
    TILE_SAND,
    TILE_LAKE,
    TILE_RIVER,
    TILE_GRASS,
    TILE_FOREST,
    TILE_MOUNTAIN,
    TILE_DESERT,
    TILE_SNOW,
    TILE_JUNGLE,
    TILE_CITY,
} TileType;


typedef struct {
    TileType type;
    // Add more properties here (e.g., isPassable, spriteIndex, etc.)
} Tile;

// World structure
typedef struct World {
    Tile** map;
    int width, height;
} World;


// Function declarations
void world_init(World* world, int width, int height, unsigned int seed);
bool world_can_move_to(const World* world, int x, int y);

#endif
