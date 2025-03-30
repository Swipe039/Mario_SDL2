#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "Entity.h"
#include "GameLogic.h"
#include "Renderer.h"
#include "Constants.h"
#include "Control.h"
#include <iostream>
#include <vector>
#include <algorithm>
// Tính điểm
// Màn hình thắng/thua

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cout << "Không thể tải " << path << "! IMG_Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cout << "Không thể tạo texture từ " << path << "! SDL_Error: " << SDL_GetError() << std::endl;
    }
    return texture;
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL không thể khởi tạo! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }
    
    if (TTF_Init() < 0) {
        std::cout << "SDL_ttf không thể khởi tạo! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cout << "SDL_image không thể khởi tạo! IMG_Error: " << IMG_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Super Mario SDL2", 
                                        SDL_WINDOWPOS_CENTERED, 
                                        SDL_WINDOWPOS_CENTERED, 
                                        SCREEN_WIDTH, 
                                        SCREEN_HEIGHT, 
                                        SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Không thể tạo cửa sổ! SDL_Error: " << SDL_GetError() << std::endl;
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Không thể tạo renderer! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    TTF_Font *font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cout << "Không thể tải font! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // Tải Groundblock.png
    SDL_Texture *groundTexture = loadTexture("Groundblock.png", renderer);
    if (!groundTexture) {
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // Tải các texture hoạt ảnh cho Mario
    SDL_Texture *jumpLeftTextures[7];
    SDL_Texture *jumpRightTextures[7];
    SDL_Texture *runLeftTextures[3];
    SDL_Texture *runRightTextures[3];
    SDL_Texture *standLeftTexture;
    SDL_Texture *standRightTexture;

    for (int i = 0; i < 7; i++) {
        jumpLeftTextures[i] = loadTexture(("Jump_left_" + std::to_string(i + 1) + ".png").c_str(), renderer);
        jumpRightTextures[i] = loadTexture(("Jump_right_" + std::to_string(i + 1) + ".png").c_str(), renderer);
        if (!jumpLeftTextures[i] || !jumpRightTextures[i]) {
            for (int j = 0; j < i; j++) {
                SDL_DestroyTexture(jumpLeftTextures[j]);
                SDL_DestroyTexture(jumpRightTextures[j]);
            }
            SDL_DestroyTexture(groundTexture);
            TTF_CloseFont(font);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            IMG_Quit();
            TTF_Quit();
            SDL_Quit();
            return -1;
        }
    }

    for (int i = 0; i < 3; i++) {
        runLeftTextures[i] = loadTexture(("Run_left_" + std::to_string(i + 1) + ".png").c_str(), renderer);
        runRightTextures[i] = loadTexture(("Run_right_" + std::to_string(i + 1) + ".png").c_str(), renderer);
        if (!runLeftTextures[i] || !runRightTextures[i]) {
            for (int j = 0; j < i; j++) {
                SDL_DestroyTexture(runLeftTextures[j]);
                SDL_DestroyTexture(runRightTextures[j]);
            }
            for (int j = 0; j < 7; j++) {
                SDL_DestroyTexture(jumpLeftTextures[j]);
                SDL_DestroyTexture(jumpRightTextures[j]);
            }
            SDL_DestroyTexture(groundTexture);
            TTF_CloseFont(font);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            IMG_Quit();
            TTF_Quit();
            SDL_Quit();
            return -1;
        }
    }

    standLeftTexture = loadTexture("Stand_left.png", renderer);
    standRightTexture = loadTexture("Stand_right.png", renderer);
    if (!standLeftTexture || !standRightTexture) {
        for (int j = 0; j < 7; j++) {
            SDL_DestroyTexture(jumpLeftTextures[j]);
            SDL_DestroyTexture(jumpRightTextures[j]);
        }
        for (int j = 0; j < 3; j++) {
            SDL_DestroyTexture(runLeftTextures[j]);
            SDL_DestroyTexture(runRightTextures[j]);
        }
        SDL_DestroyTexture(groundTexture);
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // Tải Fly_block.png
    SDL_Texture *flyBlockTexture = loadTexture("Fly_block.png", renderer);
    if (!flyBlockTexture) {
        SDL_DestroyTexture(groundTexture);
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Texture *starTexture = loadTexture("Star.png", renderer);   
    if (!starTexture) {
        SDL_DestroyTexture(flyBlockTexture);
        SDL_DestroyTexture(groundTexture);
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Texture *enemyTexture = loadTexture("Enemy.png", renderer);
    if (!enemyTexture) {        
        SDL_DestroyTexture(starTexture);
        SDL_DestroyTexture(flyBlockTexture);
        SDL_DestroyTexture(groundTexture);
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    GameState state = MAIN_MENU;
    bool running = true;
    int selectedMap = 0;
    int currentMap = 0;
    
    Entity mario(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100, 50, 50, 0, 0, true);
    std::vector<Entity> enemies;
    std::vector<bool> enemiesAlive;
    std::vector<SDL_Rect> stars;
    int cameraX = 0;
    int marioVelocityX = 0;
    bool dropThrough = false;
    int score = 0; // Biến điểm số

    Uint32 frameStart;
    int frameTime;
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    while (running) {
        frameStart = SDL_GetTicks();

        switch (state) {
            case MAIN_MENU: {
                int selectedItem = 0;
                bool inMainMenu = true;
                while (inMainMenu && running) {
                    SDL_Event e;
                    while (SDL_PollEvent(&e)) {
                        if (e.type == SDL_QUIT) {
                            inMainMenu = false;
                            running = false;
                        }
                        if (e.type == SDL_KEYDOWN) {
                            switch (e.key.keysym.sym) {
                                case SDLK_UP:
                                    selectedItem = (selectedItem - 1 + MENU_ITEM_COUNT) % MENU_ITEM_COUNT;
                                    break;
                                case SDLK_DOWN:
                                    selectedItem = (selectedItem + 1) % MENU_ITEM_COUNT;
                                    break;
                                case SDLK_RETURN:
                                    if (selectedItem == 0) {
                                        state = MAP_SELECT;
                                        inMainMenu = false;
                                    } else {
                                        running = false;
                                        inMainMenu = false;
                                    }
                                    break;
                                case SDLK_ESCAPE:
                                    running = false;
                                    inMainMenu = false;
                                    break;
                            }
                        }
                    }
                    renderMenu(renderer, font, selectedItem);
                    frameTime = SDL_GetTicks() - frameStart;
                    if (frameDelay > frameTime) {
                        SDL_Delay(frameDelay - frameTime);
                    }
                }
                break;
            }
            case MAP_SELECT: {
                bool inMapSelect = true;
                selectedMap = 0;
                while (inMapSelect && running) {
                    SDL_Event e;
                    handleMapSelectInput(e, state, selectedMap);
                    if (state == GAMEPLAY) {
                        currentMap = selectedMap;
                        inMapSelect = false;
                        mario = Entity(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100, 50, 50, 0, 0, true);
                        enemies.clear();
                        enemiesAlive.clear();
                        stars.clear();
                        for (int y = 0; y < MAP_HEIGHT; y++) {
                            for (int x = 0; x < MAP_WIDTH; x++) {
                                if (tilemaps[currentMap][y][x] == 4) {
                                    Entity enemy(x * TILE_SIZE, y * TILE_SIZE, 50, 50, 0, 0, false);
                                    enemy.direction = 1;
                                    enemies.push_back(enemy);
                                    enemiesAlive.push_back(true);
                                } else if (tilemaps[currentMap][y][x] == 5) {
                                    SDL_Rect star = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                                    stars.push_back(star);
                                }
                            }
                        }
                    } else if (state == MAIN_MENU) {
                        inMapSelect = false;
                    }
                    renderMapSelect(renderer, font, selectedMap);
                    frameTime = SDL_GetTicks() - frameStart;
                    if (frameDelay > frameTime) {
                        SDL_Delay(frameDelay - frameTime);
                    }
                }
                break;
            }
            case GAMEPLAY: {
                int deltaY = 0;
                int PreY = mario.rect.y;
                SDL_Event e;
                handleInput(e, running, mario, marioVelocityX, dropThrough, state);
                mario.velocityX = marioVelocityX;
                cameraX = mario.rect.x - SCREEN_WIDTH / 2;
                cameraX = std::max(0, std::min(cameraX, MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH));
                // Sửa lỗi: Thêm tham số score vào hàm updateEntity
                updateEntity(mario, enemies, enemiesAlive, running, dropThrough, cameraX, state, stars, tilemaps[currentMap], score);
                deltaY = mario.rect.y - PreY;
                for (size_t i = 0; i < enemies.size(); i++) {
                    if (enemiesAlive[i]) {
                        updateEnemy(enemies[i], enemiesAlive[i], cameraX, tilemaps[currentMap]);
                        if (enemies[i].rect.y > MAP_HEIGHT * TILE_SIZE) {
                            enemiesAlive[i] = false;
                        }
                    }
                }
                render(renderer, mario, enemies, enemiesAlive, cameraX, stars, currentMap, font, groundTexture, flyBlockTexture, starTexture, enemyTexture,
                       jumpLeftTextures, jumpRightTextures, runLeftTextures, runRightTextures, standLeftTexture, standRightTexture, deltaY, score);
                frameTime = SDL_GetTicks() - frameStart;
                if (frameDelay > frameTime) {
                    SDL_Delay(frameDelay - frameTime);
                }
                break;
            }
            case PAUSE_MENU: {
                int deltaY = 0;
                int PreY = mario.rect.y;
                int selectedItem = 0;
                bool inPauseMenu = true;
                while (inPauseMenu && running) {
                    SDL_Event e;
                    handlePauseMenuInput(e, state, running);
                    if (state != PAUSE_MENU) {
                        inPauseMenu = false;
                    }
                    deltaY = mario.rect.y - PreY;
                    render(renderer, mario, enemies, enemiesAlive, cameraX, stars, currentMap, font, groundTexture, flyBlockTexture, starTexture, enemyTexture,
                           jumpLeftTextures, jumpRightTextures, runLeftTextures, runRightTextures, standLeftTexture, standRightTexture, deltaY, score);
                    renderPauseMenu(renderer, font, selectedItem);
                    frameTime = SDL_GetTicks() - frameStart;
                    if (frameDelay > frameTime) {
                        SDL_Delay(frameDelay - frameTime);
                    }
                }
                break;
            }
        }
    }

    // Giải phóng tài nguyên
    SDL_DestroyTexture(groundTexture);
    for (int i = 0; i < 7; i++) {
        SDL_DestroyTexture(jumpLeftTextures[i]);
        SDL_DestroyTexture(jumpRightTextures[i]);
    }
    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(runLeftTextures[i]);
        SDL_DestroyTexture(runRightTextures[i]);
    }
    SDL_DestroyTexture(flyBlockTexture);
    SDL_DestroyTexture(standLeftTexture);
    SDL_DestroyTexture(standRightTexture);
    SDL_DestroyTexture(starTexture);
    SDL_DestroyTexture(enemyTexture);
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}