// src/player.c
#include "config.h"
#include "world.h"
#include "player.h"
#include "graphics.h"
#include <SDL_image.h>

void player_init(Player* player, SDL_Renderer* renderer) {
    player->x = 4; // Initial position
    player->y = 4;
    // Use the load_texture function from graphics.c
    player->texture = load_texture(renderer, "assets/images/player.png");
    if (!player->texture) {
        printf("Failed to load player texture\n");
    }
}

void player_move(Player* player, int dx, int dy, const World* world) {
    int newX = player->x + dx;
    int newY = player->y + dy;

    // Use world_can_move_to to check if the new position is within bounds and walkable
    if (world_can_move_to(world, newX, newY)) {
        player->x = newX;
        player->y = newY;
    }
}


void player_render(Player* player, SDL_Renderer* renderer) {
    SDL_Rect dstRect = {
        player->x * CELL_WIDTH,
        player->y * CELL_HEIGHT,
        CELL_WIDTH,
        CELL_HEIGHT
    };
    SDL_RenderCopy(renderer, player->texture, NULL, &dstRect);
}

