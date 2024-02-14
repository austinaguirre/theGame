// include/world.h
#ifndef WORLD_H
#define WORLD_H

#include <SDL.h>
#include <stdbool.h>

// World structure
typedef struct World{
    int map[18][25]; // Example size, adjust based on your needs
} World;

// Function declarations
void world_init(World* world);
void world_render(const World* world, SDL_Renderer* renderer);
bool world_can_move_to(const World* world, int x, int y);

#endif
