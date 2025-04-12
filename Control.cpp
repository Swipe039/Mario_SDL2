#include "Control.h"
#include "Texture.h"
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <algorithm>
#include "GameLogic.h"
void handleInput(Entity &mario, bool &running, bool &dropThrough, GameState &state) {
    SDL_Event event;

    dropThrough = false;

    while(SDL_PollEvent(&event)) {
         if (event.type == SDL_QUIT) {
            running = false;
            return; 
        }

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_w:
                case SDLK_SPACE: 
                    if (mario.onGround) {
                        mario.velocityY = JUMP_FORCE;
                        mario.onGround = false;
                        if (jumpSound) Mix_PlayChannel(-1, jumpSound, 0); 
                    }
                    break;
                case SDLK_s:
                    dropThrough = true;
                    break;
                case SDLK_ESCAPE:
                    state = PAUSE_MENU; 

                    break;

            }
        }

    }

    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    int desiredVelocityX = 0; 

    if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT]) { 
        desiredVelocityX -= SPEED;
    }
    if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT]) { 
        desiredVelocityX += SPEED;
    }

    mario.velocityX = desiredVelocityX; 

    if (state == PAUSE_MENU) {
        mario.velocityX = 0;
    }
}

void handleMainMenuInput(SDL_Event &e, GameState &nextState, bool &running, int &selectedItem) {

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
                       nextState = MAP_SELECT;
                       return; 
                   }

                   else if (selectedItem == 1) { 
                       running = false;
                       return; 
                   }
                   break; 

               case SDLK_ESCAPE: 
                    running = false;
                    return; 
           } 
       } 
   } 
}

void handlePauseMenuInput(SDL_Event &e, GameState &nextState, bool &running, int &selectedItem) {

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedItem = (selectedItem - 1 + PAUSE_MENU_ITEM_COUNT) % PAUSE_MENU_ITEM_COUNT;
                    break;
                case SDLK_DOWN:
                    selectedItem = (selectedItem + 1) % PAUSE_MENU_ITEM_COUNT;
                    break;
                case SDLK_RETURN:
                    if (selectedItem == 0) { 
                        nextState = GAMEPLAY;
                    } else { 
                        nextState = MAIN_MENU;
                    }
                    return; 
                case SDLK_ESCAPE: 
                    nextState = GAMEPLAY;
                    return; 
            }
        }
    }
}

void handleMapSelectInput(SDL_Event &e, GameState &nextState, bool &running, int &selectedMap) {

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }
        if (e.type == SDL_KEYDOWN) {
            int oldSelectedMap = selectedMap;
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    if (selectedMap >= MAP_SELECT_COLS) 
                       selectedMap -= MAP_SELECT_COLS;
                    break;
                case SDLK_DOWN:
                     if (selectedMap < NUM_MAPS - MAP_SELECT_COLS) 
                       selectedMap += MAP_SELECT_COLS;
                    break;
                case SDLK_LEFT:
                    if (selectedMap % MAP_SELECT_COLS != 0) 
                        selectedMap--;
                    break;
                case SDLK_RIGHT:
                     if ((selectedMap + 1) % MAP_SELECT_COLS != 0 && selectedMap < NUM_MAPS - 1) 
                        selectedMap++;
                    break;
                case SDLK_RETURN:
                    nextState = GAMEPLAY; 
                    return; 
                case SDLK_ESCAPE: 
                    nextState = MAIN_MENU;
                    return; 
            }

             selectedMap = std::max(0, std::min(selectedMap, NUM_MAPS - 1));
        }
    }
}

void handleWinLoseInput(SDL_Event &e, GameState &currentState, GameState &nextState, bool &running, int &selectedItem) {

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedItem = (selectedItem - 1 + WIN_LOSE_MENU_ITEM_COUNT) % WIN_LOSE_MENU_ITEM_COUNT;
                    break;
                case SDLK_DOWN:
                    selectedItem = (selectedItem + 1) % WIN_LOSE_MENU_ITEM_COUNT;
                    break;
                case SDLK_RETURN:
                    if (selectedItem == 0) { 
                        nextState = GAMEPLAY;
                    } else if (selectedItem == 1) { 
                        nextState = MAP_SELECT;
                    } else { 
                        running = false;
                    }
                    return; 
                case SDLK_ESCAPE: 
                    running = false;
                    return; 
            }
        }
    }

}