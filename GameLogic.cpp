#include "GameLogic.h"
#include "Constants.h"
#include "Texture.h"
#include "Renderer.h"
#include "Control.h"
#include "Init.h"
#include "Entity.h"
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>


void updateEntity(Entity &entity, std::vector<Entity> &enemies, std::vector<bool> &enemiesAlive,
                  bool &running, bool dropThrough, GameState &state,
                  std::vector<SDL_Rect> &stars,
                  std::vector<SDL_Rect>& coins, int& collectedCoins, int totalCoinsInMap,
                  const int tilemap[MAP_HEIGHT][MAP_WIDTH], int &score, int& playerHearts) {
    int newX = entity.rect.x + entity.velocityX;
    SDL_Rect futureXRect = {newX, entity.rect.y, entity.rect.w, entity.rect.h};
    bool xCollision = false;
    for (int y = 0; y < MAP_HEIGHT && !xCollision; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tileType = tilemap[y][x];
            if (tileType == GROUND_TILE || tileType == FLY_BLOCK_TILE || tileType == DECORATIVE_TILE ||
                tileType == PIPE_TOP_TILE || tileType == PIPE_BODY_TILE) {
                SDL_Rect tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                if (SDL_HasIntersection(&futureXRect, &tileRect)) {
                    if (entity.velocityX > 0) { newX = tileRect.x - entity.rect.w; }
                    else if (entity.velocityX < 0) { newX = tileRect.x + tileRect.w; }
                    entity.velocityX = 0;
                    xCollision = true;
                    break;
                }
            }
        }
    }
    entity.rect.x = newX;
    if (entity.rect.x < 0) entity.rect.x = 0;
    if (entity.rect.x + entity.rect.w > MAP_WIDTH * TILE_SIZE) entity.rect.x = MAP_WIDTH * TILE_SIZE - entity.rect.w;

    entity.velocityY += GRAVITY;
    int newY = entity.rect.y + entity.velocityY;
    SDL_Rect futureYRect = {entity.rect.x, newY, entity.rect.w, entity.rect.h};
    bool yCollision = false;
    entity.onGround = false;
    for (int y = 0; y < MAP_HEIGHT && !yCollision; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tileType = tilemap[y][x];
            if (tileType == EMPTY_TILE || tileType == ENEMY_SPAWN_TILE || tileType == STAR_TILE || tileType == COIN_TILE || tileType == DECORATIVE_TILE) continue;
            SDL_Rect tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            if (SDL_HasIntersection(&futureYRect, &tileRect)) {
                if (tileType == GROUND_TILE || tileType == PIPE_TOP_TILE || tileType == PIPE_BODY_TILE) {
                    if (entity.velocityY >= 0) { newY = tileRect.y - entity.rect.h; entity.onGround = true; }
                    else { newY = tileRect.y + tileRect.h; }
                    entity.velocityY = 0; yCollision = true; break;
                } else if (tileType == FLY_BLOCK_TILE) {
                    if (entity.velocityY >= 0 && !dropThrough) { newY = tileRect.y - entity.rect.h; entity.onGround = true; entity.velocityY = 0; yCollision = true; break; }
                    else if (entity.velocityY < 0) { newY = tileRect.y + tileRect.h; entity.velocityY = 0; yCollision = true; break; }
                }
            }
        }
    }
    entity.rect.y = newY;

    SDL_Rect marioCurrentRect = entity.getRect();
    for (size_t i = 0; i < coins.size(); ) {
        if (SDL_HasIntersection(&marioCurrentRect, &coins[i])) {
            coins.erase(coins.begin() + i);
            collectedCoins++; score += COIN_POINTS;
            std::cout << "Coin collected! Total: " << collectedCoins << "/" << totalCoinsInMap << std::endl;
        } else { ++i; }
    }

    if (entity.rect.y > MAP_HEIGHT * TILE_SIZE + TILE_SIZE * 2) {
        playerHearts--;
        std::cout << "Lost a heart by falling! Hearts remaining: " << playerHearts << std::endl;
        if (playerHearts <= 0) { state = LOSE_SCREEN; return; }
        else { respawnPlayer(entity); }
    }

    Entity::AnimationState previousAnimState = entity.animState;
    if (!entity.onGround) { entity.animState = Entity::JUMP; }
    else if (entity.velocityX != 0) { entity.animState = Entity::RUN; }
    else { entity.animState = Entity::STAND; }
    if (entity.animState != previousAnimState) { entity.animFrame = 0; entity.animTimer = 0; }
    else {
        entity.animTimer++;
        if (entity.animTimer > entity.FRAME_DELAY) {
            entity.animTimer = 0; entity.animFrame++;
            switch (entity.animState) {
                case Entity::RUN: if (entity.animFrame >= 3) entity.animFrame = 0; break;
                case Entity::JUMP: if (entity.animFrame >= 7) entity.animFrame = 6; break;
                case Entity::STAND: default: entity.animFrame = 0; break;
            }
        }
    }
    if (entity.velocityX > 0) entity.direction = 1; else if (entity.velocityX < 0) entity.direction = -1;

    SDL_Rect starMarioRect = entity.getRect();
    for (size_t i = 0; i < stars.size(); ) {
        if (SDL_HasIntersection(&starMarioRect, &stars[i])) {
            entity.isBuffed = true; entity.buffTimer = STAR_DURATION;
            if (!entity.wasBuffed) {
                int oldH = entity.rect.h;
                entity.rect.w = static_cast<int>(entity.originalW * STAR_SCALE);
                entity.rect.h = static_cast<int>(entity.originalH * STAR_SCALE);
                entity.rect.y -= (entity.rect.h - oldH); entity.wasBuffed = true;
            }
            score += STAR_POINTS;
            Mix_PlayChannel(-1, buffSound, 0);
            stars.erase(stars.begin() + i);
        } else { ++i; }
    }
    if (entity.isBuffed) {
        entity.buffTimer--;
        if (entity.buffTimer <= 0) {
            entity.isBuffed = false;
            if (entity.wasBuffed) {
                int oldH = entity.rect.h;
                entity.rect.w = entity.originalW; entity.rect.h = entity.originalH;
                entity.rect.y += (oldH - entity.rect.h); entity.wasBuffed = false;
            }
        }
    }

    SDL_Rect enemyMarioRect = entity.getRect();
    for (size_t i = 0; i < enemies.size(); i++) {
        if (enemiesAlive[i]) {
            SDL_Rect enemyRect = enemies[i].getRect();
            if (SDL_HasIntersection(&enemyMarioRect, &enemyRect)) {
                bool stomp = entity.velocityY > 0 && (enemyMarioRect.y + enemyMarioRect.h) >= enemyRect.y && (enemyMarioRect.y + enemyMarioRect.h) <= (enemyRect.y + enemyRect.h / 2);
                if (stomp) {
                    enemiesAlive[i] = false; entity.velocityY = JUMP_FORCE / 1.5; score += ENEMY_POINTS;
                } else if (entity.isBuffed) {
                    enemiesAlive[i] = false; score += ENEMY_POINTS;
                } else {
                    playerHearts--;
                    if (playerHearts <= 0) { state = LOSE_SCREEN; return; }
                    else { respawnPlayer(entity); break; }
                }
            }
        }
    }
    if (state == LOSE_SCREEN) return;

    if ( state == GAMEPLAY) {
        int footX = entity.rect.x + entity.rect.w / 2;
        int footY = entity.rect.y + entity.rect.h ;
        int tileX = footX / TILE_SIZE; int tileY = footY / TILE_SIZE;
        if (tileX >= 0 && tileX < MAP_WIDTH && tileY >= 0 && tileY < MAP_HEIGHT) {
            if (tilemap[tileY][tileX] == FINISH_BLOCK_TILE) {
                bool coinRequirementMet = (totalCoinsInMap == 0) || (collectedCoins * 3 >= totalCoinsInMap * 2);
                if (coinRequirementMet) {
                    state = WIN_SCREEN;
                }
            }
        }
    }
}

