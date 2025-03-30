#include "Renderer.h"
#include "GameLogic.h"
#include <string>

void render(SDL_Renderer* renderer, Entity& mario, std::vector<Entity>& enemies, std::vector<bool>& enemiesAlive, int cameraX, std::vector<SDL_Rect>& stars, int currentMap, TTF_Font *font, SDL_Texture *groundTexture, SDL_Texture *flyBlockTexture, SDL_Texture *starTexture, SDL_Texture *enemyTexture,
            SDL_Texture *jumpLeftTextures[7], SDL_Texture *jumpRightTextures[7], SDL_Texture *runLeftTextures[3], SDL_Texture *runRightTextures[3], SDL_Texture *standLeftTexture, SDL_Texture *standRightTexture, int deltaY, int score) {

    // Xóa màn hình với màu nền xanh da trời
    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    SDL_RenderClear(renderer);

    // Render tilemap từ map hiện tại
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            SDL_Rect tileRect = {x * TILE_SIZE - cameraX, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            
            switch (tilemaps[currentMap][y][x]) {
                case 1: // Gạch nâu đậm - Sử dụng Groundblock.png
                    SDL_RenderCopy(renderer, groundTexture, nullptr, &tileRect);
                    break;
                case 2: // Gạch nâu nhạt (có thể xuyên qua từ dưới)
                    SDL_RenderCopy(renderer, flyBlockTexture, nullptr, &tileRect);
                    break;
                case 3: // Cây xanh
                    SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
                    SDL_RenderFillRect(renderer, &tileRect);
                    break;
                case 4: 
                    // Kẻ địch (được xử lý riêng)
                    break;
                case 5:
                    // Ngôi sao (được xử lý riêng)
                    break;
            }
        }
    }

    // Render các ngôi sao (Đã làm lại)
    static int flashTimer = 0;
    flashTimer = (flashTimer + 1) % 30; // Tạo nhịp nhấp nháy (chu kỳ 30 khung hình)
    Uint8 alpha = (flashTimer < 15) ? 255 : 150; // Độ trong suốt thay đổi giữa 255 (sáng) và 150 (mờ)

    for (const auto& star : stars) {
        SDL_Rect starRect = {star.x - cameraX, star.y, TILE_SIZE, TILE_SIZE};
        
        // Đặt độ trong suốt cho texture ngôi sao
        SDL_SetTextureAlphaMod(starTexture, alpha);
        SDL_RenderCopy(renderer, starTexture, nullptr, &starRect);
    }

    // Render Mario với hoạt ảnh
    SDL_Rect marioRect = mario.getRect();
    marioRect.x -= cameraX;
    SDL_Texture *currentTexture = nullptr;

    switch (mario.animState) {
        case Entity::JUMP:
            currentTexture = (mario.direction == 1) ? jumpRightTextures[mario.animFrame] : jumpLeftTextures[mario.animFrame];
            break;
        case Entity::RUN:
            currentTexture = (mario.direction == 1) ? runRightTextures[mario.animFrame] : runLeftTextures[mario.animFrame];
            break;
        case Entity::STAND:
            currentTexture = (mario.direction == 1) ? standRightTexture : standLeftTexture;
            break;
    }
    if (deltaY < 0) {
        currentTexture = (mario.direction == 1) ? jumpRightTextures[0] : jumpLeftTextures[0];
    }
    else if (deltaY > 0) {
        currentTexture = (mario.direction == 1) ? jumpRightTextures[6] : jumpLeftTextures[6];
    }

    if (currentTexture) {
        SDL_RenderCopy(renderer, currentTexture, nullptr, &marioRect);
    }

    // Render kẻ địch
    for (size_t i = 0; i < enemies.size(); i++) {
        if (enemiesAlive[i]) {
            SDL_Rect enemyRect = enemies[i].getRect();
            enemyRect.x -= cameraX;
            SDL_RenderCopy(renderer, enemyTexture, nullptr, &enemyRect);
        }
    }

    // Hiển thị thông tin map hiện tại
    SDL_Color textColor = {255, 255, 255, 255};
    std::string mapText = "Map: " + std::to_string(currentMap + 1);
    SDL_Surface* surface = TTF_RenderText_Solid(font, mapText.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    int textW, textH;
    SDL_QueryTexture(texture, nullptr, nullptr, &textW, &textH);
    SDL_Rect textRect = {10, 10, textW, textH};
    SDL_RenderCopy(renderer, texture, nullptr, &textRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    
    // Hiển thị điểm số
    std::string scoreText = "Điểm: " + std::to_string(score);
    SDL_Surface* surface_score = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Texture* texture_score = SDL_CreateTextureFromSurface(renderer, surface_score);
    int text_scoreW, text_scoreH;
    SDL_QueryTexture(texture_score, nullptr, nullptr, &text_scoreW, &text_scoreH);
    SDL_Rect scoreRect = {10, 40, text_scoreW, text_scoreH};
    SDL_RenderCopy(renderer, texture_score, nullptr, &scoreRect);
    SDL_DestroyTexture(texture_score);
    SDL_FreeSurface(surface_score);

    // Hiển thị trạng thái buff nếu có
    if (mario.isBuffed) {
        std::string buffText = "SỨC MẠNH!";
        surface = TTF_RenderText_Solid(font, buffText.c_str(), {255, 255, 0, 255});
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(texture, nullptr, nullptr, &textW, &textH);
        SDL_Rect buffRect = {SCREEN_WIDTH - textW - 10, 10, textW, textH};
        SDL_RenderCopy(renderer, texture, nullptr, &buffRect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

    SDL_RenderPresent(renderer);
}

void renderMenu(SDL_Renderer *renderer, TTF_Font *font, int selectedItem) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const char *menuItems[MENU_ITEM_COUNT] = {"Bắt đầu", "Thoát"};
    for (int i = 0; i < MENU_ITEM_COUNT; i++) {
        SDL_Color color = (i == selectedItem) ? SDL_Color{255, 255, 0, 255} : SDL_Color{255, 255, 255, 255};
        SDL_Surface *surface = TTF_RenderText_Solid(font, menuItems[i], color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        int textW, textH;
        SDL_QueryTexture(texture, nullptr, nullptr, &textW, &textH);
        SDL_Rect dstRect = {SCREEN_WIDTH / 2 - textW / 2, MENU_START_Y + i * MENU_ITEM_SPACING, textW, textH};
        SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

    SDL_RenderPresent(renderer);
}