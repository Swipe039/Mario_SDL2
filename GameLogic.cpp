#include "GameLogic.h"
#include "Constants.h"
#include "Texture.h"
#include "Renderer.h"
#include "Control.h"
#include "Init.h"
#include "Entity.h"
#include <iostream>
#include <cmath> 
void updateEntity(Entity &entity, std::vector<Entity> &enemies, std::vector<bool> &enemiesAlive,
    bool &running, bool dropThrough, GameState &state,
    std::vector<SDL_Rect> &stars,
    std::vector<SDL_Rect>& coins, int& collectedCoins, int totalCoinsInMap,
    const int tilemap[MAP_HEIGHT][MAP_WIDTH], int &score, int& playerHearts) {  
    int originalX = entity.rect.x;
    int newX = entity.rect.x + entity.velocityX;
    SDL_Rect futureXRect = {newX, entity.rect.y, entity.rect.w, entity.rect.h};
    bool xCollision = false;
    for (int y = 0; y < MAP_HEIGHT && !xCollision; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tileType = tilemap[y][x];
            // Kiểm tra va chạm X với khối rắn: Ground(1), Fly(2), Deco(3), Finish(9), Pipe(6,7)
            if (tileType == GROUND_TILE || tileType == FLY_BLOCK_TILE || tileType == DECORATIVE_TILE ||
                tileType == FINISH_BLOCK_TILE || tileType == PIPE_TOP_TILE || tileType == PIPE_BODY_TILE)
            {
                SDL_Rect tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                if (SDL_HasIntersection(&futureXRect, &tileRect)) {
                    // std::cout << "!! X Collision: Mario futureX=" << newX << " intersects Tile (" << x << "," << y << ") type=" << tileType << std::endl;
                    if (entity.velocityX > 0) { newX = tileRect.x - entity.rect.w; }
                    else if (entity.velocityX < 0) { newX = tileRect.x + tileRect.w; }
                    entity.velocityX = 0; // Dừng vận tốc ngang
                    xCollision = true;
                    break;
                }
            }
        }
    }
    entity.rect.x = newX;
   
    if (entity.rect.x < 0) entity.rect.x = 0;
    if (entity.rect.x + entity.rect.w > MAP_WIDTH * TILE_SIZE) entity.rect.x = MAP_WIDTH * TILE_SIZE - entity.rect.w;


    // --- Logic va chạm Y và Gravity ---
    entity.velocityY += GRAVITY; // Luôn áp dụng trọng lực
    int originalY = entity.rect.y;
    int newY = entity.rect.y + entity.velocityY;
    SDL_Rect futureYRect = {entity.rect.x, newY, entity.rect.w, entity.rect.h};
    bool yCollision = false;
    entity.onGround = false; // Reset onGround trước khi kiểm tra va chạm Y

    for (int y = 0; y < MAP_HEIGHT && !yCollision; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tileType = tilemap[y][x];
            // Bỏ qua các tile không cần kiểm tra va chạm map Y
            if (tileType == EMPTY_TILE || tileType == ENEMY_SPAWN_TILE || tileType == STAR_TILE || tileType == COIN_TILE || tileType == DECORATIVE_TILE) continue; // Bỏ qua cả Decorative cho Y collision

            SDL_Rect tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            bool checkIntersection = SDL_HasIntersection(&futureYRect, &tileRect);

            if (checkIntersection) {
                 // std::cout << "!! Y Collision: Mario futureY=" << newY << " VelY=" << entity.velocityY << " intersects Tile (" << x << "," << y << ") type=" << tileType << std::endl;
                // Khối rắn: Ground(1), Finish(9), Pipe(6,7) - Decorative(3) không chặn Y
                if (tileType == GROUND_TILE || tileType == FINISH_BLOCK_TILE ||
                    tileType == PIPE_TOP_TILE || tileType == PIPE_BODY_TILE)
                {
                    if (entity.velocityY >= 0) { // Rơi hoặc đứng yên giao nhau
                        newY = tileRect.y - entity.rect.h;
                        entity.onGround = true;
                        // std::cout << "    -> Landed on Solid. Set onGround=true." << std::endl;
                    } else { // Đập đầu
                        newY = tileRect.y + tileRect.h;
                         // std::cout << "    -> Hit head on Solid." << std::endl;
                    }
                    entity.velocityY = 0;
                    yCollision = true;
                    break;
                }
                // Khối bay (2)
                else if (tileType == FLY_BLOCK_TILE) {
                    if (entity.velocityY >= 0 && !dropThrough) { // Rơi và không drop
                        newY = tileRect.y - entity.rect.h;
                        entity.onGround = true;
                        entity.velocityY = 0;
                        yCollision = true;
                         // std::cout << "    -> Landed on Fly Block. Set onGround=true." << std::endl;
                        break;
                    } else if (entity.velocityY < 0) { // Đập đầu
                        newY = tileRect.y + tileRect.h;
                        entity.velocityY = 0;
                        yCollision = true;
                         // std::cout << "    -> Hit head on Fly Block." << std::endl;
                        break;
                    }
                }
            } // end if(checkIntersection)
        } // end for x
    } // end for y
    entity.rect.y = newY; // Cập nhật vị trí Y cuối cùng
    // if(originalY != newY) std::cout << "  Y Adjusted: Old=" << originalY << " New=" << newY << " OnGround=" << entity.onGround << std::endl;
    // else if (!yCollision) std::cout << "  No Y Collision Detected. Current Y=" << entity.rect.y << " VelY=" << entity.velocityY << std::endl;


    // --- THU THẬP COIN ---
    SDL_Rect marioCurrentRect = entity.getRect();
    for (size_t i = 0; i < coins.size(); ) {
        if (SDL_HasIntersection(&marioCurrentRect, &coins[i])) {
            coins.erase(coins.begin() + i);
            collectedCoins++;
            score += COIN_POINTS;
            std::cout << "Coin collected! Total: " << collectedCoins << "/" << totalCoinsInMap << std::endl;
            // Play sound effect here (optional)
        } else {
            ++i; // Chỉ tăng i nếu không xóa coin
        }
    }


    // Check Fall Death
    if (entity.rect.y > MAP_HEIGHT * TILE_SIZE + TILE_SIZE * 2) {
        playerHearts--; // Trừ mạng
        std::cout << "Lost a heart by falling! Hearts remaining: " << playerHearts << std::endl;
        if (playerHearts <= 0) {
            std::cout << "Game Over! Mario fell (no hearts left)." << std::endl;
            state = LOSE_SCREEN; // Hết mạng -> Thua
            return;
        } else {
            respawnPlayer(entity); // Còn mạng -> Hồi sinh
            // Không cần return, tiếp tục vòng lặp game
        }
   }

    // Update Animation State
    Entity::AnimationState previousAnimState = entity.animState;
    if (!entity.onGround) { entity.animState = Entity::JUMP;}
    else if (entity.velocityX != 0) { entity.animState = Entity::RUN; }
    else { entity.animState = Entity::STAND;}
    if (entity.animState != previousAnimState) { entity.animFrame = 0; entity.animTimer = 0;}
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


    // Star Collision & Buff (Giữ lại nếu bạn vẫn muốn có sao)
    SDL_Rect starMarioRect = entity.getRect(); // Lấy rect mới nhất phòng khi buff thay đổi size
    for (size_t i = 0; i < stars.size(); ) {
        if (SDL_HasIntersection(&starMarioRect, &stars[i])) {
            entity.isBuffed = true;
            entity.buffTimer = STAR_DURATION;
             if (!entity.wasBuffed) {
                int oldH = entity.rect.h;
                entity.rect.w = static_cast<int>(entity.originalW * STAR_SCALE);
                entity.rect.h = static_cast<int>(entity.originalH * STAR_SCALE);
                entity.rect.y -= (entity.rect.h - oldH); // Điều chỉnh Y để không bị lún
                entity.wasBuffed = true;
            }
            score += STAR_POINTS;
            stars.erase(stars.begin() + i);
             std::cout << "DEBUG: Star collected. Remaining stars: " << stars.size() << std::endl;
        } else {
            ++i;
        }
    }
     if (entity.isBuffed) {
        entity.buffTimer--;
        if (entity.buffTimer <= 0) {
            entity.isBuffed = false;
             if (entity.wasBuffed) { 
                 int oldH = entity.rect.h;
                 entity.rect.w = entity.originalW;
                 entity.rect.h = entity.originalH;
                 entity.rect.y += (oldH - entity.rect.h); 
                 entity.wasBuffed = false;
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
                    // ... (logic dẫm lên địch giữ nguyên) ...
                } else if (entity.isBuffed) {
                    // ... (logic va chạm khi có buff giữ nguyên) ...
                } else { // Va chạm ngang khi không buff
                     playerHearts--; // Trừ mạng
                     std::cout << "Lost a heart by collision! Hearts remaining: " << playerHearts << std::endl;
                     if (playerHearts <= 0) {
                         std::cout << "Game Over! Mario collided with enemy (no hearts left)." << std::endl;
                         state = LOSE_SCREEN; // Hết mạng -> Thua
                         return; // Thoát khỏi updateEntity ngay
                     } else {
                         respawnPlayer(entity); // Còn mạng -> Hồi sinh
                         // Sau khi hồi sinh, nên bỏ qua việc kiểm tra va chạm với các quái khác trong frame này
                         // để tránh mất nhiều mạng liên tục nếu hồi sinh ngay chỗ đông quái.
                         // Có thể thêm cờ invincible hoặc break vòng lặp for này.
                         // Ví dụ đơn giản là break:
                         break; // Thoát vòng lặp kiểm tra quái vật sau khi hồi sinh
                     }
                }
            }
        }
    }
    // Nếu đã break khỏi vòng lặp trên do hồi sinh, cần đảm bảo không chạy code kiểm tra thắng ngay sau đó
    if (state == LOSE_SCREEN) return; // Nếu đã thua thì không kiểm tra thắng nữa


    // --- KIỂM TRA ĐIỀU KIỆN THẮNG (MỚI) ---
    if (entity.onGround && state == GAMEPLAY) { // Phải đang trên mặt đất và đang chơi
        int footX = entity.rect.x + entity.rect.w / 2;
        int footY = entity.rect.y + entity.rect.h + 1; // Điểm kiểm tra ngay dưới chân
        int tileX = footX / TILE_SIZE;
        int tileY = footY / TILE_SIZE;

        if (tileX >= 0 && tileX < MAP_WIDTH && tileY >= 0 && tileY < MAP_HEIGHT) {
            if (tilemap[tileY][tileX] == FINISH_BLOCK_TILE) { // Đứng trên khối kết thúc?
                // Kiểm tra điều kiện coin (>= 2/3)
                bool coinRequirementMet = (totalCoinsInMap == 0) || (collectedCoins * 3 >= totalCoinsInMap * 2);

                if (coinRequirementMet) {
                    std::cout << "You won! Coins: " << collectedCoins << "/" << totalCoinsInMap << ", On Finish Block." << std::endl;
                    state = WIN_SCREEN; // Chuyển trạng thái thắng
                } else {
                    // Tùy chọn: std::cout << "Need more coins! (" << collectedCoins << "/" << totalCoinsInMap << ")" << std::endl;
                }
            }
        }
    }
}



