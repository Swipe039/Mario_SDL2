#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>

struct Entity {
    SDL_Rect rect;
    int velocityX;
    int velocityY;
    bool onGround;
    int direction = 1; // 1: phải, -1: trái
    bool isBuffed = false;
    int buffTimer = 0;
    int originalW, originalH;

    // Thuộc tính hoạt ảnh
    enum AnimationState { STAND, RUN, JUMP };
    AnimationState animState = STAND;
    int animFrame = 0; // Khung hình hiện tại
    int animTimer = 0; // Bộ đếm thời gian để chuyển khung hình
    int FRAME_DELAY = 5; // Độ trễ giữa các khung (điều chỉnh tốc độ hoạt ảnh)

    Entity(int x, int y, int w, int h, int velX, int velY, bool onG)
        : rect{x, y, w, h}, velocityX(velX), velocityY(velY), onGround(onG), originalW(w), originalH(h) {}

    SDL_Rect getRect() const { return rect; }
};

#endif