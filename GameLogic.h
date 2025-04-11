#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "GameLogic.h"
#include "Constants.h"
#include "Texture.h"
#include "Renderer.h"
#include "Control.h"
#include "Init.h"
#include "Entity.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

// <<< Cập nhật chữ ký hàm updateEntity >>>
void updateEntity(Entity &entity, std::vector<Entity> &enemies, std::vector<bool> &enemiesAlive,
                  bool &running, bool dropThrough, GameState &state,
                  std::vector<SDL_Rect> &stars, // Giữ lại stars nếu vẫn dùng
                  std::vector<SDL_Rect>& coins, int& collectedCoins, int totalCoinsInMap, // Thêm coin
                  const int tilemap[MAP_HEIGHT][MAP_WIDTH], int &score,int& playerHearts);

bool updateEnemy(Entity &enemy, bool isAlive, int cameraX, const int tilemap[MAP_HEIGHT][MAP_WIDTH]);
void renderMapSelect(SDL_Renderer *renderer, TTF_Font *font, int selectedMap);
void renderPauseMenu(SDL_Renderer *renderer, TTF_Font *font, int selectedItem);
void renderWinScreen(SDL_Renderer *renderer, TTF_Font *font, int score, int selectedItem);
void renderLoseScreen(SDL_Renderer *renderer, TTF_Font *font, int score, int selectedItem);
void respawnPlayer(Entity& mario); 
void resetGameLevel(Entity& mario, std::vector<Entity>& enemies, std::vector<bool>& enemiesAlive,
    std::vector<SDL_Rect>& stars, std::vector<Cloud>& clouds,
    std::vector<SDL_Rect>& coins, int& collectedCoins, int& totalCoinsInMap,
    SDL_Texture* cloudTexture1, SDL_Texture* cloudTexture2,
    int& score, int currentMap, int& playerHearts); 
#endif