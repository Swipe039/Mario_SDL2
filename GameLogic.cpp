#include "GameLogic.h"
#include "Constants.h"
#include <iostream>

void updateEntity(Entity &entity, std::vector<Entity> &enemies, std::vector<bool> &enemiesAlive, bool &running, bool dropThrough, int cameraX, GameState &state, std::vector<SDL_Rect> &stars, const int tilemap[MAP_HEIGHT][MAP_WIDTH], int &score) {
    // ... (existing code) ...    // Di chuyển thử theo trục X
    int newX = entity.rect.x + entity.velocityX;
    SDL_Rect futureXRect = {newX, entity.rect.y, entity.rect.w, entity.rect.h};

    // Va chạm với tilemap theo trục X
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (tilemap[y][x] == 1 || tilemap[y][x] == 3 || (tilemap[y][x] == 2 && !dropThrough && entity.velocityY >= 0)) {
                SDL_Rect tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                if (SDL_HasIntersection(&futureXRect, &tileRect)) {
                    if (entity.velocityX > 0) {
                        newX = tileRect.x - entity.rect.w;
                    } else if (entity.velocityX < 0) {
                        newX = tileRect.x + tileRect.w;
                    }
                    break;
                }
            }
        }
    }
    entity.rect.x = newX;

    // Giới hạn Mario trong bản đồ
    if (entity.rect.x < 0) entity.rect.x = 0;
    if (entity.rect.x + entity.rect.w > MAP_WIDTH * TILE_SIZE) entity.rect.x = MAP_WIDTH * TILE_SIZE - entity.rect.w;

    // Di chuyển theo trục Y
    entity.velocityY += GRAVITY;
    int newY = entity.rect.y + entity.velocityY;
    SDL_Rect futureYRect = {entity.rect.x, newY, entity.rect.w, entity.rect.h};

    // Va chạm với tilemap theo trục Y
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            SDL_Rect tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            if (tilemap[y][x] == 1 || tilemap[y][x] == 3) {
                if (SDL_HasIntersection(&futureYRect, &tileRect)) {
                    if (entity.velocityY > 0) {
                        newY = tileRect.y - entity.rect.h;
                        entity.velocityY = 0;
                        entity.onGround = true;
                    } else if (entity.velocityY < 0) {
                        newY = tileRect.y + tileRect.h;
                        entity.velocityY = 0;
                    }
                    break;
                }
            } else if (tilemap[y][x] == 2) {
                if (SDL_HasIntersection(&futureYRect, &tileRect)) {
                    if (entity.velocityY > 0 && !dropThrough) { // Rơi từ trên xuống
                        newY = tileRect.y - entity.rect.h;
                        entity.velocityY = 0;
                        entity.onGround = true;
                        std::cout << "Standing on fly block from above" << std::endl;
                        break;
                    } else if (entity.velocityY < 0) { // Nhảy từ dưới lên
                        int entityBottom = entity.rect.y + entity.rect.h; // Điểm chân hiện tại
                        int tileTop = tileRect.y; // Mép trên của block
                        int newEntityBottom = newY + entity.rect.h; // Điểm chân sau khi di chuyển

                        std::cout << "Jumping up: entityBottom=" << entityBottom << ", tileTop=" << tileTop << ", newEntityBottom=" << newEntityBottom << std::endl;

                        if (entityBottom <= tileTop && newEntityBottom > tileTop) {
                            // Chân vượt qua mép trên
                            newY = tileRect.y - entity.rect.h;
                            entity.velocityY = 0;
                            entity.onGround = true;
                            std::cout << "Standing on fly block from below" << std::endl;
                            break;
                        } else {
                            std::cout << "Passing through fly block" << std::endl;
                            // Không làm gì, để nhân vật xuyên qua
                        }
                    }
                }
            }
        }
    }

    entity.rect.y = newY;


    // Kiểm tra rơi xuống vực
    if (entity.rect.y > MAP_HEIGHT * TILE_SIZE) {
        std::cout << "Mario fell into the pit! Returning to menu." << std::endl;
        state = MAIN_MENU;
        entity.rect.x = SCREEN_WIDTH / 2;
        entity.rect.y = SCREEN_HEIGHT - 100;
        entity.velocityX = 0;
        entity.velocityY = 0;
        entity.onGround = true;
        entity.isBuffed = false;
        entity.rect.w = entity.originalW;
        entity.rect.h = entity.originalH;
    }

    // Cập nhật trạng thái hoạt ảnh
    entity.animTimer++;
    if (entity.animTimer >= entity.FRAME_DELAY) {
        entity.animTimer = 0;
        entity.animFrame++;
    }

    if (!entity.onGround) { // Nhảy
        entity.animState = Entity::JUMP;
        if (entity.animFrame >= 7) entity.animFrame = 0; // Jump_left_1 đến Jump_left_7 (hoặc Jump_right)
    } else if (entity.velocityX != 0) { // Chạy
        entity.animState = Entity::RUN;
        if (entity.animFrame >= 3) entity.animFrame = 0; // Run_left_1 đến Run_left_3 (hoặc Run_right)
        entity.direction = (entity.velocityX > 0) ? 1 : -1;
    } else { // Đứng yên
        entity.animState = Entity::STAND;
        entity.animFrame = 0; // Chỉ có 1 khung cho Stand_left hoặc Stand_right
    }

    // Kiểm tra va chạm với ngôi sao
    SDL_Rect marioRect = entity.getRect();
    for (size_t i = 0; i < stars.size(); i++) {
        if (SDL_HasIntersection(&marioRect, &stars[i])) {
            entity.isBuffed = true;
            entity.buffTimer = STAR_DURATION;
            entity.rect.w = entity.originalW * STAR_SCALE;
            entity.rect.h = entity.originalH * STAR_SCALE;
            entity.rect.y -= (entity.rect.h - entity.originalH);
            score += STAR_POINTS;
            stars.erase(stars.begin() + i);
            break;
        }
    }

    // Cập nhật thời gian buff
    if (entity.isBuffed) {
        entity.buffTimer--;
        if (entity.buffTimer <= 0) {
            entity.isBuffed = false;
            entity.rect.w = entity.originalW;
            entity.rect.h = entity.originalH;
            entity.rect.y += (entity.originalH * (STAR_SCALE - 1));
        }
    }

    // Kiểm tra va chạm với kẻ địch
    marioRect = entity.getRect();
    for (size_t i = 0; i < enemies.size(); i++) {
        if (enemiesAlive[i]) {
            SDL_Rect enemyRect = enemies[i].getRect();
            if (SDL_HasIntersection(&marioRect, &enemyRect)) {
                if (entity.isBuffed) {
                    if (entity.velocityY > 0 && marioRect.y + marioRect.h - enemyRect.y <= 10) {
                        enemiesAlive[i] = false;
                        entity.velocityY = JUMP_FORCE / 2;
                    } else {
                        enemies[i].direction *= -1;
                        enemies[i].rect.x += enemies[i].direction * ENEMY_SPEED * 5;
                    }
                } else {
                    std::cout << "Game Over! Mario collided with an enemy." << std::endl;
                    state = MAIN_MENU;
                    entity.rect.x = SCREEN_WIDTH / 2;
                    entity.rect.y = SCREEN_HEIGHT - 100;
                    entity.velocityX = 0;
                    entity.velocityY = 0;
                    entity.onGround = true;
                    entity.isBuffed = false;
                    entity.rect.w = entity.originalW;
                    entity.rect.h = entity.originalH;
                    break;
                }
            }
        }
    }

   
    // When defeating an enemy
    for (size_t i = 0; i < enemies.size(); i++) {
        if (enemiesAlive[i]) {
            SDL_Rect enemyRect = enemies[i].getRect();
            if (SDL_HasIntersection(&marioRect, &enemyRect)) {
                if (entity.isBuffed) {
                    if (entity.velocityY > 0 && marioRect.y + marioRect.h - enemyRect.y <= 10) {
                        enemiesAlive[i] = false;
                        entity.velocityY = JUMP_FORCE / 2;
                        score += ENEMY_POINTS; // Add points for enemy
                    } else {
                        enemies[i].direction *= -1;
                        enemies[i].rect.x += enemies[i].direction * ENEMY_SPEED * 5;
                    }
                } else {
                    // ... (existing game over logic) ...
                }
            }
        }
    }

    // ... (rest of the function) ...
}


