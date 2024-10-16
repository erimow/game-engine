//
//  Entity.h
//  CSDL
//
//  Created by Erik Morris on 7/17/24.
//

#ifndef Entity_h
#define Entity_h

#include "Camera.h"
#include "Texture.h"
#include <SDL2/SDL.h>
#include <stdio.h>

typedef struct {
  Texture spriteSheet;
  // default vars
  float xPos, yPos;
  float xVel, yVel;
  float width, height;
  float entityVelocity;
  double entityRotation;
  SDL_FRect collider; // also used to render entity: should separate
  // physics vars
  float jumpStrength, gravity, friction, maxXVel;
  // flags
  Uint8 left, right, up, down, onGround;
  bool isPhysics;

  // background entity stuff
  float depth;

  // Animation stuff
  Uint8 clipLength, currentAnimationFrame;
  SDL_Rect *clip;
} Entity;

extern void Entity_init(Entity *entity, float xPos, float yPos, float width,
                        float height, float velocity, Uint8 spriteAmount);
extern void Entity_initPhysics(Entity *entity, float xPos, float yPos,
                               float width, float height, float velocity,
                               float jumpStr, float grav, float frict,
                               float maxXVel, Uint8 spriteAmount);
extern void Entity_free(Entity *entity, bool freeClip);
extern bool Entity_setTexture(Entity *entity, SDL_Renderer *renderer,
                              const char *path);
extern void Entity_getPosition(Entity *entity, int *x, int *y);
extern void Entity_render(Entity *entity, SDL_Renderer *renderer,
                          SDL_Rect *clip, Uint8 clipToRender,
                          SDL_FPoint *center, SDL_RendererFlip flip,
                          Camera *camera, float depthZ);
extern void Entity_setPosition(Entity *entity, int x, int y);
extern void Entity_setRotation(Entity *entity, double rotation);
extern void Entity_updateCollider(Entity *entity);
extern void Entity_setBaseVelocity(Entity *entity, float newVelocity);
extern void Entity_move(Entity *entity, SDL_FRect colliders[],
                        int collidersSize);
extern void Entity_handleEvent(Entity *entity, SDL_Event *e);
extern bool Entity_checkCollision(Entity *entity, SDL_FRect rect);

// getters
extern bool Entity_onGround(Entity *entity);

#endif /* Entity_h */
