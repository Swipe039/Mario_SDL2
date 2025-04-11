#ifndef CONTROL_H
#define CONTROL_H

#include <SDL2/SDL.h>
#include <vector>
#include "Entity.h"
#include "Constants.h"

// << THAY ĐỔI KHAI BÁO HÀM handleInput >>
// Giờ đây nó nhận trực tiếp mario và thay đổi velocityX của mario
void handleInput(Entity &mario, bool &running, bool &dropThrough, GameState &state);
// << Giữ nguyên các hàm khác >>
void handleMainMenuInput(SDL_Event &e, GameState &state, bool &running, int &selectedItem);
void handlePauseMenuInput(SDL_Event &e, GameState &state, bool &running, int &selectedItem);
void handleMapSelectInput(SDL_Event &e, GameState &state, bool &running, int &selectedMap);
void handleWinLoseInput(SDL_Event &e, GameState &currentState, GameState &nextState, bool &running, int &selectedItem);

#endif