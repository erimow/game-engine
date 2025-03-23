//
//  BackgroundEntity.h
//  CSDL
//
//  Created by Erik Morris on 7/22/24.
//

#ifndef BackgroundEntity_h
#define BackgroundEntity_h

#include "Camera.h"
#include "Entity.h"
#include "constants.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
  int amount;
  Entity *entity;
} BackgroundEntity;

extern void BackgroundEntity_init(BackgroundEntity *bge, Entity *entity,
                                  int amount);
extern void BackgroundEntity_free(BackgroundEntity *bge);
extern void BackgroundEntity_update(BackgroundEntity *bge,
                                    SDL_Renderer *renderer, Camera *camera,
                                    int frameCount, Uint16 animationInterval);

#endif /* BackgroundEntity_h */
