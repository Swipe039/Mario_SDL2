#ifndef RENDERGAMESTATE_H
#define RENDERGAMESTATE_H
#include "GameLogic.h"
#include "Constants.h"
#include "Texture.h"
#include "Renderer.h"
#include "Control.h"
#include "Init.h"
#include "Entity.h"
#include <iostream>
#include <cmath> 

void renderMapSelect(SDL_Renderer *renderer, TTF_Font *font, int selectedMap);
void renderPauseMenu(SDL_Renderer *renderer, TTF_Font *font, int selectedItem);
void renderWinScreen(SDL_Renderer *renderer, TTF_Font *font, int score, int selectedItem);
void renderLoseScreen(SDL_Renderer *renderer, TTF_Font *font, int score, int selectedItem);
void renderMenu(SDL_Renderer *renderer, TTF_Font *font, int selectedItem);
#endif