bool updateEnemy(Entity &enemy, bool isAlive, int cameraX, const int tilemap[MAP_HEIGHT][MAP_WIDTH]) {
     if (!isAlive) return false;

    // Logic di chuyển và va chạm X
    int newX = enemy.rect.x + enemy.direction * ENEMY_SPEED;
    SDL_Rect futureXRect = {newX, enemy.rect.y, enemy.rect.w, enemy.rect.h};
    bool xCollision = false;
    bool foundGroundAhead = false;
    int groundCheckTileX = -1;
    int groundCheckTileY = (enemy.rect.y + enemy.rect.h + 5) / TILE_SIZE;

    for (int y = 0; y < MAP_HEIGHT && !xCollision; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tileType = tilemap[y][x];
             // Kẻ địch va chạm X với Ground(1), Fly(2), Deco(3), Finish(9), Pipe(6,7)
             if (tileType == GROUND_TILE || tileType == FLY_BLOCK_TILE || tileType == DECORATIVE_TILE ||
                 tileType == FINISH_BLOCK_TILE || tileType == PIPE_TOP_TILE || tileType == PIPE_BODY_TILE)
             {
                SDL_Rect tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                if (SDL_HasIntersection(&futureXRect, &tileRect)) {
                    if (enemy.direction > 0) newX = tileRect.x - enemy.rect.w;
                    else if (enemy.direction < 0) newX = tileRect.x + tileRect.w;
                    enemy.direction *= -1;
                    xCollision = true;
                    break;
                }
            }
        }
    }

     // Kiểm tra vực thẳm phía trước
     if (!xCollision && enemy.onGround) {
        groundCheckTileX = (enemy.direction > 0) ? (newX + enemy.rect.w) / TILE_SIZE : newX / TILE_SIZE;
        if (groundCheckTileY >= 0 && groundCheckTileY < MAP_HEIGHT && groundCheckTileX >= 0 && groundCheckTileX < MAP_WIDTH) {
             int aheadTile = tilemap[groundCheckTileY][groundCheckTileX];
             // Có đất/vật cản phía trước không?
             if (aheadTile == GROUND_TILE || aheadTile == FLY_BLOCK_TILE || aheadTile == DECORATIVE_TILE ||
                 aheadTile == FINISH_BLOCK_TILE || aheadTile == PIPE_TOP_TILE || aheadTile == PIPE_BODY_TILE)
             {
                 foundGroundAhead = true;
             }
        }
         if (!foundGroundAhead) { enemy.direction *= -1; newX = enemy.rect.x; }
    }
    enemy.rect.x = newX;


    // Logic rơi và va chạm Y
    enemy.velocityY += GRAVITY;
    int newY = enemy.rect.y + enemy.velocityY;
    SDL_Rect futureYRect = {enemy.rect.x, newY, enemy.rect.w, enemy.rect.h};
    bool yCollision = false;
    enemy.onGround = false;

    for (int y = 0; y < MAP_HEIGHT && !yCollision; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tileType = tilemap[y][x];
             // Kẻ địch va chạm Y với Ground(1), Fly(2), Finish(9), Pipe(6,7) (Deco(3) không chặn Y)
             if (tileType == GROUND_TILE || tileType == FLY_BLOCK_TILE ||
                 tileType == FINISH_BLOCK_TILE || tileType == PIPE_TOP_TILE || tileType == PIPE_BODY_TILE)
             {
                SDL_Rect tileRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                if (SDL_HasIntersection(&futureYRect, &tileRect)) {
                    if (enemy.velocityY >= 0) { // Đang rơi
                        newY = tileRect.y - enemy.rect.h;
                        enemy.onGround = true;
                    } else { // Đập đầu
                        newY = tileRect.y + tileRect.h;
                    }
                    enemy.velocityY = 0;
                    yCollision = true;
                    break;
                }
            }
        }
    }
    enemy.rect.y = newY;
    enemy.enemyAnimTimer++; // Tăng bộ đếm thời gian
    if (enemy.enemyAnimTimer > ENEMY_FRAME_DELAY) { // Nếu đủ thời gian để chuyển frame
        enemy.enemyAnimTimer = 0; // Reset bộ đếm
        enemy.enemyAnimFrame = (enemy.enemyAnimFrame + 1) % 2; // Chuyển giữa frame 0 và 1
    }
    // Check Fall Death
    if (enemy.rect.y > MAP_HEIGHT * TILE_SIZE + TILE_SIZE * 2) return false;

    return true; // Enemy còn sống
    
}
void resetGameLevel(Entity& mario, std::vector<Entity>& enemies, std::vector<bool>& enemiesAlive,
    std::vector<SDL_Rect>& stars, std::vector<Cloud>& clouds,
    std::vector<SDL_Rect>& coins, int& collectedCoins, int& totalCoinsInMap,
    SDL_Texture* cloudTexture1, SDL_Texture* cloudTexture2,
    int& score, int currentMap, int& playerHearts)
{
// Reset Mario (Spawn cố định)
    respawnPlayer(mario);
    enemies.clear(); enemiesAlive.clear(); stars.clear(); clouds.clear(); coins.clear();
    collectedCoins = 0; totalCoinsInMap = 0;

    playerHearts = MAX_HEARTS; 
enemies.clear(); enemiesAlive.clear(); stars.clear(); clouds.clear(); coins.clear();
collectedCoins = 0; totalCoinsInMap = 0; // Reset score ở đây hoặc nơi gọi resetGameLevel
// score = 0; // Thường score chỉ reset khi bắt đầu game mới hoặc map mới từ đầu

// Tạo Mây
SDL_Texture* cloudTextures[] = {cloudTexture1, cloudTexture2}; // Sử dụng texture toàn cục đã load
float mapVisualWidth = MAP_WIDTH * TILE_SIZE * 1.1f;
for (int i = 0; i < NUM_CLOUDS_PER_MAP; ++i) {
if (!cloudTexture1 || !cloudTexture2) break; // Kiểm tra texture đã load chưa
Cloud cloud; int texIndex = rand() % 2; cloud.texture = cloudTextures[texIndex];
cloud.x = static_cast<float>(rand() % static_cast<int>(mapVisualWidth));
cloud.y = 50 + (rand() % (SCREEN_HEIGHT / 3 - 50)); clouds.push_back(cloud);
}

// Tải lại Enemies, Stars, Coins từ Tilemap
for (int y = 0; y < MAP_HEIGHT; y++) {
for (int x = 0; x < MAP_WIDTH; x++) {
int tileType = tilemaps[currentMap][y][x];
if (tileType == ENEMY_SPAWN_TILE) { // Enemy
int enemyY = y * TILE_SIZE; bool enemyGroundFound = false;
for(int checkY = y; checkY < MAP_HEIGHT; ++checkY){
    int checkTile = tilemaps[currentMap][checkY][x];
    if(checkTile != EMPTY_TILE && checkTile != ENEMY_SPAWN_TILE && checkTile != STAR_TILE && checkTile != COIN_TILE){
        enemyY = checkY * TILE_SIZE - 50;
        enemyGroundFound = true;
        break;
    }
}
if (!enemyGroundFound) enemyY = y * TILE_SIZE;

Entity enemy(x * TILE_SIZE, enemyY, 50, 50, 0, 0, false);
enemy.direction = (rand() % 2 == 0) ? 1 : -1;
enemy.enemyAnimFrame = 0;
enemy.enemyAnimTimer = 0;
enemies.push_back(enemy);
enemiesAlive.push_back(true);
} else if (tileType == STAR_TILE) { // Star
SDL_Rect star = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}; stars.push_back(star);
} else if (tileType == COIN_TILE) { // Coin
 SDL_Rect coin = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}; coins.push_back(coin); totalCoinsInMap++;
}
}
}
std::cout << "DEBUG: resetGameLevel finished for Map " << currentMap << ". Total Coins: " << totalCoinsInMap << std::endl;
}

void respawnPlayer(Entity& mario) {
    // Reset vị trí về điểm bắt đầu (bạn có thể điều chỉnh tọa độ này)
    mario.rect.x = 100;
    mario.rect.y = 100;

    // Reset tốc độ
    mario.velocityX = 0;
    mario.velocityY = 0;

    // Reset trạng thái
    mario.onGround = false; // Sẽ rơi xuống do trọng lực
    mario.isBuffed = false;
    mario.buffTimer = 0;

    // Reset kích thước nếu đang được buff
    if (mario.wasBuffed) {
        int oldH = mario.rect.h;
        mario.rect.w = mario.originalW;
        mario.rect.h = mario.originalH;
        // Không cần điều chỉnh y ở đây, trọng lực sẽ xử lý
        mario.wasBuffed = false;
    }

    // Reset animation
    mario.animState = Entity::STAND;
    mario.animFrame = 0;
    mario.animTimer = 0;
    mario.direction = 1; // Hướng mặc định sang phải

    std::cout << "Player respawned." << std::endl;

    // Tùy chọn: Thêm một khoảng thời gian bất tử ngắn sau khi hồi sinh
    // mario.invincibleTimer = 120; // ví dụ 2 giây ở 60 FPS
}