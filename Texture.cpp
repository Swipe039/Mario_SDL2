#include "Texture.h"
#include "Constants.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
// Không cần include <SDL2/SDL_mixer.h> ở đây nếu chỉ quản lý texture/font
#include <iostream>
#include <string>
#include <vector> // Include nếu dùng vector trong Texture.h

// Định nghĩa các biến toàn cục đã khai báo với extern trong Texture.h
TTF_Font *font = nullptr; // Khởi tạo là nullptr
SDL_Texture *groundTexture = nullptr;
SDL_Texture *flyBlockTexture = nullptr;
SDL_Texture *starTexture = nullptr;
SDL_Texture *enemyTextures[2] = {nullptr, nullptr}; // Khởi tạo các phần tử
SDL_Texture *standLeftTexture = nullptr;
SDL_Texture *standRightTexture = nullptr;
SDL_Texture *cloudTexture1 = nullptr;
SDL_Texture *cloudTexture2 = nullptr;
SDL_Texture *pipeTopTexture = nullptr;
SDL_Texture *pipeBodyTexture = nullptr;
SDL_Texture *coinTexture = nullptr;
SDL_Texture *finishLineTexture = nullptr;
SDL_Texture *heartTexture = nullptr; 
SDL_Texture *jumpLeftTextures[7] = {nullptr}; // Khởi tạo các phần tử
SDL_Texture *jumpRightTextures[7] = {nullptr};
SDL_Texture *runLeftTextures[3] = {nullptr};
SDL_Texture *runRightTextures[3] = {nullptr};


// Hàm tiện ích tải texture (giữ nguyên)
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path);
    if (newTexture == nullptr) {
        std::cerr << "Unable to load texture: " << path << "! IMG_Error: " << IMG_GetError() << std::endl;
    }
    return newTexture;
}

// Hàm tải tất cả texture (đã sửa)
bool Load_textures(SDL_Renderer* renderer) {
    bool success = true; // Giả định thành công ban đầu

    std::cout << "Loading textures..." << std::endl;

    groundTexture = loadTexture("Texture/Groundblock.png", renderer);
    if (groundTexture == nullptr) success = false;

    flyBlockTexture = loadTexture("Texture/Fly_block.png", renderer);
    if (flyBlockTexture == nullptr) success = false;

    starTexture = loadTexture("Texture/Star.png", renderer);
    if (starTexture == nullptr) success = false;

    enemyTextures[0] = loadTexture("Texture/Enemy_1_1.png", renderer);
    if (enemyTextures[0] == nullptr) success = false;
    enemyTextures[1] = loadTexture("Texture/Enemy_1_2.png", renderer);
    if (enemyTextures[1] == nullptr) success = false;

    standLeftTexture = loadTexture("Texture/Stand_left.png", renderer);
    if (standLeftTexture == nullptr) success = false;
    standRightTexture = loadTexture("Texture/Stand_right.png", renderer);
    if (standRightTexture == nullptr) success = false;

    cloudTexture1 = loadTexture("Texture/Cloud_1.png", renderer);
    if (cloudTexture1 == nullptr) success = false;
    cloudTexture2 = loadTexture("Texture/Cloud_2.png", renderer);
    if (cloudTexture2 == nullptr) success = false;

    pipeTopTexture = loadTexture("Texture/Pipe_1.png", renderer);
    if (pipeTopTexture == nullptr) success = false;
    pipeBodyTexture = loadTexture("Texture/Pipe_2.png", renderer);
    if (pipeBodyTexture == nullptr) success = false;
    heartTexture = loadTexture("Texture/Heart.png", renderer);
    if (heartTexture == nullptr) success = false;
    coinTexture = loadTexture("Texture/Coin.png", renderer);
    if (coinTexture == nullptr) success = false;

    finishLineTexture = loadTexture("Texture/Finish_line.png", renderer);
    if (finishLineTexture == nullptr) success = false;

    // Tải animation Jump
    for (int i = 0; i < 7; i++) {
        std::string jumpLeftPath = "Texture/Jump_left_" + std::to_string(i + 1) + ".png";
        jumpLeftTextures[i] = loadTexture(jumpLeftPath.c_str(), renderer);
        if (jumpLeftTextures[i] == nullptr) {
            success = false;
            std::cerr << "Failed to load jump left animation frame " << (i + 1) << std::endl;
        }

        std::string jumpRightPath = "Texture/Jump_right_" + std::to_string(i + 1) + ".png";
        jumpRightTextures[i] = loadTexture(jumpRightPath.c_str(), renderer);
        if (jumpRightTextures[i] == nullptr) {
            success = false;
            std::cerr << "Failed to load jump right animation frame " << (i + 1) << std::endl;
        }
    }

    // Tải animation Run
    for (int i = 0; i < 3; i++) {
        std::string runLeftPath = "Texture/Run_left_" + std::to_string(i + 1) + ".png";
        runLeftTextures[i] = loadTexture(runLeftPath.c_str(), renderer);
        if (runLeftTextures[i] == nullptr) {
            success = false;
            std::cerr << "Failed to load run left animation frame " << (i + 1) << std::endl;
        }

        std::string runRightPath = "Texture/Run_right_" + std::to_string(i + 1) + ".png";
        runRightTextures[i] = loadTexture(runRightPath.c_str(), renderer);
        if (runRightTextures[i] == nullptr) {
            success = false;
            std::cerr << "Failed to load run right animation frame " << (i + 1) << std::endl;
        }
    }

    if (success) {
        std::cout << "All textures loaded successfully." << std::endl;
    } else {
        std::cerr << "!!! Failed to load one or more textures." << std::endl;
    }

    return success;
}


