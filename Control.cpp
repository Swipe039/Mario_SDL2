#include "Control.h"
#include <iostream>

void handleInput(SDL_Event &e, bool &running, Entity &mario, int &marioVelocityX, bool &dropThrough, GameState &state) {
    dropThrough = false;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_a:
                    marioVelocityX = -SPEED;
                    break;
                case SDLK_d:
                    marioVelocityX = SPEED;
                    break;
                case SDLK_w:
                    if (mario.onGround) {
                        mario.velocityY = JUMP_FORCE;
                        mario.onGround = false;
                    }
                    break;
                case SDLK_s:
                    if (mario.onGround) {
                        dropThrough = true;
                    }
                    break;
                case SDLK_ESCAPE:
                    state = PAUSE_MENU;
                    break;
            }
        }
        if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_a:
                case SDLK_d:
                    marioVelocityX = 0;
                    break;
            }
        }
    }
}

void handlePauseMenuInput(SDL_Event &e, GameState &state, bool &running) {
    static int selectedItem = 0;
    
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedItem = (selectedItem - 1 + MENU_ITEM_COUNT) % MENU_ITEM_COUNT;
                    break;
                case SDLK_DOWN:
                    selectedItem = (selectedItem + 1) % MENU_ITEM_COUNT;
                    break;
                case SDLK_RETURN:
                    if (selectedItem == 0) {
                        state = GAMEPLAY; // Continue
                    } else {
                        state = MAIN_MENU; // Exit to Menu
                    }
                    break;
                case SDLK_ESCAPE:
                    state = GAMEPLAY; // ESC to continue
                    break;
            }
        }
    }
}

void handleMapSelectInput(SDL_Event &e, GameState &state, int &selectedMap) {
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            state = MAIN_MENU;
            return;
        }
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedMap = std::max(0, selectedMap - MAP_SELECT_COLS);
                    break;
                case SDLK_DOWN:
                    selectedMap = std::min(MAP_SELECT_ROWS * MAP_SELECT_COLS - 1, selectedMap + MAP_SELECT_COLS);
                    break;
                case SDLK_LEFT:
                    selectedMap = (selectedMap - 1 + MAP_SELECT_ROWS * MAP_SELECT_COLS) % (MAP_SELECT_ROWS * MAP_SELECT_COLS);
                    break;
                case SDLK_RIGHT:
                    selectedMap = (selectedMap + 1) % (MAP_SELECT_ROWS * MAP_SELECT_COLS);
                    break;
                case SDLK_RETURN:
                    state = GAMEPLAY;
                    break;
                case SDLK_ESCAPE:
                    state = MAIN_MENU;
                    break;
            }
        }
    }
}