#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include "Constants.h" 

struct Entity {
    SDL_Rect rect;
    int velocityX;
    int velocityY;
    bool onGround;
    int direction = 1; 
    bool isBuffed = false;
    int buffTimer = 0;
    int originalW, originalH;
    bool wasBuffed = false;

    enum AnimationState { STAND, RUN, JUMP };
    AnimationState animState = STAND;
    int animFrame = 0; 
    int animTimer = 0; 
    int FRAME_DELAY = 5; 

    int enemyAnimFrame = 0; 
    int enemyAnimTimer = 0; 

    Entity(int x, int y, int w, int h, int velX, int velY, bool onG)
        : rect{x, y, w, h}, velocityX(velX), velocityY(velY), onGround(onG), originalW(w), originalH(h) {}

    SDL_Rect getRect() const { return rect; }
};

#endif