void updateEnemy(Entity &enemy, bool isAlive, int cameraX, const int tilemap[MAP_HEIGHT][MAP_WIDTH]) {
    if (!isAlive) return;

    // Di chuyển thử theo trục X
    int newX = enemy.rect.x + enemy.direction * ENEMY_SPEED;
    SDL_Rect futureXRect = {newX, enemy.rect.y, enemy.rect.w, enemy.rect.h};

    // Va chạm với tilemap
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (tilemap[y][x] == 1 || tilemap[y][x] == 3) {
                SDL_Rect tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                if (SDL_HasIntersection(&futureXRect, &tileRect)) {
                    if (enemy.direction > 0) {
                        newX = tileRect.x - enemy.rect.w;
                        enemy.direction = -1;
                    } else if (enemy.direction < 0) {
                        newX = tileRect.x + tileRect.w;
                        enemy.direction = 1;
                    }
                    break;
                }
            }
        }
    }
    enemy.rect.x = newX;

    // Di chuyển theo trục Y
    enemy.velocityY += GRAVITY;
    int newY = enemy.rect.y + enemy.velocityY;
    SDL_Rect futureYRect = {enemy.rect.x, newY, enemy.rect.w, enemy.rect.h};
    bool yCollision = false;

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            SDL_Rect tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            if (tilemap[y][x] == 1 || tilemap[y][x] == 3) {
                if (SDL_HasIntersection(&futureYRect, &tileRect)) {
                    yCollision = true;
                    if (enemy.velocityY > 0) {
                        newY = tileRect.y - enemy.rect.h;
                        enemy.velocityY = 0;
                        enemy.onGround = true;
                    } else if (enemy.velocityY < 0) {
                        newY = tileRect.y + tileRect.h;
                        enemy.velocityY = 0;
                    }
                    break;
                }
            } else if (tilemap[y][x] == 2) {
                if (SDL_HasIntersection(&futureYRect, &tileRect) && enemy.velocityY > 0) {
                    yCollision = true;
                    newY = tileRect.y - enemy.rect.h;
                    enemy.velocityY = 0;
                    enemy.onGround = true;
                    break;
                }
            }
        }
    }

    if (!yCollision) {
        enemy.onGround = false;
    }

    enemy.rect.y = newY;

    // Kiểm tra rơi xuống vực
    if (enemy.rect.y > MAP_HEIGHT * TILE_SIZE) {
        std::cout << "Enemy fell into the pit and died!" << std::endl;
    }
}

