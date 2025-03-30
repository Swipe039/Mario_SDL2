#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "Entity.h"
#include "Constants.h"

void updateEntity(Entity &entity, std::vector<Entity> &enemies, std::vector<bool> &enemiesAlive, bool &running, bool dropThrough, int cameraX, GameState &state, std::vector<SDL_Rect> &stars, const int tilemap[MAP_HEIGHT][MAP_WIDTH], int &score);
void updateEnemy(Entity &enemy, bool isAlive, int cameraX, const int tilemap[MAP_HEIGHT][MAP_WIDTH]);
void renderMapSelect(SDL_Renderer *renderer, TTF_Font *font, int selectedMap);
void renderPauseMenu(SDL_Renderer *renderer, TTF_Font *font, int selectedItem);

#endif