bool updateEnemy(Entity &enemy, bool isAlive, int cameraX, const int tilemap[MAP_HEIGHT][MAP_WIDTH]) {
    if (!isAlive) return false;

    int newX = enemy.rect.x + enemy.direction * ENEMY_SPEED;
    SDL_Rect futureXRect = {newX, enemy.rect.y, enemy.rect.w, enemy.rect.h};
    bool xCollision = false;
    for (int y = 0; y < MAP_HEIGHT && !xCollision; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tileType = tilemap[y][x];
            if (tileType == GROUND_TILE || tileType == FLY_BLOCK_TILE || tileType == DECORATIVE_TILE ||
                tileType == FINISH_BLOCK_TILE || tileType == PIPE_TOP_TILE || tileType == PIPE_BODY_TILE) {
                SDL_Rect tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                if (SDL_HasIntersection(&futureXRect, &tileRect)) {
                    if (enemy.direction > 0) newX = tileRect.x - enemy.rect.w;
                    else if (enemy.direction < 0) newX = tileRect.x + tileRect.w;
                    enemy.direction *= -1; xCollision = true; break;
                }
            }
        }
    }

    if (!xCollision && enemy.onGround) {
        int groundCheckTileX = (enemy.direction > 0) ? (newX + enemy.rect.w) / TILE_SIZE : newX / TILE_SIZE;
        int groundCheckTileY = (enemy.rect.y + enemy.rect.h + 5) / TILE_SIZE;
        bool foundGroundAhead = false;
        if (groundCheckTileY >= 0 && groundCheckTileY < MAP_HEIGHT && groundCheckTileX >= 0 && groundCheckTileX < MAP_WIDTH) {
            int aheadTile = tilemap[groundCheckTileY][groundCheckTileX];
            if (aheadTile == GROUND_TILE || aheadTile == FLY_BLOCK_TILE || aheadTile == DECORATIVE_TILE ||
                aheadTile == FINISH_BLOCK_TILE || aheadTile == PIPE_TOP_TILE || aheadTile == PIPE_BODY_TILE) {
                foundGroundAhead = true;
            }
        }
        if (!foundGroundAhead) { enemy.direction *= -1; newX = enemy.rect.x; }
    }
    enemy.rect.x = newX;

    enemy.velocityY += GRAVITY;
    int newY = enemy.rect.y + enemy.velocityY;
    SDL_Rect futureYRect = {enemy.rect.x, newY, enemy.rect.w, enemy.rect.h};
    bool yCollision = false;
    enemy.onGround = false;
    for (int y = 0; y < MAP_HEIGHT && !yCollision; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tileType = tilemap[y][x];
            if (tileType == GROUND_TILE || tileType == FLY_BLOCK_TILE || tileType == FINISH_BLOCK_TILE || tileType == PIPE_TOP_TILE || tileType == PIPE_BODY_TILE) {
                SDL_Rect tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                if (SDL_HasIntersection(&futureYRect, &tileRect)) {
                    if (enemy.velocityY >= 0) { newY = tileRect.y - enemy.rect.h; enemy.onGround = true; }
                    else { newY = tileRect.y + tileRect.h; }
                    enemy.velocityY = 0; yCollision = true; break;
                }
            }
        }
    }
    enemy.rect.y = newY;

    enemy.enemyAnimTimer++;
    if (enemy.enemyAnimTimer > ENEMY_FRAME_DELAY) {
        enemy.enemyAnimTimer = 0;
        enemy.enemyAnimFrame = (enemy.enemyAnimFrame + 1) % 2;
    }

    if (enemy.rect.y > MAP_HEIGHT * TILE_SIZE + TILE_SIZE * 2) return false;

    return true;
}

