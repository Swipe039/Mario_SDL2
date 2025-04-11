#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "Entity.h"
#include "GameLogic.h" // Chứa respawnPlayer, resetGameLevel, updateEntity, updateEnemy
#include "Renderer.h"   // Chứa render
#include "Constants.h"  // Chứa MAX_HEARTS, GameState enum, etc.
#include "Control.h"    // Chứa các hàm handleInput
#include "Init.h"       // Chứa initializeSDL, shutdownSDL
#include "Texture.h"    // Chứa Load_textures, Clean_up_textures và các biến texture extern (bao gồm heartTexture)
#include "RenderGameState.h" // Chứa các hàm render màn hình khác (MapSelect, Win, Lose, Pause)
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // Cho std::max, std::min
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[]) {
    srand(static_cast<unsigned int>(time(0))); // Khởi tạo seed cho random

    // --- Khởi tạo SDL và các thành phần ---
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!initializeSDL(window, renderer)) {
        std::cerr << "Failed to initialize SDL. Exiting." << std::endl;
        return -1;
    }

    // Mở Font
    font = TTF_OpenFont("Font/SuperMario256.ttf", 24);
    if (!font) {
        std::cerr << "Font Loading Error: " << TTF_GetError() << std::endl;
        shutdownSDL(window, renderer); // Dọn dẹp SDL đã khởi tạo
        return -1;
    }

    // Load Textures (bao gồm cả heartTexture)
    if (!Load_textures(renderer)) {
        std::cerr << "Failed to load textures. Exiting." << std::endl;
        Clean_up_textures(); // Dọn dẹp texture đã load (nếu có)
        TTF_CloseFont(font); // Đóng font
        shutdownSDL(window, renderer); // Dọn dẹp SDL
        return -1;
    }

    // Load Âm thanh
    Mix_Music *gameplayMusic = Mix_LoadMUS("Sound/Ground-Theme.wav");
    Mix_Music *menuMusic = Mix_LoadMUS("Sound/Underground-Theme.wav");
    if (!gameplayMusic) {
        std::cerr << "Failed to load Ground-Theme.wav! Mix_Error: " << Mix_GetError() << std::endl;
        // Có thể tiếp tục không có nhạc gameplay
    }
    if (!menuMusic) {
        std::cerr << "Failed to load Underground-Theme.wav! Mix_Error: " << Mix_GetError() << std::endl;
        // Có thể tiếp tục không có nhạc menu
    }

    // --- Khởi tạo biến trạng thái Game ---
    GameState state = MAIN_MENU;
    GameState nextState = MAIN_MENU; // Dùng để quản lý chuyển đổi state mượt hơn
    bool running = true;
    int selectedMap = 0; // Map đang được chọn trong màn hình Map Select
    int currentMap = 0; // Map đang chơi thực tế
    int selectedMenuItem = 0; // Item đang được chọn trong các menu

    // --- Khởi tạo biến người chơi và thế giới game ---
    Entity mario(100, 100, 50, 50, 0, 0, false); // Vị trí bắt đầu (sẽ được reset trong resetGameLevel)
    int playerHearts = MAX_HEARTS;                 // << Biến mạng sống >>
    std::vector<Entity> enemies;                   // Danh sách kẻ địch
    std::vector<bool> enemiesAlive;                // Trạng thái sống/chết của kẻ địch
    std::vector<SDL_Rect> stars;                   // Danh sách sao (nếu còn dùng)
    std::vector<Cloud> clouds;                     // Danh sách mây
    std::vector<SDL_Rect> coins;                   // Danh sách coin
    int collectedCoins = 0;                        // Số coin đã thu thập
    int totalCoinsInMap = 0;                       // Tổng số coin trong map hiện tại
    int cameraX = 0;                               // Vị trí camera theo trục X
    bool dropThrough = false;                      // Cờ cho phép rơi qua platform
    int score = 0;                                 // Điểm số
    Mix_Music* currentlyPlayingMusic = nullptr;    // Nhạc đang phát

    // --- Vòng lặp Game chính ---
    while (running) {
        SDL_Event e; // Biến lưu trữ sự kiện

        // --- Xử lý chuyển đổi trạng thái ---
        if (state != nextState) {
            state = nextState;
            selectedMenuItem = 0; // Reset lựa chọn menu khi chuyển state
            if (Mix_PlayingMusic()) {
                 Mix_HaltMusic(); // Dừng nhạc cũ
                 currentlyPlayingMusic = nullptr;
            }
            // Reset thêm nếu cần khi vào state mới
            if (state == MAP_SELECT) {
                selectedMap = currentMap; // Hiển thị map đang chọn là map hiện tại
            }
        }

        // --- Xử lý phát nhạc nền ---
        Mix_Music* desiredMusic = nullptr;
        if (state == GAMEPLAY) {
            desiredMusic = gameplayMusic;
        } else if (state == MAIN_MENU || state == MAP_SELECT || state == WIN_SCREEN || state == LOSE_SCREEN) { // Các màn hình dùng nhạc menu
            desiredMusic = menuMusic;
        }
        // Phát nhạc mới nếu cần
        if (desiredMusic != nullptr && currentlyPlayingMusic != desiredMusic) {
            if (Mix_PlayingMusic()) {
                Mix_HaltMusic();
            }
            Mix_PlayMusic(desiredMusic, -1); // Lặp vô hạn
            currentlyPlayingMusic = desiredMusic;
        }
        // Dừng nhạc nếu không có nhạc mong muốn (ví dụ: màn hình Pause không nhạc)
        else if (desiredMusic == nullptr && Mix_PlayingMusic()) {
            Mix_HaltMusic();
            currentlyPlayingMusic = nullptr;
        }


        // --- Xử lý logic và render dựa trên trạng thái game ---
        switch (state) {
            case MAIN_MENU:
                 handleMainMenuInput(e, nextState, running, selectedMenuItem); // nextState sẽ thay đổi nếu chọn Start/Exit
                 renderMenu(renderer, font, selectedMenuItem); // Vẽ menu chính
                 break; // Kết thúc case MAIN_MENU

             case MAP_SELECT:
                // Reset level khi vào màn hình chọn map (để chuẩn bị cho map mới)
                // Lưu ý: score và hearts cũng được reset ở đây
                resetGameLevel(mario, enemies, enemiesAlive, stars, clouds, coins, collectedCoins, totalCoinsInMap, cloudTexture1, cloudTexture2, score, currentMap, playerHearts);
                cameraX = 0; // Reset camera khi quay lại chọn map

                handleMapSelectInput(e, nextState, running, selectedMap); // Xử lý input chọn map

                // Nếu người chơi chọn map và nhấn Enter (nextState = GAMEPLAY)
                if (nextState == GAMEPLAY) {
                    currentMap = selectedMap; // Cập nhật map sẽ chơi
                    score = 0; // Reset điểm khi bắt đầu map mới
                    // Reset lại game với map MỚI và reset hearts/score
                    resetGameLevel(mario, enemies, enemiesAlive, stars, clouds, coins, collectedCoins, totalCoinsInMap, cloudTexture1, cloudTexture2, score, currentMap, playerHearts);
                    cameraX = 0; // Đảm bảo camera bắt đầu từ 0 cho map mới
                }
                renderMapSelect(renderer, font, selectedMap); // Vẽ màn hình chọn map
                 break; // Kết thúc case MAP_SELECT

            case GAMEPLAY: { // Sử dụng scope để khai báo biến tạm thời
                int heartsBeforeUpdate = playerHearts; // Lưu số mạng trước khi cập nhật

                // Xử lý input người chơi
                handleInput(mario, running, dropThrough, nextState); // nextState có thể thành PAUSE_MENU

                // Cập nhật trạng thái người chơi và va chạm
                // Hàm này sẽ xử lý mất mạng, hồi sinh, và chuyển sang LOSE_SCREEN nếu hết mạng
                updateEntity(mario, enemies, enemiesAlive, running, dropThrough, nextState, stars, coins, collectedCoins, totalCoinsInMap, tilemaps[currentMap], score, playerHearts);

                // Nếu người chơi mất mạng nhưng game chưa kết thúc (vừa hồi sinh) -> Reset camera
                if (state == GAMEPLAY && playerHearts < heartsBeforeUpdate) {
                     cameraX = 0; // Reset camera về đầu map
                     std::cout << "Camera reset after respawn." << std::endl;
                }

                // Cập nhật trạng thái kẻ địch (chỉ khi game đang chạy bình thường)
                if (state == GAMEPLAY){
                    for (size_t i = 0; i < enemies.size(); i++) {
                        if (enemiesAlive[i]) {
                            // Hàm updateEnemy trả về false nếu enemy chết (ví dụ: rơi vực)
                            enemiesAlive[i] = updateEnemy(enemies[i], enemiesAlive[i], cameraX, tilemaps[currentMap]);
                        }
                    }
                }

                // Cập nhật camera (chỉ khi game đang chạy bình thường)
                 if (state == GAMEPLAY){
                    cameraX = mario.rect.x - SCREEN_WIDTH / 3; // Giữ Mario ở khoảng 1/3 màn hình bên trái
                    // Giới hạn camera không đi ra ngoài map
                    cameraX = std::max(0, std::min(cameraX, MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH));
                 }

                // Render màn hình game (bao gồm cả hearts)
                render(renderer, mario, enemies, enemiesAlive, cameraX, stars, clouds, cloudTexture1, cloudTexture2,
                       pipeTopTexture, pipeBodyTexture, coinTexture, coins, collectedCoins, currentMap, font,
                       groundTexture, flyBlockTexture, starTexture,enemyTextures,
                       jumpLeftTextures, jumpRightTextures, runLeftTextures, runRightTextures,
                       standLeftTexture, standRightTexture,
                       finishLineTexture, score,
                       playerHearts, heartTexture); // Truyền playerHearts và heartTexture
                SDL_RenderPresent(renderer); // Hiển thị lên màn hình
                break; // Kết thúc case GAMEPLAY
            } // Đóng scope của case GAMEPLAY

            case PAUSE_MENU:
                 handlePauseMenuInput(e, nextState, running, selectedMenuItem); // nextState có thể thành GAMEPLAY hoặc MAIN_MENU
                 // Vẫn render màn hình game phía sau Pause Menu
                 render(renderer, mario, enemies, enemiesAlive, cameraX, stars, clouds, cloudTexture1, cloudTexture2,
                        pipeTopTexture, pipeBodyTexture, coinTexture, coins, collectedCoins, currentMap, font,
                        groundTexture, flyBlockTexture, starTexture,
                        enemyTextures, jumpLeftTextures, jumpRightTextures, runLeftTextures, runRightTextures,
                        standLeftTexture, standRightTexture,
                        finishLineTexture, score,
                        playerHearts, heartTexture); // Truyền playerHearts và heartTexture
                 renderPauseMenu(renderer, font, selectedMenuItem); // Vẽ menu Pause đè lên trên
                 SDL_RenderPresent(renderer);
                 break; // Kết thúc case PAUSE_MENU

            case WIN_SCREEN:
                  handleWinLoseInput(e, state, nextState, running, selectedMenuItem); // Xử lý input màn hình Win

                  // Nếu chọn Retry (GAMEPLAY) hoặc Map Select, cần reset
                  if (nextState == GAMEPLAY || nextState == MAP_SELECT) {
                      score = 0; // Reset điểm
                      // Reset level với map hiện tại (nếu retry) hoặc chuẩn bị reset ở MAP_SELECT
                      resetGameLevel(mario, enemies, enemiesAlive, stars, clouds, coins, collectedCoins, totalCoinsInMap, cloudTexture1, cloudTexture2, score, currentMap, playerHearts);
                      cameraX = 0;
                  }
                  renderWinScreen(renderer, font, score, selectedMenuItem); // Vẽ màn hình thắng
                 break; // Kết thúc case WIN_SCREEN

             case LOSE_SCREEN:
                  handleWinLoseInput(e, state, nextState, running, selectedMenuItem); // Xử lý input màn hình Lose

                  // Nếu chọn Retry (GAMEPLAY) hoặc Map Select, cần reset
                  if (nextState == GAMEPLAY || nextState == MAP_SELECT) {
                     score = 0; // Reset điểm
                     // Reset level với map hiện tại (nếu retry) hoặc chuẩn bị reset ở MAP_SELECT
                     resetGameLevel(mario, enemies, enemiesAlive, stars, clouds, coins, collectedCoins, totalCoinsInMap, cloudTexture1, cloudTexture2, score, currentMap, playerHearts);
                     cameraX = 0;
                  }
                  renderLoseScreen(renderer, font, score, selectedMenuItem); // Vẽ màn hình thua
                  break; 
        } 
    } 

    std::cout << "Exiting game loop. Cleaning up all resources..." << std::endl;
    Clean_up_textures(); // Dọn dẹp tất cả textures và font
    if (gameplayMusic) { Mix_FreeMusic(gameplayMusic); gameplayMusic = nullptr; std::cout << "Gameplay music freed." << std::endl;}
    if (menuMusic) { Mix_FreeMusic(menuMusic); menuMusic = nullptr; std::cout << "Menu music freed." << std::endl;}
    // Font đã được dọn dẹp trong Clean_up_textures()
    shutdownSDL(window, renderer); // Dọn dẹp SDL, window, renderer, và các thư viện con
    std::cout << "Cleanup complete. Exiting application." << std::endl;
    return 0; 
}