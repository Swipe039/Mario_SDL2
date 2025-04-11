#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "Entity.h"
#include "Constants.h"

struct Cloud {
    float x;
    int y;
    SDL_Texture* texture;
};

// <<< Cập nhật chữ ký hàm render - THAY enemyTexture thành enemyTextures[2] >>>
void render(SDL_Renderer* renderer, Entity& mario, std::vector<Entity>& enemies, std::vector<bool>& enemiesAlive, int cameraX, std::vector<SDL_Rect>& stars,
    std::vector<Cloud>& clouds, SDL_Texture* cloudTexture1, SDL_Texture* cloudTexture2,
    SDL_Texture* pipeTopTexture, SDL_Texture* pipeBodyTexture,
    SDL_Texture* coinTexture, const std::vector<SDL_Rect>& coins,
    int collectedCoins,
    int currentMap, TTF_Font *font, SDL_Texture *groundTexture, SDL_Texture *flyBlockTexture, SDL_Texture *starTexture,
    SDL_Texture *enemyTextures[2],
    SDL_Texture *jumpLeftTextures[7], SDL_Texture *jumpRightTextures[7], SDL_Texture *runLeftTextures[3], SDL_Texture *runRightTextures[3],
    SDL_Texture *standLeftTexture, SDL_Texture *standRightTexture,
    SDL_Texture* finishLineTexture,
    int score,
    int playerHearts, SDL_Texture* heartTexture);

void renderMenu(SDL_Renderer *renderer, TTF_Font *font, int selectedItem);
void renderText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, int x, int y, SDL_Color color);
#endif