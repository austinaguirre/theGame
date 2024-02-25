//stats.h
#ifndef STATS_H
#define STATS_H

#include <SDL.h>
#include "config.h"
#include "text.h"
#include "ui.h"

#define NUM_MAIN_STATS 5

typedef enum {
    CLASS_KNIGHT,
    CLASS_ARCHER,
    CLASS_RANGER,
    CLASS_PALADIN,
    CLASS_NECROMANCER,
    CLASS_HEALER
} CharacterClass;

typedef struct {
    int strength;
    int attackSpeed;
} MeleeStats;

typedef struct {
    int accuracy;
    int reloadSpeed;
} RangeStats;

typedef struct {
    int health;
    int healthRecovery;
    int magicResistance;
    int meleeResistance;
} DefenceStats;

typedef struct {
    int spellPower;
    int mana;
    int manaRecovery;
} MagicStats;

typedef struct {
    int dodgeChance;
    int movementSpeed;
} AgilityStats;

typedef struct {
    MeleeStats melee;
    RangeStats range;
    DefenceStats defence;
    MagicStats magic;
    AgilityStats agility;
} PlayerStats;

UIButton mainStatButtons[NUM_MAIN_STATS];

void renderStatsPageScreen (SDL_Renderer* renderer, PlayerStats* playerStats, CharacterClass* characterClass);
void renderMainStats(SDL_Renderer* renderer, PlayerStats* playerStats);
void renderSubStats(SDL_Renderer* renderer, PlayerStats* playerStats, int mainStatIndex);
int checkStatsButtonClick(SDL_Event* event, UIButton* buttons, int numButtons);
int checkReturnButtonClick(SDL_Event* event, UIButton* button);
void handle_stats_page_input(SDL_Event* event);

#endif
