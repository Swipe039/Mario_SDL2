#include "Init.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include "Constants.h" 

bool initializeSDL(SDL_Window*& window, SDL_Renderer*& renderer) {

    std::cout << "Initializing SDL..." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "Initializing SDL_ttf..." << std::endl;
    if (TTF_Init() < 0) {
        std::cerr << "TTF Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit(); 
        return false;
    }

    std::cout << "Initializing SDL_image..." << std::endl;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG Init Error: " << IMG_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    std::cout << "Initializing SDL_mixer..." << std::endl;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Mixer Init Error: " << Mix_GetError() << std::endl;
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return false;
    }



    window = SDL_CreateWindow("Super Mario SDL2",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (!window) {

        Mix_CloseAudio();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return false;
    }



    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {

        SDL_DestroyWindow(window); 
        Mix_CloseAudio();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return false;
    }


    return true; 
}

void shutdownSDL(SDL_Window* window, SDL_Renderer* renderer) {


    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr; 

    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    Mix_CloseAudio(); 
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}