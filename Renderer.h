#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "Entity.h"
#include "GameLogic.h"
#include "Constants.h"

void render(SDL_Renderer* renderer, Entity& mario, std::vector<Entity>& enemies, std::vector<bool>& enemiesAlive, int cameraX, std::vector<SDL_Rect>& stars, int currentMap, TTF_Font *font, SDL_Texture *groundTexture, SDL_Texture *flyBlockTexture, SDL_Texture *starTexture, SDL_Texture *enemyTexture, SDL_Texture *jumpLeftTextures[7], SDL_Texture *jumpRightTextures[7], SDL_Texture *runLeftTextures[3], SDL_Texture *runRightTextures[3], SDL_Texture *standLeftTexture, SDL_Texture *standRightTexture, int deltaY, int score);
void renderMenu(SDL_Renderer *renderer, TTF_Font *font, int selectedItem);

#endif