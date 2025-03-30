#ifndef CONTROL_H
#define CONTROL_H

#include <SDL2/SDL.h>
#include <vector>
#include "Entity.h"
#include "Constants.h"
void handleInput(SDL_Event &e, bool &running, Entity &mario, int &marioVelocityX, bool &dropThrough, GameState &state);
void handlePauseMenuInput(SDL_Event &e, GameState &state, bool &running);
void handleMapSelectInput(SDL_Event &e, GameState &state, int &selectedMap);
#endif