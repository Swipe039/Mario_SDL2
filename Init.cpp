#include "Init.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include "Constants.h" // Cần cho SCREEN_WIDTH, SCREEN_HEIGHT

// Hàm khởi tạo
bool initializeSDL(SDL_Window*& window, SDL_Renderer*& renderer) {
    // --- 1. Khởi tạo SDL và các thư viện con ---
    std::cout << "Initializing SDL..." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "Initializing SDL_ttf..." << std::endl;
    if (TTF_Init() < 0) {
        std::cerr << "TTF Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit(); // Chỉ cần quit SDL chính
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
    std::cout << "SDL Subsystems Initialized." << std::endl;

    // --- 2. Tạo Window ---
    std::cout << "Creating Window..." << std::endl;
    window = SDL_CreateWindow("Super Mario SDL2",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window Creation Error: " << SDL_GetError() << std::endl;
        Mix_CloseAudio();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return false;
    }
    std::cout << "Window Created." << std::endl;

    // --- 3. Tạo Renderer ---
    std::cout << "Creating Renderer..." << std::endl;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer Creation Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window); // Dọn dẹp window đã tạo
        Mix_CloseAudio();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return false;
    }
    std::cout << "Renderer Created." << std::endl;
    std::cout << "SDL Initialization Complete." << std::endl;
    return true; // Tất cả thành công
}

// Hàm dọn dẹp
void shutdownSDL(SDL_Window* window, SDL_Renderer* renderer) {
    std::cout << "Shutting down SDL..." << std::endl;

    // 1. Dọn dẹp renderer và window (Thứ tự quan trọng)
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr; // Không thực sự cần thiết vì hàm này không trả về gì
        std::cout << "Renderer destroyed." << std::endl;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
        std::cout << "Window destroyed." << std::endl;
    }

    // 2. Đóng hệ thống audio
    Mix_CloseAudio(); // Nên gọi trước khi Quit các hệ thống khác
    std::cout << "SDL_mixer closed." << std::endl;

    // 3. Thoát các thư viện con của SDL
    IMG_Quit();
    std::cout << "SDL_image quit." << std::endl;
    TTF_Quit();
    std::cout << "SDL_ttf quit." << std::endl;

    // 4. Thoát SDL chính
    SDL_Quit();
    std::cout << "SDL quit." << std::endl;
    std::cout << "SDL Shutdown Complete." << std::endl;
}