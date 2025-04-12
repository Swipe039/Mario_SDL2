#include "GameLogic.h" 
#include "Constants.h"
#include "Texture.h"
#include "Renderer.h" 
#include <SDL2/SDL.h> 
#include <iostream>
#include <cmath>
#include <string> 

void renderMapSelect(SDL_Renderer *renderer, TTF_Font *font, int selectedMap) {

    if (mapSelectBackgroundTexture) {
        SDL_Rect backgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, mapSelectBackgroundTexture, nullptr, &backgroundRect);
    } else {

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255}; 

    SDL_Color ms_red = {242, 80, 34, 255};
    SDL_Color ms_green = {127, 186, 0, 255};
    SDL_Color ms_blue = {0, 164, 239, 255};
    SDL_Color ms_yellow = {255, 185, 0, 255};

    renderText(renderer, font, "SELECT MAP", SCREEN_WIDTH / 2, 50, white);

    for (int i = 0; i < NUM_MAPS; i++) {
        int row = i / MAP_SELECT_COLS;
        int col = i % MAP_SELECT_COLS;

        int x = MAP_SELECT_START_X + col * (MAP_SELECT_BUTTON_SIZE + MAP_SELECT_MARGIN);
        int y = MAP_SELECT_START_Y + row * (MAP_SELECT_BUTTON_SIZE + MAP_SELECT_MARGIN);
        SDL_Rect rect = {x, y, MAP_SELECT_BUTTON_SIZE, MAP_SELECT_BUTTON_SIZE};

        SDL_Color base_color;
        switch (i) {
            case 0: base_color = ms_red; break;
            case 1: base_color = ms_green; break;
            case 2: base_color = ms_blue; break;
            case 3: base_color = ms_yellow; break;
            default: base_color = {100, 100, 100, 255}; break; 
        }

        SDL_SetRenderDrawColor(renderer, base_color.r, base_color.g, base_color.b, base_color.a);
        SDL_RenderFillRect(renderer, &rect);

        if (i == selectedMap) {
            SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, white.a);

            SDL_Rect borderRect1 = {rect.x - 2, rect.y - 2, rect.w + 4, rect.h + 4};
            SDL_RenderDrawRect(renderer, &borderRect1);
            SDL_Rect borderRect2 = {rect.x - 1, rect.y - 1, rect.w + 2, rect.h + 2};
            SDL_RenderDrawRect(renderer, &borderRect2);
        }

        std::string mapText = std::to_string(i + 1);
        renderText(renderer, font, mapText, x + MAP_SELECT_BUTTON_SIZE / 2 + 1, y + MAP_SELECT_BUTTON_SIZE / 2 + 1, black); 
        renderText(renderer, font, mapText, x + MAP_SELECT_BUTTON_SIZE / 2, y + MAP_SELECT_BUTTON_SIZE / 2, white); 
    }

    SDL_RenderPresent(renderer); 
} 

void renderPauseMenu(SDL_Renderer *renderer, TTF_Font *font, int selectedItem) {

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180); 
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE); 

    const char *menuItems[PAUSE_MENU_ITEM_COUNT] = {"Continue", "Main Menu"};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255}; 

    renderText(renderer, font, "PAUSED", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4, yellow);

    for (int i = 0; i < PAUSE_MENU_ITEM_COUNT; i++) {
        SDL_Color color = (i == selectedItem) ? yellow : white; 
        renderText(renderer, font, menuItems[i], SCREEN_WIDTH / 2, MENU_START_Y + i * MENU_ITEM_SPACING, color);
    }

} 

void renderWinScreen(SDL_Renderer *renderer, TTF_Font *font, int score, int selectedItem) {

     SDL_SetRenderDrawColor(renderer, 30, 150, 30, 255);
     SDL_RenderClear(renderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    renderText(renderer, font, "YOU WIN!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4, yellow);
    renderText(renderer, font, "Score: " + std::to_string(score), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3, white);

    const char *menuItems[WIN_LOSE_MENU_ITEM_COUNT] = {"Retry", "Map Select", "Quit Game"};
    for (int i = 0; i < WIN_LOSE_MENU_ITEM_COUNT; i++) {
        SDL_Color color = (i == selectedItem) ? yellow : white;

        renderText(renderer, font, menuItems[i], SCREEN_WIDTH / 2, MENU_START_Y + 50 + i * MENU_ITEM_SPACING, color);
    }

    SDL_RenderPresent(renderer); 
} 

void renderLoseScreen(SDL_Renderer *renderer, TTF_Font *font, int score, int selectedItem) {

    SDL_SetRenderDrawColor(renderer, 150, 30, 30, 255);
    SDL_RenderClear(renderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    renderText(renderer, font, "GAME OVER", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4, yellow);
    renderText(renderer, font, "Score: " + std::to_string(score), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3, white);

    const char *menuItems[WIN_LOSE_MENU_ITEM_COUNT] = {"Retry", "Map Select", "Quit Game"};
     for (int i = 0; i < WIN_LOSE_MENU_ITEM_COUNT; i++) {
        SDL_Color color = (i == selectedItem) ? yellow : white;

        renderText(renderer, font, menuItems[i], SCREEN_WIDTH / 2, MENU_START_Y + 50 + i * MENU_ITEM_SPACING, color);
    }

    SDL_RenderPresent(renderer); 
} 

void renderMenu(SDL_Renderer *renderer, TTF_Font *font, int selectedItem) {

     if (mapSelectBackgroundTexture) { 
        SDL_Rect backgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, mapSelectBackgroundTexture, nullptr, &backgroundRect);
    } else { 
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    } 

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255}; 

    renderText(renderer, font, "MARIO SDL", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4, yellow);

    const char *menuItems[MENU_ITEM_COUNT] = {"Start Game", "Exit Game"};

    for (int i = 0; i < MENU_ITEM_COUNT; i++) { 
        SDL_Color color = (i == selectedItem) ? yellow : white; 
        renderText(renderer, font, menuItems[i], SCREEN_WIDTH / 2, MENU_START_Y + i * MENU_ITEM_SPACING, color);
    } 

    SDL_RenderPresent(renderer); 
} 