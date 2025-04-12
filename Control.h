#ifndef CONTROL_H
#define CONTROL_H

#include <SDL2/SDL.h>
#include <vector>
#include "Entity.h"
#include "Constants.h"

void handleInput(Entity &mario, bool &running, bool &dropThrough, GameState &state);
void handleMainMenuInput(SDL_Event &e, GameState &state, bool &running, int &selectedItem);
void handlePauseMenuInput(SDL_Event &e, GameState &state, bool &running, int &selectedItem);
void handleMapSelectInput(SDL_Event &e, GameState &state, bool &running, int &selectedMap);
void handleWinLoseInput(SDL_Event &e, GameState &currentState, GameState &nextState, bool &running, int &selectedItem);

#endif