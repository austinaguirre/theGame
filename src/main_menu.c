#include "main_menu.h"
#include "text.h"
#include <stdbool.h>
#include "game.h"


// Define colors and text scale globally for easy adjustments
static const SDL_Color textColor = {255, 255, 255, 255}; // White color for text
static const float textScale = 1.0f;
static const SDL_Color highlightColor = {255, 0, 0, 255}; // Red color for highlighted text
static SDL_Rect menuItemsPositions[2]; // Assuming 2 menu items for simplicity
static int hoverIndex = -1; // -1 indicates no item is being hovered


void main_menu_init(SDL_Renderer* renderer) {
    // Initialize menu-specific resources if any
    // For this example, we're using the shared text rendering system, so no init here
    menuItemsPositions[0] = (SDL_Rect){100, 100, 200, 50}; // "Start Game"
    menuItemsPositions[1] = (SDL_Rect){100, 150, 200, 50}; // "Quit"
}


void main_menu_render(SDL_Renderer* renderer, int menuSelection) {
    const char* menuItems[] = {"start game", "quit"};
    int numMenuItems = sizeof(menuItems) / sizeof(menuItems[0]);
    SDL_Color hoverColor = {255, 255, 0, 255}; // Yellow for hover

    for (int i = 0; i < numMenuItems; i++) {
        SDL_Color currentColor = textColor;
        if (i == menuSelection) {
            currentColor = highlightColor; // Red for selection
        }
        if (i == hoverIndex) {
            currentColor = hoverColor; // Yellow for hover
        }
        render_text_trimmed(renderer, menuItems[i], menuItemsPositions[i].x, menuItemsPositions[i].y, currentColor, textScale);
    }
}

void main_menu_handle_mouse_movement(SDL_Event* event, int* menuSelection) {
    hoverIndex = -1; // Reset hover state on each movement
    if (event->type == SDL_MOUSEMOTION) {
        int mouseX = event->motion.x;
        int mouseY = event->motion.y;
        for (int i = 0; i < 2; i++) {
            if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &menuItemsPositions[i])) {
                hoverIndex = i; // Update hover index based on mouse position
                break;
            }
        }
    }
}

void main_menu_handle_mouse_click(SDL_Event* event, bool* running, GameState* currentGameState) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = event->button.x;
        int mouseY = event->button.y;
        for (int i = 0; i < 2; i++) {
            if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &menuItemsPositions[i])) {
                if (i == 0) *currentGameState = GAMEPLAY; // Start the game
                else if (i == 1) *running = false; // Quit the game
                break;
            }
        }
    }
}

void main_menu_handle_keyboard_input(SDL_Event* event, bool* running, GameState* currentGameState, int* menuSelection) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_UP:
                *menuSelection = (*menuSelection - 1 + 2) % 2; // Cycle through menu options upwards
                break;
            case SDLK_DOWN:
                *menuSelection = (*menuSelection + 1) % 2; // Cycle through menu options downwards
                break;
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                if (*menuSelection == 0) {
                    *currentGameState = GAMEPLAY; // Start the game
                } else if (*menuSelection == 1) {
                    *running = false; // Quit the game
                }
                break;
        }
    }
}

void main_menu_cleanup(void) {
    // Cleanup menu-specific resources if any
}
