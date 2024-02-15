// include/player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>

struct Camera; 
struct World;

// Player structure
typedef struct {
    int x, y; // Player position
    SDL_Texture* texture; // Player texture
} Player;

// Function declarations
void player_init(Player* player, SDL_Renderer* renderer);
void player_move(Player* player, int dx, int dy, const struct World* world);

#endif
