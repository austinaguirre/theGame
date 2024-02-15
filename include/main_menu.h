#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SDL.h>
#include <stdbool.h>
#include "game.h"

// Initializes the main menu
void main_menu_init(SDL_Renderer* renderer);

void main_menu_handle_keyboard_input(SDL_Event* event, bool* running, GameState* currentGameState, int* menuSelection);

// Renders the main menu
void main_menu_render(SDL_Renderer* renderer, int menuSelection);

void main_menu_handle_mouse_movement(SDL_Event* event, int* menuSelection);
void main_menu_handle_mouse_click(SDL_Event* event, bool* running, GameState* currentGameState);

// Cleans up resources used by the main menu (if any)
void main_menu_cleanup(void);

#endif // MAIN_MENU_H
