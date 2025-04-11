#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h> 
#include <SDL2/SDL_ttf.h>  
#include <string>
#include <vector> 
#include "Constants.h"


extern TTF_Font *font; 
extern SDL_Texture *groundTexture;
extern SDL_Texture *flyBlockTexture;
extern SDL_Texture *starTexture;
extern SDL_Texture *enemyTextures[2];
extern SDL_Texture *standLeftTexture;
extern SDL_Texture *standRightTexture;
extern SDL_Texture *cloudTexture1;
extern SDL_Texture *cloudTexture2;
extern SDL_Texture *pipeTopTexture;
extern SDL_Texture *pipeBodyTexture;
extern SDL_Texture *coinTexture;
extern SDL_Texture *finishLineTexture;
extern SDL_Texture *jumpLeftTextures[7];
extern SDL_Texture *jumpRightTextures[7];
extern SDL_Texture *runLeftTextures[3];
extern SDL_Texture *runRightTextures[3];
extern SDL_Texture *heartTexture;

bool Load_textures(SDL_Renderer* renderer);
void Clean_up_textures();
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);

#endif // TEXTURE_H