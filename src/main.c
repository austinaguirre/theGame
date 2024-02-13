#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include "graphics.h"
#include "input.h"
#include "player.h"

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    // Initialize SDL and create window and renderer
    graphics_init(&window, &renderer);

    // Initialize player
    Player player;
    player_init(&player, renderer); // Initialize the player with the renderer

    bool running = true;
    SDL_Event event;

    while (running) {
        handle_input(&event, &running, &player);

        // Clear the screen to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        player_render(&player, renderer);

        // Update screen
        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyTexture(player.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
