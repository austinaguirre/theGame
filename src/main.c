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
#include "ui.h"
#include "inventory.h"
#include "city.h"

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
    bool mapView = false;

    UIButton buttonInventory = {0, 400, 50, 50, "Inventory"};
    UIButton buttonStatsPage = {0, 450, 50, 50, "Stats Page"};

    SDL_Point mousePos;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            // Check if the game is currently not in MENU, PAUSE, or GAME_OVER states
            if (currentGameState != MENU && currentGameState != PAUSE && currentGameState != GAME_OVER) {
                // Check for Inventory button click
                if (checkButtonClick(&event, &buttonInventory)) {
                    currentGameState = (currentGameState == INVENTORY) ? GAMEPLAY : INVENTORY;
                    // if (currentGameState == INVENTORY) {
                    //     currentGameState = GAMEPLAY;
                    // }else{
                    //     currentGameState = INVENTORY;
                    // }
                }
                // Check for Stats Page button click
                else if (checkButtonClick(&event, &buttonStatsPage)) {
                    currentGameState = (currentGameState == STATS_PAGE) ? GAMEPLAY : STATS_PAGE;
                    // if(currentGameState == STATS_PAGE){
                    //     currentGameState = GAMEPLAY;
                    // }else{
                    //     currentGameState = INVENTORY;
                    // }
                }
            }

            SDL_GetMouseState(&mousePos.x, &mousePos.y);

            // Handle input based on the current game state
            switch (currentGameState) {
                case MENU:
                    main_menu_handle_mouse_movement(&event, &menuSelection);
                    main_menu_handle_mouse_click(&event, &running, &currentGameState);
                    main_menu_handle_keyboard_input(&event, &running, &currentGameState, &menuSelection);
                    break;
                case GAMEPLAY:
                    handle_gameplay_input(&event, &running, &player, &world, &currentGameState, &mapView);
                    break;
                case PAUSE:
                    if (event.key.keysym.sym == SDLK_ESCAPE && event.type == SDL_KEYDOWN) {
                        currentGameState = GAMEPLAY; // Resume game from pause
                    }
                    break;
                case ENEMY_ENCOUNTER:

                    break;
                case CITY_INTERACTION:{
                    City* currentCity = getCityFromTile(&world, player.x, player.y);
                    if (currentCity != NULL) {handle_city_interaction(&event, currentCity, &currentGameState);}
                    break;
                }
                case INVENTORY:
                    handle_inventory_input(&event, &player);
                    break;
                case GAME_OVER:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        currentGameState = GAMEPLAY; // Restart game from game over
                        player_init(&player, renderer); // Reinitialize player state
                        world_init(&world, worldWidth, worldHeight, seed); // Reset world
                    }
                    break;
                case STATS_PAGE:
                    handle_stats_page_input(&event);
                    break;
                case INSIDE_SHOP:
                    handleShopInput(&event, &player.inventory);
                    break;
            }
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
                    update_camera_position(&camera, &world, &player);
                    world_render(&world, renderer, &camera);
                    player_render(&player, renderer, &camera);
                }
                break;
            case PAUSE:
                render_text(renderer, "paused", 100, 100, textColor, textScale); // Display pause message
                break;
            case ENEMY_ENCOUNTER:
                render_text(renderer, "enemy", 100, 100, textColor, textScale); // Display pause message
                break;
            case CITY_INTERACTION:{
                City* currentCity = getCityFromTile(&world, player.x, player.y);
                if (currentCity != NULL) { renderCityInteraction(renderer, currentCity); } else { /*error handle*/}
                break;
            }
            case GAME_OVER:
                render_text(renderer, "Game Over", 100, 100, textColor, textScale); // Display game over message
                break;
            case INVENTORY:
                renderInventoryScreen(renderer, &player.inventory);
                renderDraggedItem(renderer, &player.inventory);
                break;
            case STATS_PAGE:
                renderStatsPageScreen(renderer, &player.stats, &player.characterClass);
                break;
            case INSIDE_SHOP:
                renderShopScreen(renderer, mousePos, &player.inventory);
                break;
        }

        // Render UI only in certain states
        if (currentGameState != MENU && currentGameState != PAUSE && currentGameState != GAME_OVER) {
            renderUI(renderer, &buttonInventory);
            renderUI(renderer, &buttonStatsPage);
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
