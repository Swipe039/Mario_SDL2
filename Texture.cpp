#include "Texture.h"
#include "Constants.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <string>
#include <vector>

TTF_Font *font = nullptr;
SDL_Texture *groundTexture = nullptr;
SDL_Texture *flyBlockTexture = nullptr;
SDL_Texture *starTexture = nullptr;
SDL_Texture *enemyTextures[2] = {nullptr, nullptr};
SDL_Texture *standLeftTexture = nullptr;
SDL_Texture *standRightTexture = nullptr;
SDL_Texture *cloudTexture1 = nullptr;
SDL_Texture *cloudTexture2 = nullptr;
SDL_Texture *pipeTopTexture = nullptr;
SDL_Texture *pipeBodyTexture = nullptr;
SDL_Texture *coinTexture = nullptr;
SDL_Texture *finishLineTexture = nullptr;
SDL_Texture *heartTexture = nullptr;
SDL_Texture *jumpLeftTextures[7] = {nullptr};
SDL_Texture *jumpRightTextures[7] = {nullptr};
SDL_Texture *runLeftTextures[3] = {nullptr};
SDL_Texture *runRightTextures[3] = {nullptr};
SDL_Texture *mapSelectBackgroundTexture = nullptr;

Mix_Chunk *winSound = nullptr;
Mix_Chunk *buffSound = nullptr;
Mix_Chunk *jumpSound = nullptr;


SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path);
    if (newTexture == nullptr) {
        std::cerr << "Unable to load texture: " << path << "! IMG_Error: " << IMG_GetError() << std::endl;
    }
    return newTexture;
}

bool Load_textures(SDL_Renderer* renderer) {
    bool success = true;

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

    finishLineTexture = loadTexture("Texture/Flag.png", renderer);
    if (finishLineTexture == nullptr) success = false;

    mapSelectBackgroundTexture = loadTexture("Texture/Background.png", renderer);
    if (mapSelectBackgroundTexture == nullptr) {
         std::cerr << "!!! Failed to load map select background texture." << std::endl;
            success = false;
    }

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

    std::cout << "Loading sound effects..." << std::endl;
    winSound = Mix_LoadWAV("Sound/win.wav");
    if (winSound == nullptr) {
        std::cerr << "Failed to load win sound effect! Mix_Error: " << Mix_GetError() << std::endl;
        success = false;
    }
    buffSound = Mix_LoadWAV("Sound/buff.wav");
    if (buffSound == nullptr) {
        std::cerr << "Failed to load buff sound effect! Mix_Error: " << Mix_GetError() << std::endl;
        success = false;
    }
    jumpSound = Mix_LoadWAV("Sound/jump.wav");
    if (jumpSound == nullptr) {
        std::cerr << "Failed to load jump sound effect! Mix_Error: " << Mix_GetError() << std::endl;
        success = false;
    }
    else  Mix_VolumeChunk(jumpSound, 15);


    if (success) {
        std::cout << "All textures, font, and sound effects loaded successfully." << std::endl;
    } else {
        std::cerr << "!!! Failed to load one or more textures, font, or sound effects." << std::endl;
    }

    return success;
}

void Clean_up_textures() {
    std::cout << "Cleaning up textures, font, and sound effects..." << std::endl;

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
    if (heartTexture) { SDL_DestroyTexture(heartTexture); heartTexture = nullptr; }
    if (mapSelectBackgroundTexture) {
        SDL_DestroyTexture(mapSelectBackgroundTexture);
        mapSelectBackgroundTexture = nullptr;
    }
    for(int i=0; i<2; ++i) { if(enemyTextures[i]) { SDL_DestroyTexture(enemyTextures[i]); enemyTextures[i] = nullptr; } }
    for(int i=0; i<7; ++i) {
        if(jumpLeftTextures[i]) { SDL_DestroyTexture(jumpLeftTextures[i]); jumpLeftTextures[i] = nullptr; }
        if(jumpRightTextures[i]) { SDL_DestroyTexture(jumpRightTextures[i]); jumpRightTextures[i] = nullptr; }
    }
    for (int i=0; i<3; ++i) {
        if(runLeftTextures[i]) { SDL_DestroyTexture(runLeftTextures[i]); runLeftTextures[i] = nullptr; }
        if(runRightTextures[i]) { SDL_DestroyTexture(runRightTextures[i]); runRightTextures[i] = nullptr; }
    }

    if (winSound) { Mix_FreeChunk(winSound); winSound = nullptr; std::cout << "Win sound freed." << std::endl;}
    if (buffSound) { Mix_FreeChunk(buffSound); buffSound = nullptr; std::cout << "Buff sound freed." << std::endl;}
    if (jumpSound) { Mix_FreeChunk(jumpSound); jumpSound = nullptr; std::cout << "Jump sound freed." << std::endl;}

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
        std::cout << "Font closed." << std::endl;
    }

    std::cout << "Asset cleanup complete." << std::endl;
}