// Hàm dọn dẹp texture và font (đã sửa)
void Clean_up_textures() {
    std::cout << "Cleaning up textures and font..." << std::endl;


    if (groundTexture) { SDL_DestroyTexture(groundTexture); groundTexture = nullptr; }
    if (flyBlockTexture) { SDL_DestroyTexture(flyBlockTexture); flyBlockTexture = nullptr; }
    if (starTexture) { SDL_DestroyTexture(starTexture); starTexture = nullptr; }
    if (standLeftTexture) { SDL_DestroyTexture(standLeftTexture); standLeftTexture = nullptr; }
    if (standRightTexture) { SDL_DestroyTexture(standRightTexture); standRightTexture = nullptr; }
    if (cloudTexture1) { SDL_DestroyTexture(cloudTexture1); cloudTexture1 = nullptr; }
    if (cloudTexture2) { SDL_DestroyTexture(cloudTexture2); cloudTexture2 = nullptr; }
    if (pipeTopTexture) { SDL_DestroyTexture(pipeTopTexture); pipeTopTexture = nullptr; }
    if (pipeBodyTexture) { SDL_DestroyTexture(pipeBodyTexture); pipeBodyTexture = nullptr; }
    if (coinTexture) { SDL_DestroyTexture(coinTexture); coinTexture = nullptr; }
    if (finishLineTexture) { SDL_DestroyTexture(finishLineTexture); finishLineTexture = nullptr; }
    for(int i=0; i<2; ++i) { if(enemyTextures[i]) { SDL_DestroyTexture(enemyTextures[i]); enemyTextures[i] = nullptr; } }
    for(int i=0; i<7; ++i) {
        if(jumpLeftTextures[i]) { SDL_DestroyTexture(jumpLeftTextures[i]); jumpLeftTextures[i] = nullptr; }
        if(jumpRightTextures[i]) { SDL_DestroyTexture(jumpRightTextures[i]); jumpRightTextures[i] = nullptr; }
    }
    for (int i=0; i<3; ++i) {
        if(runLeftTextures[i]) { SDL_DestroyTexture(runLeftTextures[i]); runLeftTextures[i] = nullptr; }
        if(runRightTextures[i]) { SDL_DestroyTexture(runRightTextures[i]); runRightTextures[i] = nullptr; }
    }


    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
        std::cout << "Font closed." << std::endl;
    }

    std::cout << "Texture and font cleanup complete." << std::endl;
  
}