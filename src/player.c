// src/player.c
#include "player.h"
#include "graphics.h"
#include <SDL_image.h>

void player_init(Player* player, SDL_Renderer* renderer) {
    player->x = 0; // Initial position
    player->y = 0;
    // Use the load_texture function from graphics.c
    player->texture = load_texture(renderer, "assets/images/player.png");
    if (!player->texture) {
        printf("Failed to load player texture\n");
    }
}

void player_move(Player* player, int dx, int dy, int gridWidth, int gridHeight) {
    // Calculate new position and perform boundary checks
    int newX = player->x + dx;
    int newY = player->y + dy;
    if (newX >= 0 && newX < gridWidth && newY >= 0 && newY < gridHeight) {
        player->x = newX;
        player->y = newY;
    }
}


void player_render(Player* player, SDL_Renderer* renderer) {
    // Use the render_texture function from graphics.c
    // Adjust the size if needed to match your grid cell size
    render_texture(renderer, player->texture, player->x * 32, player->y * 32);
}
