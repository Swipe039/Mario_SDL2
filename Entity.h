#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include "Constants.h" // Bao gồm Constants để sử dụng ENEMY_FRAME_DELAY

struct Entity {
    SDL_Rect rect;
    int velocityX;
    int velocityY;
    bool onGround;
    int direction = 1; // 1: phải, -1: trái
    bool isBuffed = false;
    int buffTimer = 0;
    int originalW, originalH;
    bool wasBuffed = false;

    // Thuộc tính hoạt ảnh Mario
    enum AnimationState { STAND, RUN, JUMP };
    AnimationState animState = STAND;
    int animFrame = 0; // Khung hình hiện tại của Mario
    int animTimer = 0; // Bộ đếm thời gian để chuyển khung hình Mario
    int FRAME_DELAY = 5; // Độ trễ giữa các khung Mario

    // --- THÊM: Thuộc tính hoạt ảnh Enemy ---
    int enemyAnimFrame = 0; // Khung hình hiện tại của Enemy (0 hoặc 1)
    int enemyAnimTimer = 0; // Bộ đếm thời gian Enemy
    // Sử dụng ENEMY_FRAME_DELAY từ Constants.h

    Entity(int x, int y, int w, int h, int velX, int velY, bool onG)
        : rect{x, y, w, h}, velocityX(velX), velocityY(velY), onGround(onG), originalW(w), originalH(h) {}

    SDL_Rect getRect() const { return rect; }
};

#endif