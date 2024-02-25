// include/game.h
#ifndef GAME_H
#define GAME_H

// Game state enumeration
typedef enum GameState {
    MENU,
    GAMEPLAY,
    PAUSE,
    GAME_OVER,
    ENEMY_ENCOUNTER,
    CITY_INTERACTION,
    INVENTORY,
    STATS_PAGE
} GameState;

#endif
