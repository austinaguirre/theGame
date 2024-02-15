// src/player.c
#include "graphics.h"
#include "config.h"
#include "world.h"
#include "player.h"
#include <SDL_image.h>

void player_init(Player* player, SDL_Renderer* renderer) {
    player->x = 100; // Initial position
    player->y = 100;
    // Use the load_texture function from graphics.c
    player->texture = load_texture(renderer, "assets/images/player.png");
    if (!player->texture) {
        printf("Failed to load player texture\n");
    }
}

void player_move(Player* player, int dx, int dy, const struct World* world) {
    int newX = player->x + dx;
    int newY = player->y + dy;

    // Use world_can_move_to to check if the new position is within bounds and walkable
    if (world_can_move_to(world, newX, newY)) {
        player->x = newX;
        player->y = newY;
    }
}