void renderMapSelect(SDL_Renderer *renderer, TTF_Font *font, int selectedMap) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, "SELECT MAP", color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int textW, textH;
    SDL_QueryTexture(texture, nullptr, nullptr, &textW, &textH);
    SDL_Rect dstRect = {SCREEN_WIDTH/2 - textW/2, 50, textW, textH};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

    for (int i = 0; i < MAP_SELECT_ROWS * MAP_SELECT_COLS; i++) {
        int row = i / MAP_SELECT_COLS;
        int col = i % MAP_SELECT_COLS;
        int x = MAP_SELECT_START_X + col * (MAP_SELECT_BUTTON_SIZE + MAP_SELECT_MARGIN);
        int y = MAP_SELECT_START_Y + row * (MAP_SELECT_BUTTON_SIZE + MAP_SELECT_MARGIN);

        SDL_Rect rect = {x, y, MAP_SELECT_BUTTON_SIZE, MAP_SELECT_BUTTON_SIZE};
        
        if (i == selectedMap) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        }
        
        SDL_RenderFillRect(renderer, &rect);

        std::string mapText = "Map " + std::to_string(i+1);
        surface = TTF_RenderText_Solid(font, mapText.c_str(), color);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(texture, nullptr, nullptr, &textW, &textH);
        dstRect = {x + MAP_SELECT_BUTTON_SIZE/2 - textW/2, y + MAP_SELECT_BUTTON_SIZE/2 - textH/2, textW, textH};
        SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

    SDL_RenderPresent(renderer);
}

void renderPauseMenu(SDL_Renderer *renderer, TTF_Font *font, int selectedItem) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);

    const char *menuItems[MENU_ITEM_COUNT] = {"Continue", "Exit to Menu"};
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