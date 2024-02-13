#include "input.h"
#include "player.h"
#include <SDL.h>
#include <stdbool.h>

// Adjust these based on your game's window size and grid cell size
const int GRID_WIDTH = 25; // Assuming 25 grid cells horizontally
const int GRID_HEIGHT = 18; // Assuming 18 grid cells vertically

void handle_input(SDL_Event *event, bool *running, Player *player) {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) {
            *running = false;
        } else if (event->type == SDL_KEYDOWN) {
            int dx = 0, dy = 0;
            switch (event->key.keysym.sym) {
                case SDLK_UP:    dy = -1; break;
                case SDLK_DOWN:  dy = 1; break;
                case SDLK_LEFT:  dx = -1; break;
                case SDLK_RIGHT: dx = 1; break;
            }
            // Prepare the new potential position
            int newX = player->x + dx;
            int newY = player->y + dy;

            // Boundary check
            if (newX >= 0 && newX < GRID_WIDTH && newY >= 0 && newY < GRID_HEIGHT) {
                player->x = newX;
                player->y = newY;
            }
        }
    }
}
