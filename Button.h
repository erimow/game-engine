//
//  Button.h
//  CSDL
//
//  Created by Erik Morris on 7/17/24.
//

#ifndef Button_h
#define Button_h

#include "Texture.h"
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
  SDL_FRect buttonInfo;
  Texture buttonTexture;
  Texture buttonTextToDisplay;
} Button;

void Button_init(Button *button, float x, float y, float w, float h);
void Button_free(Button *button);
void Button_setPosition(Button *button, float x, float y);
bool Button_loadTextures(Button *button, SDL_Renderer *renderer,
                         const char *buttonBackground, const char *buttonText,
                         TTF_Font *gFont, SDL_Color textColor);
void Button_render(Button *button, SDL_Renderer *renderer);
void Button_handleEvent(Button *button, SDL_Event *e, bool *pressed);

#endif /* Button_h */