void resetGameLevel(Entity& mario, std::vector<Entity>& enemies, std::vector<bool>& enemiesAlive,
    std::vector<SDL_Rect>& stars, std::vector<Cloud>& clouds,
    std::vector<SDL_Rect>& coins, int& collectedCoins, int& totalCoinsInMap,
    SDL_Texture* cloudTexture1, SDL_Texture* cloudTexture2,
    int& score, int currentMap, int& playerHearts) {

    respawnPlayer(mario);
    enemies.clear(); enemiesAlive.clear(); stars.clear(); clouds.clear(); coins.clear();
    collectedCoins = 0; totalCoinsInMap = 0;
    playerHearts = MAX_HEARTS;

    SDL_Texture* cloudTextures[] = {cloudTexture1, cloudTexture2};
    float mapVisualWidth = MAP_WIDTH * TILE_SIZE * 1.1f;
    for (int i = 0; i < NUM_CLOUDS_PER_MAP; ++i) {
        if (!cloudTexture1 || !cloudTexture2) break;
        Cloud cloud; int texIndex = rand() % 2; cloud.texture = cloudTextures[texIndex];
        cloud.x = static_cast<float>(rand() % static_cast<int>(mapVisualWidth));
        cloud.y = 50 + (rand() % (SCREEN_HEIGHT / 3 - 50)); clouds.push_back(cloud);
    }

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tileType = tilemaps[currentMap][y][x];
            if (tileType == ENEMY_SPAWN_TILE) {
                int enemyY = y * TILE_SIZE; bool enemyGroundFound = false;
                for(int checkY = y; checkY < MAP_HEIGHT; ++checkY){
                    int checkTile = tilemaps[currentMap][checkY][x];
                    if(checkTile != EMPTY_TILE && checkTile != ENEMY_SPAWN_TILE && checkTile != STAR_TILE && checkTile != COIN_TILE){
                        enemyY = checkY * TILE_SIZE - 50;
                        enemyGroundFound = true; break;
                    }
                }
                if (!enemyGroundFound) enemyY = y * TILE_SIZE;

                Entity enemy(x * TILE_SIZE, enemyY, 50, 50, 0, 0, false);
                enemy.direction = (rand() % 2 == 0) ? 1 : -1;
                enemy.enemyAnimFrame = 0; enemy.enemyAnimTimer = 0;
                enemies.push_back(enemy); enemiesAlive.push_back(true);
            } else if (tileType == STAR_TILE) {
                SDL_Rect star = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}; stars.push_back(star);
            } else if (tileType == COIN_TILE) {
                SDL_Rect coin = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}; coins.push_back(coin); totalCoinsInMap++;
            }
        }
    }
}

void respawnPlayer(Entity& mario) {
    mario.rect.x = 100;
    mario.rect.y = 100;
    mario.velocityX = 0;
    mario.velocityY = 0;
    mario.onGround = false;
    mario.isBuffed = false;
    mario.buffTimer = 0;
    if (mario.wasBuffed) {
        mario.rect.w = mario.originalW;
        mario.rect.h = mario.originalH;
        mario.wasBuffed = false;
    }
    mario.animState = Entity::STAND;
    mario.animFrame = 0;
    mario.animTimer = 0;
    mario.direction = 1;
    std::cout << "Player respawned." << std::endl;
}