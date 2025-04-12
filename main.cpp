#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "Entity.h"
#include "GameLogic.h"
#include "Renderer.h"
#include "Constants.h"
#include "Control.h"
#include "Init.h"
#include "Texture.h"
#include "RenderGameState.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[]) {
    srand(static_cast<unsigned int>(time(0)));

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!initializeSDL(window, renderer)) {
        std::cerr << "Failed to initialize SDL. Exiting." << std::endl;
        return -1;
    }

    font = TTF_OpenFont("Font/SuperMario256.ttf", 24);
    if (!font) {
        std::cerr << "Font Loading Error: " << TTF_GetError() << std::endl;
        shutdownSDL(window, renderer);
        return -1;
    }

    if (!Load_textures(renderer)) {
        std::cerr << "Failed to load textures. Exiting." << std::endl;
        Clean_up_textures();
        TTF_CloseFont(font);
        shutdownSDL(window, renderer);
        return -1;
    }

    Mix_Music *gameplayMusic = Mix_LoadMUS("Sound/Ground-Theme.wav");
    Mix_Music *menuMusic = Mix_LoadMUS("Sound/Underground-Theme.wav");
    if (!gameplayMusic) {
        std::cerr << "Failed to load Ground-Theme.wav! Mix_Error: " << Mix_GetError() << std::endl;
    }
    if (!menuMusic) {
        std::cerr << "Failed to load Underground-Theme.wav! Mix_Error: " << Mix_GetError() << std::endl;
    }

    GameState state = MAIN_MENU;
    GameState nextState = MAIN_MENU;
    bool running = true;
    int selectedMap = 0;
    int currentMap = 0;
    int selectedMenuItem = 0;

    Entity mario(100, 100, 50, 50, 0, 0, false);
    int playerHearts = MAX_HEARTS;
    std::vector<Entity> enemies;
    std::vector<bool> enemiesAlive;
    std::vector<SDL_Rect> stars;
    std::vector<Cloud> clouds;
    std::vector<SDL_Rect> coins;
    int collectedCoins = 0;
    int totalCoinsInMap = 0;
    int cameraX = 0;
    bool dropThrough = false;
    int score = 0;
    Mix_Music* currentlyPlayingMusic = nullptr;


    while (running) {
        SDL_Event e;
        if (state != nextState) {
            state = nextState;
            selectedMenuItem = 0;
            if (Mix_PlayingMusic()) {
                 Mix_HaltMusic();
                 currentlyPlayingMusic = nullptr;
            }
            if (state == MAP_SELECT) {
                selectedMap = currentMap;
            }
        }
        Mix_Music* desiredMusic = nullptr;
        if (state == GAMEPLAY) {
            desiredMusic = gameplayMusic;
        } else if (state == MAIN_MENU || state == MAP_SELECT || state == WIN_SCREEN || state == LOSE_SCREEN) {
            desiredMusic = menuMusic;
        }
        if (desiredMusic != nullptr && currentlyPlayingMusic != desiredMusic) {
            if (Mix_PlayingMusic()) {
                Mix_HaltMusic();
            }
            Mix_PlayMusic(desiredMusic, -1);
            currentlyPlayingMusic = desiredMusic;
        }
        else if (desiredMusic == nullptr && Mix_PlayingMusic()) {
            Mix_HaltMusic();
            currentlyPlayingMusic = nullptr;
        }
        switch (state) {
            case MAIN_MENU:
                 handleMainMenuInput(e, nextState, running, selectedMenuItem);
                 renderMenu(renderer, font, selectedMenuItem);
                 break;

             case MAP_SELECT:
                resetGameLevel(mario, enemies, enemiesAlive, stars, clouds, coins, collectedCoins, totalCoinsInMap, cloudTexture1, cloudTexture2, score, currentMap, playerHearts);
                cameraX = 0;

                handleMapSelectInput(e, nextState, running, selectedMap);
                if (nextState == GAMEPLAY) {
                    currentMap = selectedMap;
                    score = 0;
                    resetGameLevel(mario, enemies, enemiesAlive, stars, clouds, coins, collectedCoins, totalCoinsInMap, cloudTexture1, cloudTexture2, score, currentMap, playerHearts);
                    cameraX = 0;
                }
                renderMapSelect(renderer, font, selectedMap);
                 break;

            case GAMEPLAY: {
                int heartsBeforeUpdate = playerHearts;
                handleInput(mario, running, dropThrough, nextState);
                updateEntity(mario, enemies, enemiesAlive, running, dropThrough, nextState, stars, coins, collectedCoins, totalCoinsInMap, tilemaps[currentMap], score, playerHearts);
                if (state == GAMEPLAY && playerHearts < heartsBeforeUpdate) {
                     cameraX = 0;
                     std::cout << "Camera reset after respawn." << std::endl;
                }
                if (state == GAMEPLAY){
                    for (size_t i = 0; i < enemies.size(); i++) {
                        if (enemiesAlive[i]) {
                            enemiesAlive[i] = updateEnemy(enemies[i], enemiesAlive[i], cameraX, tilemaps[currentMap]);
                        }
                    }
                }


                 if (state == GAMEPLAY){
                    cameraX = mario.rect.x - SCREEN_WIDTH / 3;
                    cameraX = std::max(0, std::min(cameraX, MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH));
                 }

                render(renderer, mario, enemies, enemiesAlive, cameraX, stars, clouds, cloudTexture1, cloudTexture2,
                       pipeTopTexture, pipeBodyTexture, coinTexture, coins, collectedCoins, currentMap, font,
                       groundTexture, flyBlockTexture, starTexture,enemyTextures,
                       jumpLeftTextures, jumpRightTextures, runLeftTextures, runRightTextures,
                       standLeftTexture, standRightTexture,
                       finishLineTexture, score,
                       playerHearts, heartTexture);
                SDL_RenderPresent(renderer);
                break;
            }

            case PAUSE_MENU:
                 handlePauseMenuInput(e, nextState, running, selectedMenuItem);
                 render(renderer, mario, enemies, enemiesAlive, cameraX, stars, clouds, cloudTexture1, cloudTexture2,
                        pipeTopTexture, pipeBodyTexture, coinTexture, coins, collectedCoins, currentMap, font,
                        groundTexture, flyBlockTexture, starTexture,
                        enemyTextures, jumpLeftTextures, jumpRightTextures, runLeftTextures, runRightTextures,
                        standLeftTexture, standRightTexture,
                        finishLineTexture, score,
                        playerHearts, heartTexture);
                 renderPauseMenu(renderer, font, selectedMenuItem);
                 SDL_RenderPresent(renderer);
                 break;

            case WIN_SCREEN:
                  handleWinLoseInput(e, state, nextState, running, selectedMenuItem);

                  if (nextState == GAMEPLAY || nextState == MAP_SELECT) {
                      score = 0;
                      resetGameLevel(mario, enemies, enemiesAlive, stars, clouds, coins, collectedCoins, totalCoinsInMap, cloudTexture1, cloudTexture2, score, currentMap, playerHearts);
                      cameraX = 0;
                  }
                  renderWinScreen(renderer, font, score, selectedMenuItem);
                 break;

             case LOSE_SCREEN:
                  handleWinLoseInput(e, state, nextState, running, selectedMenuItem);

                  if (nextState == GAMEPLAY || nextState == MAP_SELECT) {
                     score = 0;
                     resetGameLevel(mario, enemies, enemiesAlive, stars, clouds, coins, collectedCoins, totalCoinsInMap, cloudTexture1, cloudTexture2, score, currentMap, playerHearts);
                     cameraX = 0;
                  }
                  renderLoseScreen(renderer, font, score, selectedMenuItem);
                  break;
        }
    }

    std::cout << "Exiting game loop. Cleaning up all resources..." << std::endl;
    Clean_up_textures();
    if (gameplayMusic) { Mix_FreeMusic(gameplayMusic); gameplayMusic = nullptr; std::cout << "Gameplay music freed." << std::endl;}
    if (menuMusic) { Mix_FreeMusic(menuMusic); menuMusic = nullptr; std::cout << "Menu music freed." << std::endl;}
    shutdownSDL(window, renderer);
    std::cout << "Cleanup complete. Exiting application." << std::endl;
    return 0;
}