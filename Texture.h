//
//  Texture.h
//  CSDL
//
//  Created by Erik Morris on 7/16/24.
//

#ifndef Texture_h
#define Texture_h

#include <SDL2/SDL.h>
#ifdef __linux__
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#elif defined __APPLE__
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#endif
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  SDL_Texture *texture;
  unsigned int width;
  unsigned int height;
} Texture;

void Texture_init(Texture *texture);
void Texture_free(Texture *texture);
bool Texture_loadFromFile(Texture *texture, SDL_Renderer *renderer,
                          const char *path);
bool Texture_loadFromRenderedText(Texture *texture, SDL_Renderer *renderer,
                                  TTF_Font *gFont, const char *textureText,
                                  SDL_Color textColor);
void Texture_setColor(Texture *texture, Uint8 red, Uint8 green, Uint8 blue);
void Texture_setBlendMode(Texture *texture, SDL_BlendMode blending);
void Texture_setAlpha(Texture *texture, Uint8 alpha);
void Texture_render(Texture *texture, SDL_Renderer *renderer, SDL_Rect *clip,
                    SDL_FRect *pos, double angle, SDL_FPoint *center,
                    SDL_RendererFlip flip);
int Texture_getWidth(Texture *texture);
int Texture_getHeight(Texture *texture);

#endif /* Texture_h */
