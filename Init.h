#ifndef INIT_H
#define INIT_H

#include <SDL2/SDL.h>

bool initializeSDL(SDL_Window*& window, SDL_Renderer*& renderer);
void shutdownSDL(SDL_Window* window, SDL_Renderer* renderer);

#endif // INIT_H