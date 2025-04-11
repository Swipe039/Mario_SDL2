#include "Control.h"
#include <iostream>
#include <algorithm> // For std::max, std::min

// << VIẾT LẠI HOÀN TOÀN handleInput >>
void handleInput(Entity &mario, bool &running, bool &dropThrough, GameState &state) {
    SDL_Event event;

    // Reset trạng thái di chuyển mỗi khi bắt đầu xử lý input mới
    mario.velocityX = 0;
    dropThrough = false;

    // Xử lý tất cả sự kiện trong hàng đợi
    while(SDL_PollEvent(&event)) {
         if (event.type == SDL_QUIT) {
            running = false;
            return; // Thoát ngay khi có sự kiện quit
        }

        // Xử lý nhấn phím xuống
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_w: // Nhảy
                     std::cout << "Attempting Jump. onGround=" << mario.onGround << ", velocityY=" << mario.velocityY << std::endl; // DEBUG
                    if (mario.onGround) {
                        mario.velocityY = JUMP_FORCE;
                        mario.onGround = false; // Quan trọng: Đặt lại ngay để tránh nhảy liên tục
                    }
                    break;
                case SDLK_s: // Rơi xuống platform
                    dropThrough = true;
                    break;
                case SDLK_ESCAPE: // Pause game
                    state = PAUSE_MENU;
                    // Không cần return ở đây, vì vòng lặp chính sẽ xử lý chuyển state
                    break;
                // Lưu ý: Không xử lý A, D ở đây nữa
            }
        }
    }

    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    if (currentKeyStates[SDL_SCANCODE_A]) {
        mario.velocityX = -SPEED;
    }
    if (currentKeyStates[SDL_SCANCODE_D]) {
        // Nếu cả A và D được nhấn, D sẽ ghi đè A (hoặc ngược lại tùy thứ tự if)
        // Hoặc bạn có thể làm: if (!currentKeyStates[SDL_SCANCODE_A]) mario.velocityX = SPEED;
        mario.velocityX = SPEED;
    }

    // Nếu state bị đổi thành PAUSE_MENU trong lúc xử lý sự kiện,
    // mario.velocityX nên được đặt về 0 để dừng di chuyển khi pause
    if (state == PAUSE_MENU) {
        mario.velocityX = 0;
    }
}


// --- Giữ nguyên các hàm handleMainMenuInput, handlePauseMenuInput, handleMapSelectInput, handleWinLoseInput ---
// (Code của các hàm này giống như ở lượt trả lời trước)

void handleMainMenuInput(SDL_Event &e, GameState &state, bool &running, int &selectedItem) {
     GameState nextState = state;
     while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
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
                    if (selectedItem == 0) { // Start
                        nextState = MAP_SELECT;
                    } else { // Exit
                        running = false;
                    }
                    state = nextState;
                    return;
                case SDLK_ESCAPE:
                     running = false;
                     return;
            }
        }
    }
}


void handlePauseMenuInput(SDL_Event &e, GameState &state, bool &running, int &selectedItem) {
    GameState nextState = state;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedItem = (selectedItem - 1 + PAUSE_MENU_ITEM_COUNT) % PAUSE_MENU_ITEM_COUNT;
                    break;
                case SDLK_DOWN:
                    selectedItem = (selectedItem + 1) % PAUSE_MENU_ITEM_COUNT;
                    break;
                case SDLK_RETURN:
                    if (selectedItem == 0) { // Continue
                        nextState = GAMEPLAY;
                    } else { // Main Menu
                        nextState = MAIN_MENU;
                    }
                    state = nextState;
                    return;
                case SDLK_ESCAPE:
                    nextState = GAMEPLAY;
                    state = nextState;
                    return;
            }
        }
    }
}

void handleMapSelectInput(SDL_Event &e, GameState &state, bool &running, int &selectedMap) {
    GameState nextState = state;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }
        if (e.type == SDL_KEYDOWN) {
            int oldSelectedMap = selectedMap;
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedMap -= MAP_SELECT_COLS;
                    break;
                case SDLK_DOWN:
                    selectedMap += MAP_SELECT_COLS;
                    break;
                case SDLK_LEFT:
                    selectedMap--;
                    break;
                case SDLK_RIGHT:
                    selectedMap++;
                    break;
                case SDLK_RETURN:
                    nextState = GAMEPLAY;
                    state = nextState;
                    return;
                case SDLK_ESCAPE:
                    nextState = MAIN_MENU;
                    state = nextState;
                    return;
            }
             selectedMap = std::max(0, std::min(selectedMap, NUM_MAPS - 1));
             // Optional boundary wrapping logic can be kept or removed
        }
    }
}


void handleWinLoseInput(SDL_Event &e, GameState &currentState, GameState &nextState, bool &running, int &selectedItem) {
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedItem = (selectedItem - 1 + WIN_LOSE_MENU_ITEM_COUNT) % WIN_LOSE_MENU_ITEM_COUNT;
                    break;
                case SDLK_DOWN:
                    selectedItem = (selectedItem + 1) % WIN_LOSE_MENU_ITEM_COUNT;
                    break;
                case SDLK_RETURN:
                    if (selectedItem == 0) { // Retry
                        nextState = GAMEPLAY;
                    } else if (selectedItem == 1) { // Map Select
                        nextState = MAP_SELECT;
                    } else { // Quit Game
                        running = false;
                        nextState = currentState;
                    }
                    return;
                case SDLK_ESCAPE:
                    running = false;
                    nextState = currentState;
                    return;
            }
        }
    }
    nextState = currentState;
}