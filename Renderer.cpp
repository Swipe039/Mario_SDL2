#include "Renderer.h"
#include "Constants.h"
#include "GameLogic.h"
#include <string>
#include <iostream>

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
    int playerHearts, SDL_Texture* heartTexture)
{

    SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
    SDL_RenderClear(renderer);

    for (const auto& cloud : clouds) {
        if (!cloud.texture) continue;
        int renderX = static_cast<int>(cloud.x - cameraX * CLOUD_PARALLAX_FACTOR);
        SDL_Rect cloudDestRect = {renderX, cloud.y, CLOUD_WIDTH, CLOUD_HEIGHT};
        if (cloudDestRect.x + cloudDestRect.w > 0 && cloudDestRect.x < SCREEN_WIDTH) {
            SDL_RenderCopy(renderer, cloud.texture, nullptr, &cloudDestRect);
        }
    }

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            SDL_Rect baseTileRect = {x * TILE_SIZE - cameraX, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            if (baseTileRect.x + baseTileRect.w < 0 || baseTileRect.x > SCREEN_WIDTH) continue;

            SDL_Texture* currentTileTexture = nullptr;
            SDL_Rect destinationRect = baseTileRect;

            int tileType = tilemaps[currentMap][y][x];

            if (tileType == FINISH_BLOCK_TILE) {
                currentTileTexture = finishLineTexture;
                destinationRect.h = TILE_SIZE * 5; 
                destinationRect.y -= TILE_SIZE * 4; 
            }
            else if (tileType == GROUND_TILE) {
                currentTileTexture = groundTexture;
            }
            else if (tileType == FLY_BLOCK_TILE) {
                currentTileTexture = flyBlockTexture;
            }
            else if (tileType == PIPE_TOP_TILE) {
                currentTileTexture = pipeTopTexture;
            }
            else if (tileType == PIPE_BODY_TILE) {
                currentTileTexture = pipeBodyTexture;
            }

            if (currentTileTexture) {
                SDL_RenderCopy(renderer, currentTileTexture, nullptr, &destinationRect);
            }
        }
    }

    static int starFlashTimer = 0;
    starFlashTimer = (starFlashTimer + 1) % 30;
    Uint8 starAlpha = (starFlashTimer < 15) ? 255 : 150;

    for (const auto& star : stars) {
        SDL_Rect starRect = {star.x - cameraX, star.y, TILE_SIZE, TILE_SIZE};
        if (starRect.x + starRect.w < 0 || starRect.x > SCREEN_WIDTH) continue;
        if (starTexture) {
            SDL_SetTextureAlphaMod(starTexture, starAlpha);
            SDL_RenderCopy(renderer, starTexture, nullptr, &starRect);
            SDL_SetTextureAlphaMod(starTexture, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, starAlpha);
            SDL_RenderFillRect(renderer, &starRect);
        }
    }

    if (coinTexture) {
        for (const auto& coinRect : coins) {
            SDL_Rect renderRect = {coinRect.x - cameraX, coinRect.y, TILE_SIZE, TILE_SIZE};
            if (renderRect.x + renderRect.w > 0 && renderRect.x < SCREEN_WIDTH) {
                SDL_RenderCopy(renderer, coinTexture, nullptr, &renderRect);
            }
        }
    }

    SDL_Rect marioRect = mario.getRect();
    marioRect.x -= cameraX;
    SDL_Texture *currentMarioTexture = nullptr;

    switch (mario.animState) {
        case Entity::JUMP:
             if (mario.animFrame < 0) mario.animFrame = 0;
             if (mario.animFrame >= 7) mario.animFrame = 6;
             currentMarioTexture = (mario.direction == 1) ? jumpRightTextures[mario.animFrame] : jumpLeftTextures[mario.animFrame];
             break;
        case Entity::RUN:
             if (mario.animFrame < 0 || mario.animFrame >= 3) mario.animFrame = 0;
             currentMarioTexture = (mario.direction == 1) ? runRightTextures[mario.animFrame] : runLeftTextures[mario.animFrame];
             break;
        case Entity::STAND:
        default:
             currentMarioTexture = (mario.direction == 1) ? standRightTexture : standLeftTexture;
             break;
    }

    if (currentMarioTexture) {
        SDL_RenderCopy(renderer, currentMarioTexture, nullptr, &marioRect);
    } else {

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &marioRect);
    }

    for (size_t i = 0; i < enemies.size(); i++) {
        if (enemiesAlive[i]) {
            SDL_Rect enemyRect = enemies[i].getRect();
            enemyRect.x -= cameraX;
            if (enemyRect.x + enemyRect.w < 0 || enemyRect.x > SCREEN_WIDTH) continue;

            int frameIndex = enemies[i].enemyAnimFrame % 2;
            SDL_Texture* currentEnemyTexture = enemyTextures[frameIndex];

            if (currentEnemyTexture) {
                SDL_RendererFlip flip = (enemies[i].direction == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                SDL_RenderCopyEx(renderer, currentEnemyTexture, nullptr, &enemyRect, 0.0, nullptr, flip);
            } else {

                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &enemyRect);
            }
        }
    }

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    renderText(renderer, font, "Map: " + std::to_string(currentMap + 1), 70, 30, white);
    renderText(renderer, font, "Coins: " + std::to_string(collectedCoins), 70, 60, yellow);

    renderText(renderer, font, "Score: " + std::to_string(score), SCREEN_WIDTH / 2, 30, white);

    if (heartTexture) {
        int heartSize = 30;
        int heartPadding = 5;

        int startX = SCREEN_WIDTH - heartSize - 10; 
        int startY = 30; 

        for (int i = 0; i < playerHearts; ++i) {
            SDL_Rect heartDestRect = { startX - i * (heartSize + heartPadding), startY, heartSize, heartSize };
            SDL_RenderCopy(renderer, heartTexture, nullptr, &heartDestRect);
        }
    }

}

void renderText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, int x, int y, SDL_Color color) {
    if (!font) {
         std::cerr << "renderText Error: Font is null!" << std::endl;
         return;
    }
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) { std::cerr << "TTF_RenderText_Solid error: " << TTF_GetError() << std::endl; return; }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) { std::cerr << "SDL_CreateTextureFromSurface error: " << SDL_GetError() << std::endl; SDL_FreeSurface(surface); return; }
    int textW, textH;
    SDL_QueryTexture(texture, nullptr, nullptr, &textW, &textH);
    SDL_Rect dstRect = {x - textW / 2, y - textH / 2, textW, textH}; 
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}