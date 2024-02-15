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
    TILE_GRASS,
    TILE_FOREST,
    TILE_MOUNTAIN,
    TILE_DESERT,
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
void world_init(World* world, int width, int height);
bool world_can_move_to(const World* world, int x, int y);

#endif
