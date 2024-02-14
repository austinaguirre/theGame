// include/input.h
#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>
#include <stdbool.h>
#include "player.h"
#include "world.h"


// Function declarations
// Adjust the signature in input.h
void handle_input(SDL_Event *event, bool *running, Player *player, World* world);

#endif
