#include "input.h"
#include "player.h"
#include "world.h"
#include <SDL.h>
#include <stdbool.h>
#include "game.h"
#include "ui.h"

// Adjust these based on your game's window size and grid cell size
const int GRID_WIDTH = 20; // Assuming 25 grid cells horizontally
const int GRID_HEIGHT = 15; // Assuming 18 grid cells vertically

void handle_input(SDL_Event *event, bool *running, Player *player, World *world) {
    if (event->type == SDL_KEYDOWN) {
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

        // Use world_can_move_to to check if the position is valid
        if (world_can_move_to(world, newX, newY)) {
            player->x = newX;
            player->y = newY;
        }
    }
}

void handle_gameplay_input(SDL_Event *event, bool *running, Player *player, World *world, GameState *currentGameState, bool *mapView){
    if (event->key.keysym.sym == SDLK_ESCAPE && event->type == SDL_KEYDOWN) {
        *currentGameState = PAUSE; // Toggle pause state with ESC
    } else if (event->key.keysym.sym == SDLK_m && event->type == SDL_KEYDOWN) {
        *mapView = !(*mapView); // Toggle map view
    } else if (event->key.keysym.sym == SDLK_RETURN && event->type == SDL_KEYDOWN) {
        // Handle interactions based on the current tile the player is standing on
        Tile currentTile = world->map[player->y][player->x];
        switch (currentTile.interactionType) {
            case INTERACTION_CITY:
                // Enter city interaction mode
                *currentGameState = CITY_INTERACTION;
                break;
            default:
                // Start an enemy encounter for all other types, except for special cases like water
                *currentGameState = ENEMY_ENCOUNTER;
                break;
        }
    } else {
        handle_input(event, running, player, world);
    }
}

bool checkButtonClick(SDL_Event* event, UIButton* button) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX > button->x && mouseX < button->x + button->width &&
            mouseY > button->y && mouseY < button->y + button->height) {
            return true;
        }
    }
    return false;
}


