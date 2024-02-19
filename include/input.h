// include/input.h
#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>
#include <stdbool.h>
#include "player.h"
#include "world.h"
#include "game.h"
#include "ui.h"


// Function declarations
// Adjust the signature in input.h
void handle_input(SDL_Event *event, bool *running, Player *player, World* world);
void handle_gameplay_input(SDL_Event *event, bool *running, Player *player, World *world, GameState *currentGameState, bool *mapView);
bool checkButtonClick(SDL_Event* event, UIButton* button);


#endif
