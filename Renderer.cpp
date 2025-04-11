#include "Renderer.h"
#include "Constants.h"
#include "GameLogic.h" // Cần thiết cho renderText
#include <string>
#include <iostream>

// <<< Cập nhật định nghĩa hàm render - THAY enemyTexture thành enemyTextures[2] >>>
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
{ // <<< Bắt đầu thân hàm

    // 1. Clear screen
    SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255); // Màu trời xanh
    SDL_RenderClear(renderer);

    // 2. Render Clouds (Parallax)
    for (const auto& cloud : clouds) {
        if (!cloud.texture) continue;
        int renderX = static_cast<int>(cloud.x - cameraX * CLOUD_PARALLAX_FACTOR);
        SDL_Rect cloudDestRect = {renderX, cloud.y, CLOUD_WIDTH, CLOUD_HEIGHT};
        // Chỉ render nếu mây có thể nhìn thấy trên màn hình
        if (cloudDestRect.x + cloudDestRect.w > 0 && cloudDestRect.x < SCREEN_WIDTH) {
            SDL_RenderCopy(renderer, cloud.texture, nullptr, &cloudDestRect);
        }
    }

    // 3. Render tilemap
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            SDL_Rect tileRect = {x * TILE_SIZE - cameraX, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            // Chỉ render nếu tile có thể nhìn thấy trên màn hình
            if (tileRect.x + tileRect.w < 0 || tileRect.x > SCREEN_WIDTH) continue;

            SDL_Texture* currentTileTexture = nullptr;
            int tileType = tilemaps[currentMap][y][x];

            if (tileType == GROUND_TILE) { currentTileTexture = groundTexture; }
            else if (tileType == FINISH_BLOCK_TILE) { currentTileTexture = finishLineTexture; }
            else if (tileType == FLY_BLOCK_TILE) { currentTileTexture = flyBlockTexture; }
            else if (tileType == DECORATIVE_TILE) {}
            else if (tileType == PIPE_TOP_TILE) { currentTileTexture = pipeTopTexture; }
            else if (tileType == PIPE_BODY_TILE) { currentTileTexture = pipeBodyTexture; }
            // Các tile khác (EMPTY, SPAWN, STAR, COIN) không cần vẽ texture map ở đây

            if (currentTileTexture) {
                SDL_RenderCopy(renderer, currentTileTexture, nullptr, &tileRect);
            }
        }
    }

    // 4. Render Stars (với hiệu ứng nhấp nháy đơn giản)
    static int starFlashTimer = 0;
    starFlashTimer = (starFlashTimer + 1) % 30; // Chu kỳ 30 frames
    Uint8 starAlpha = (starFlashTimer < 15) ? 255 : 150; // Nhấp nháy alpha

    for (const auto& star : stars) {
        SDL_Rect starRect = {star.x - cameraX, star.y, TILE_SIZE, TILE_SIZE};
        if (starRect.x + starRect.w < 0 || starRect.x > SCREEN_WIDTH) continue;
        if (starTexture) {
            SDL_SetTextureAlphaMod(starTexture, starAlpha); // Đặt alpha
            SDL_RenderCopy(renderer, starTexture, nullptr, &starRect);
            SDL_SetTextureAlphaMod(starTexture, 255); // Reset alpha về mặc định
        } else { // Fallback nếu không có texture
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, starAlpha);
            SDL_RenderFillRect(renderer, &starRect);
        }
    }

    // 4.5 Render COINS
    if (coinTexture) {
        for (const auto& coinRect : coins) {
            SDL_Rect renderRect = {coinRect.x - cameraX, coinRect.y, TILE_SIZE, TILE_SIZE};
            if (renderRect.x + renderRect.w > 0 && renderRect.x < SCREEN_WIDTH) {
                SDL_RenderCopy(renderer, coinTexture, nullptr, &renderRect);
            }
        }
    }

    // 5. Render Mario
    SDL_Rect marioRect = mario.getRect();
    marioRect.x -= cameraX;
    SDL_Texture *currentMarioTexture = nullptr;

    switch (mario.animState) {
        case Entity::JUMP:
             if (mario.animFrame < 0) mario.animFrame = 0;
             if (mario.animFrame >= 7) mario.animFrame = 6; // Giữ frame nhảy cuối
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
    } else { // Fallback nếu texture bị lỗi
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Màu đỏ cho Mario
        SDL_RenderFillRect(renderer, &marioRect);
    }

    // --- THAY ĐỔI: Render Enemies với Animation và Flip ---
    for (size_t i = 0; i < enemies.size(); i++) {
        if (enemiesAlive[i]) {
            SDL_Rect enemyRect = enemies[i].getRect();
            enemyRect.x -= cameraX;
            // Chỉ render nếu enemy có thể nhìn thấy trên màn hình
            if (enemyRect.x + enemyRect.w < 0 || enemyRect.x > SCREEN_WIDTH) continue;

            // Chọn texture dựa trên frame animation hiện tại của enemy
            // Đảm bảo enemyAnimFrame luôn là 0 hoặc 1
            int frameIndex = enemies[i].enemyAnimFrame % 2;
            SDL_Texture* currentEnemyTexture = enemyTextures[frameIndex];

            if (currentEnemyTexture) {
                // Xác định có lật ảnh không (nếu đang đi sang trái, direction = -1)
                SDL_RendererFlip flip = (enemies[i].direction == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

                // Sử dụng SDL_RenderCopyEx để vẽ và lật ảnh nếu cần
                SDL_RenderCopyEx(renderer, currentEnemyTexture, nullptr, &enemyRect, 0.0, nullptr, flip);
            } else {
                // Fallback: Vẽ hình chữ nhật nếu không có texture
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Màu xanh dương cho enemy
                SDL_RenderFillRect(renderer, &enemyRect);
            }
        }
    }
    // --- KẾT THÚC THAY ĐỔI ---


    // 7. Render UI Text (Score, Coins, etc.)
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    renderText(renderer, font, "Map: " + std::to_string(currentMap + 1), 70, 30, white);
    renderText(renderer, font, "Score: " + std::to_string(score), 70, 60, white);
    renderText(renderer, font, "Coins: " + std::to_string(collectedCoins), 70, 90, yellow); // Hiển thị số coin thu thập được
    if (mario.isBuffed) {
        renderText(renderer, font, "POWER UP!", SCREEN_WIDTH - 100, 30, yellow);
    }
    if (heartTexture) {
        int heartSize = 30; // Kích thước hiển thị của trái tim
        int heartPadding = 5; // Khoảng cách giữa các trái tim
        int startX = SCREEN_WIDTH - heartSize - 10; // Vị trí X của trái tim ngoài cùng bên phải
        int startY = 20; // Vị trí Y của hàng trái tim

        for (int i = 0; i < playerHearts; ++i) {
            SDL_Rect heartDestRect = { startX - i * (heartSize + heartPadding), startY, heartSize, heartSize };
            SDL_RenderCopy(renderer, heartTexture, nullptr, &heartDestRect);
        }
    }

} 



// Định nghĩa hàm renderMenu (Không thay đổi)
void renderMenu(SDL_Renderer *renderer, TTF_Font *font, int selectedItem) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); SDL_RenderClear(renderer);
    SDL_Color white = {255, 255, 255, 255}; SDL_Color yellow = {255, 255, 0, 255};
    renderText(renderer, font, "MARIO SDL", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4, yellow);
    const char *menuItems[MENU_ITEM_COUNT] = {"Start Game", "Exit Game"};
    for (int i = 0; i < MENU_ITEM_COUNT; i++) {
        SDL_Color color = (i == selectedItem) ? yellow : white;
        renderText(renderer, font, menuItems[i], SCREEN_WIDTH / 2, MENU_START_Y + i * MENU_ITEM_SPACING, color);
    }
    SDL_RenderPresent(renderer);
}
void renderText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, int x, int y, SDL_Color color) {
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