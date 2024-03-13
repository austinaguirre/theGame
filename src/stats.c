//stats.c
#include "stats.h"

int selectedMainStat = -1;
UIButton returnButton = {10, 530, 300, 50, "return"};


void renderStatsPageScreen(SDL_Renderer* renderer, PlayerStats* playerStats, CharacterClass* characterClass) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //background
    SDL_RenderClear(renderer);
    render_text(renderer, "stats page", 0, 0, textColor, textScale * 0.5);

    if (selectedMainStat == -1) {
        renderMainStats(renderer, playerStats);
    } else {
        renderSubStats(renderer, playerStats, selectedMainStat);
        renderButton(renderer, &returnButton);
    }
}

void renderMainStats(SDL_Renderer* renderer, PlayerStats* playerStats) {
    const int buttonHeight = 50; // Example height
    const int buttonWidth = 300; // Example width
    const int startY = 60; // Starting Y position
    const int gapY = 60; // Gap between buttons
    char* mainStatLabels[NUM_MAIN_STATS] = {"melee", "range", "defence", "magic", "agility"};

    for (int i = 0; i < NUM_MAIN_STATS; i++) {
        int yPos = startY + (gapY * i);

        // Define button for this stat
        mainStatButtons[i] = (UIButton){20, yPos, buttonWidth, buttonHeight, mainStatLabels[i]};
        
        // Render button / also shows the label for the button
        renderButton(renderer, &mainStatButtons[i]);
    }
}

void renderSubStats(SDL_Renderer* renderer, PlayerStats* playerStats, int mainStatIndex) {
    SDL_Color textColor = {255, 255, 255, 255}; // White color for text
    float textScale = 1.0; // Example text scale
    int x = 20; // X position for text
    int y = 60; // Starting Y position
    int incr = 50; // Increment for Y position after each stat

    char statText[128]; // Buffer for formatted stat text

    switch (mainStatIndex) {
        case 0: // Melee Stats
            sprintf(statText, "strength: %d", playerStats->melee.strength);
            render_text(renderer, statText, x, y, textColor, textScale * 0.4);
            y += incr;

            sprintf(statText, "attack speed: %d", playerStats->melee.attackSpeed);
            render_text(renderer, statText, x, y, textColor, textScale * 0.4);
            break;
        case 1: // Range Stats
            sprintf(statText, "accuracy: %d", playerStats->range.accuracy);
            render_text(renderer, statText, x, y, textColor, textScale * 0.4);
            y += incr;

            sprintf(statText, "reload speed: %d", playerStats->range.reloadSpeed);
            render_text(renderer, statText, x, y, textColor, textScale * 0.4);
            break;
        case 2: // Defence Stats
            sprintf(statText, "health: %d", playerStats->defence.health);
            render_text(renderer, statText, x, y, textColor, textScale * 0.4);
            y += incr;

            sprintf(statText, "health recovery: %d", playerStats->defence.healthRecovery);
            render_text(renderer, statText, x, y, textColor, textScale * 0.4);
            y += incr;

            sprintf(statText, "magic resistance: %d", playerStats->defence.magicResistance);
            render_text(renderer, statText, x, y, textColor, textScale * 0.4);
            y += incr;

            sprintf(statText, "melee resistance: %d", playerStats->defence.meleeResistance);
            render_text(renderer, statText, x, y, textColor, textScale * 0.4);
            break;
        case 3: // Magic Stats
            sprintf(statText, "spell power: %d", playerStats->magic.spellPower);
            render_text(renderer, statText, x, y, textColor, textScale * 0.4);
            y += incr;

            sprintf(statText, "mana: %d", playerStats->magic.mana);
            render_text(renderer, statText, x, y, textColor, textScale * 0.4);
            y += incr;

            sprintf(statText, "mana recovery: %d", playerStats->magic.manaRecovery);
            render_text(renderer, statText, x, y, textColor, textScale * 0.4);
            break;
        case 4: // Agility Stats
            sprintf(statText, "dodge chance: %d", playerStats->agility.dodgeChance);
            render_text(renderer, statText, x, y, textColor, textScale * 0.4);
            y += incr;

            sprintf(statText, "movement speed: %d", playerStats->agility.movementSpeed);
            render_text(renderer, statText, x, y, textColor, textScale * 0.4);
            break;
        default:
            // Handle out-of-range index if necessary
            break;
    }
}

void handle_stats_page_input(SDL_Event* event) {
    // Check if a main stat button was clicked
    int clickedIndex = checkStatsButtonClick(event, mainStatButtons, NUM_MAIN_STATS);

    if (clickedIndex != -1) {
        // If a main stat button was clicked, update selectedMainStat
        selectedMainStat = clickedIndex;
    } else {
        // Check other buttons specific to the stats page, like a return button
        // For example, to return to the main stats view from sub-stats view
        if (selectedMainStat != -1 && checkReturnButtonClick(event, &returnButton)) {
            selectedMainStat = -1; // Return to main stats view
        }
    }
}

int checkStatsButtonClick(SDL_Event* event, UIButton* buttons, int numButtons) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        for (int i = 0; i < numButtons; i++) {
            if (mouseX >= buttons[i].x && mouseX <= buttons[i].x + buttons[i].width &&
                mouseY >= buttons[i].y && mouseY <= buttons[i].y + buttons[i].height) {
                return i; // Return the index of the clicked button
            }
        }
    }
    return -1; // No button was clicked
}

int checkReturnButtonClick(SDL_Event* event, UIButton* button) {
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX >= button->x && mouseX <= button->x + button->width &&
            mouseY >= button->y && mouseY <= button->y + button->height) {
            return 1; // Return button was clicked
        }
    }
    return 0; // Return button was not clicked
}




