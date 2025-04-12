#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "Constants.h"
#include "Texture.h"
#include "Renderer.h"
#include "Control.h"
#include "Init.h"
#include "Entity.h"
#include <SDL2/SDL.h> 
#include <vector>
#include <string> 
#include <fstream>
#include <iostream>
#include <cstdlib>


extern int highScore; 


void updateEntity(Entity &entity, std::vector<Entity> &enemies, std::vector<bool> &enemiesAlive,
                  bool &running, bool dropThrough, GameState &state,
                  std::vector<SDL_Rect> &stars,
                  std::vector<SDL_Rect>& coins, int& collectedCoins, int totalCoinsInMap,
                  const int tilemap[MAP_HEIGHT][MAP_WIDTH], int &score,int& playerHearts);

bool updateEnemy(Entity &enemy, bool isAlive, int cameraX, const int tilemap[MAP_HEIGHT][MAP_WIDTH]);
void respawnPlayer(Entity& mario);
void resetGameLevel(Entity& mario, std::vector<Entity>& enemies, std::vector<bool>& enemiesAlive,
    std::vector<SDL_Rect>& stars, std::vector<Cloud>& clouds,
    std::vector<SDL_Rect>& coins, int& collectedCoins, int& totalCoinsInMap,
    SDL_Texture* cloudTexture1, SDL_Texture* cloudTexture2,
    int& score, int currentMap, int& playerHearts);

#endif 