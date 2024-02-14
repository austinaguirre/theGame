#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include "game.h"
#include "graphics.h"
#include "input.h"
#include "player.h"
#include "world.h"
#include "text.h"

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    // Initialize SDL and create window and renderer
    graphics_init(&window, &renderer);

    // Initialize text rendering
    text_init(renderer);

    // Initialize player
    Player player;
    player_init(&player, renderer);

    // Initialize the world
    World world;
    world_init(&world);

    bool running = true;
    SDL_Event event;
    GameState currentGameState = MENU;

    int menuSelection = 0; // 0 for "Start Game", 1 for "Quit"

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (currentGameState) {
                    case MENU:
                        if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_UP) {
                            menuSelection = !menuSelection; // Toggle menu selection
                        } else if (event.key.keysym.sym == SDLK_RETURN) {
                            if (menuSelection == 0) {
                                currentGameState = GAMEPLAY; // Start the game
                            } else {
                                running = false; // Quit the game
                            }
                        }
                        break;
                    case GAMEPLAY:
                        if (event.key.keysym.sym == SDLK_ESCAPE) {
                            currentGameState = PAUSE; // Pause the game on ESC
                        } else {
                            handle_input(&event, &running, &player, &world);
                        }
                        break;
                    case PAUSE:
                        if (event.key.keysym.sym == SDLK_ESCAPE) { // Press ESC again to resume
                            currentGameState = GAMEPLAY;
                        }
                        break;
                    case GAME_OVER:
                        if (event.key.keysym.sym == SDLK_RETURN) { // Press ENTER to restart
                            currentGameState = GAMEPLAY;
                            player_init(&player, renderer); // Reset player
                            world_init(&world); // Reset world
                        }
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Default clear color
        SDL_RenderClear(renderer);

        switch (currentGameState) {
            case MENU:
                render_text(renderer, "Start Game", 100, 100 - (menuSelection * 50));
                render_text(renderer, "Quit", 100, 150 - (menuSelection * 50));
                break;
            case GAMEPLAY:
                world_render(&world, renderer);
                player_render(&player, renderer);
                break;
            case PAUSE:
                render_text(renderer, "Paused", 100, 100);
                break;
            case GAME_OVER:
                render_text(renderer, "Game Over", 100, 100);
                break;
        }

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    text_cleanup();
    SDL_DestroyTexture(player.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
