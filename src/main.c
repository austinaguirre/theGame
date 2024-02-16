#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include "game.h"
#include "input.h"
#include "world.h"
#include "player.h"
#include "graphics.h"
#include "text.h"
#include "main_menu.h"
#include "config.h"

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    // Initialize the various components of the game
    graphics_init(&window, &renderer);
    text_init(renderer, "assets/text/alphabet-7111781_640.png");
    main_menu_init(renderer);

    Player player;
    player_init(&player, renderer);

    Camera camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}; // Replace WINDOW_WIDTH and WINDOW_HEIGHT with actual values

    World world;
    // Example world size; adjust as needed for your game
    int worldWidth = 200; // Width of the world in tiles
    int worldHeight = 200; // Height of the world in tiles
    unsigned int seed = 123456789;
    world_init(&world, worldWidth, worldHeight, seed); // Initialize with dynamic size

    bool running = true;
    SDL_Event event;
    GameState currentGameState = MENU;

    int menuSelection = 0; // For handling menu selection
    SDL_Color textColor = {255, 255, 255, 255}; // Default text color
    float textScale = 1.0f; // Default text scale
    bool mapView = false;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            // Handle input based on the current game state
            switch (currentGameState) {
                case MENU:
                    main_menu_handle_mouse_movement(&event, &menuSelection);
                    main_menu_handle_mouse_click(&event, &running, &currentGameState);
                    main_menu_handle_keyboard_input(&event, &running, &currentGameState, &menuSelection);
                    break;
                case GAMEPLAY:
                    if (event.key.keysym.sym == SDLK_ESCAPE && event.type == SDL_KEYDOWN) {
                        currentGameState = PAUSE; // Toggle pause state with ESC
                    } else if (event.key.keysym.sym == SDLK_m && event.type == SDL_KEYDOWN) {
                        mapView = !mapView; // Toggle map view
                    } else {
                        handle_input(&event, &running, &player, &world);
                    }
                    break;
                case PAUSE:
                    if (event.key.keysym.sym == SDLK_ESCAPE && event.type == SDL_KEYDOWN) {
                        currentGameState = GAMEPLAY; // Resume game from pause
                    }
                    break;
                case GAME_OVER:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        currentGameState = GAMEPLAY; // Restart game from game over
                        player_init(&player, renderer); // Reinitialize player state
                        world_init(&world, worldWidth, worldHeight, seed); // Reset world
                    }
                    break;
            }
        }

        if (currentGameState == GAMEPLAY) {
            update_camera_position(&camera, &world, &player);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Set background color
        SDL_RenderClear(renderer); // Clear the screen with the background color

        // Render the current state of the game
        switch (currentGameState) {
            case MENU:
                main_menu_render(renderer, menuSelection);
                break;
            case GAMEPLAY:
                if (mapView) {
                    // Adjust scaleFactor as needed to fit the map on your screen
                    float scaleFactor = 0.1f; // Example scale factor, adjust as needed
                    renderMiniMap(renderer, &world, &player, scaleFactor);
                } else {
                    // Regular gameplay rendering
                    world_render(&world, renderer, &camera);
                    player_render(&player, renderer, &camera);
                }
                break;
            case PAUSE:
                render_text(renderer, "paused", 100, 100, textColor, textScale); // Display pause message
                break;
            case GAME_OVER:
                render_text(renderer, "Game Over", 100, 100, textColor, textScale); // Display game over message
                break;
        }

        SDL_RenderPresent(renderer); // Update the screen with rendered content
    }

    // Clean up resources
    main_menu_cleanup();
    text_cleanup();
    SDL_DestroyTexture(player.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
