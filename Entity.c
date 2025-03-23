//
//  Entity.c
//  CSDL
//
//  Created by Erik Morris on 7/17/24.
//

#include "Entity.h"
#include "Camera.h"
#include "constants.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_stdinc.h>
#include "efuncs.h"

void Entity_init(Entity *entity, float xPos, float yPos, float spriteWidth,
                 float spriteHeight, float colWidth, float colHeight,
                 SDL_FPoint colliderOffset, float velocity,
                 Uint8 spriteAmount) {
  entity->xVel = 0;
  entity->yVel = 0;
  entity->left = 0;
  entity->right = 0;
  entity->up = 0;
  entity->down = 0;
    entity->jump = 0;
  entity->xPos = xPos;
  entity->yPos = yPos;
  entity->colliderOffset = colliderOffset;
  // entity->width = width;

  entity->entityRotation = 0.0;
  entity->collider =
      (SDL_FRect){entity->xPos + colliderOffset.x,
                  entity->yPos + colliderOffset.y, colWidth, colHeight};
  entity->spriteRenderRect =
      (SDL_FRect){entity->xPos, entity->yPos, spriteWidth, spriteHeight};
  entity->entityVelocity = velocity;
  entity->currentAnimationFrame = 0;
  entity->animationSpeed = 0;
  Texture_init(&entity->spriteSheet);
  entity->clipLength = spriteAmount;
  entity->clip = (SDL_Rect *)malloc(sizeof(SDL_Rect) * spriteAmount);
  if (entity->clip == NULL)
    printf("clip not properly malloced!\n");
  entity->currentControlType = MAK;
  entity->isPhysics = false;
}
void Entity_initPhysics(Entity *entity, float xPos, float yPos,
                        float spriteWidth, float spriteHeight, float colWidth,
                        float colHeight, SDL_FPoint colliderOffset,
                        float velocity, float jumpStr, float grav, float frict,
                        float maxXVel, Uint8 spriteAmount) {
  entity->xVel = 0;
  entity->yVel = 0;
  entity->xPos = xPos;
  entity->yPos = yPos;
  // entity->spriteWidth = spriteWidth;
  // entity->spriteHeight = spriteHeight;
  entity->entityRotation = 0.0;
  entity->collider =
      (SDL_FRect){entity->xPos + colliderOffset.x,
                  entity->yPos + colliderOffset.y, colWidth, colHeight};
  entity->spriteRenderRect =
      (SDL_FRect){entity->xPos, entity->yPos, spriteWidth, spriteHeight};

  entity->entityVelocity = velocity;
  entity->currentAnimationFrame = 0;
  entity->animationSpeed = 0;
  // init physics
  entity->jumpStrength = jumpStr;
  entity->gravity = grav;
  entity->friction = frict;
  entity->maxXVel = maxXVel;
  entity->left = 0;
  entity->right = 0;
  entity->up = 0;
  entity->down = 0;
  entity->onGround = 0;
    entity->jump = 0;
  Texture_init(&entity->spriteSheet);
  entity->clipLength = spriteAmount;
  entity->clip = (SDL_Rect *)malloc(sizeof(SDL_Rect) * spriteAmount);
  if (entity->clip == NULL)
    printf("clip not properly malloced!\n");

  entity->currentControlType = MAK;
  entity->isPhysics = true;
}
void Entity_free(Entity *entity, bool freeClip) {
  entity->xVel = 0;
  entity->yVel = 0;
  entity->xPos = 0;
  entity->yPos = 0;
  // entity->width = 0;
  // entity->height = 0;
  entity->spriteRenderRect = (SDL_FRect){0, 0, 0, 0};
  entity->collider = (SDL_FRect){0, 0, 0, 0};
  entity->entityRotation = 0.0;
  entity->entityVelocity = 0;
  entity->currentAnimationFrame = 0;
  entity->animationSpeed = 0;

  // free physics
  entity->jumpStrength = 0;
  entity->gravity = 0;
  entity->friction = 0;
  entity->maxXVel = 0;
  entity->left = 0;
  entity->right = 0;
  entity->up = 0;
  entity->down = 0;
  entity->onGround = 0;
  Texture_free(&entity->spriteSheet);
  entity->isPhysics = false;
  entity->clipLength = 0;
  if (entity->clip != NULL && freeClip) {
    free(entity->clip);
    entity->clip = NULL;
  }
}
bool Entity_setTexture(Entity *entity, SDL_Renderer *renderer,
                       const char *path) {
  return Texture_loadFromFile(&entity->spriteSheet, renderer, path);
}

void Entity_render(Entity *entity, SDL_Renderer *renderer, SDL_Rect *clip,
                   Uint8 clipToRender, SDL_FPoint *center,
                   SDL_RendererFlip flip, Camera *camera, float depthZ) {
  SDL_FRect pos;
  if (camera != NULL)
    pos = (SDL_FRect){entity->xPos - (Camera_getObjectXOffset(camera) / depthZ),
                      entity->yPos - (Camera_getObjectYOffset(camera) / depthZ),
                      entity->spriteRenderRect.w, entity->spriteRenderRect.h};
  else
    pos = (SDL_FRect){entity->xPos / depthZ, entity->yPos / depthZ,
                      entity->spriteRenderRect.w, entity->spriteRenderRect.h};

  if (clip != NULL) {
    Texture_render(&entity->spriteSheet, renderer, clip, &pos,
                   entity->entityRotation, center, flip);
  } else {
    Texture_render(&entity->spriteSheet, renderer, &entity->clip[clipToRender],
                   &pos, entity->entityRotation, center, flip);
  }
}
void Entity_setPosition(Entity *entity, int x, int y) {
  entity->xPos = x;
  entity->yPos = y;
}
void Entity_setRotation(Entity *entity, double rotation) {
  entity->entityRotation = rotation;
}
void Entity_setBaseVelocity(Entity *entity, float newVelocity) {
  entity->entityVelocity = newVelocity;
}
void Entity_move(Entity *entity, SDL_FRect *colliders[], int size) {
  if (entity->isPhysics) {
    entity->xVel += ((entity->right * entity->entityVelocity) -
                     (entity->left * entity->entityVelocity));
    entity->xVel *= entity->friction;

    if (entity->xVel >= entity->maxXVel)
      entity->xVel = entity->maxXVel;
    if (entity->xVel <= -entity->maxXVel)
      entity->xVel = -entity->maxXVel;
    if (entity->xVel < .1f && entity->xVel > -.1f)
      entity->xVel = 0;
      
      
if (entity->jump>0 && entity->yVel<0 && entity->onGround!=1)
        entity->jump = (entity->jump-0.04f <= 0) ? 0 : entity->jump-0.04f;
      if (entity->onGround==1 && entity->jump!=0){
          entity->jump=1;
      }
      else if (entity->yVel>=0){
          entity->jump=0;
      }
      entity->yVel -= (entity->onGround * entity->jump* entity->jumpStrength) -
      entity->gravity + entity->jump;
      
    printf("entity->jump = %.02f\n", entity->jump);
      
    //if (entity->yVel < 0)
     // entity->onGround = 0;

    if (entity->yVel != 0)
      entity->onGround = 0;
  } else {
    if (entity->up == 0 && entity->down == 0) {
      entity->yVel = 0;
    }
    if (entity->left == 0 && entity->right == 0) {
      entity->xVel = 0;
    }
  }
  entity->xPos += entity->xVel;
  entity->collider.x = entity->xPos;
  for (int i = 0; i < size; i++)
    if (Entity_checkCollision(entity, *colliders[i])) {
      //            entity->xPos -= entity->xVel;
      if (entity->xVel < 0)
        entity->xPos = colliders[i]->x + colliders[i]->w;
      else if (entity->xVel > 0)
        entity->xPos = colliders[i]->x - entity->collider.w;
      if (entity->isPhysics)
        entity->xVel = 0;
      entity->collider.x = entity->xPos;
    }
  entity->yPos += entity->yVel;
  entity->collider.y = entity->yPos;
  for (int i = 0; i < size; i++)
    if (Entity_checkCollision(entity, *colliders[i])) {
      // for phyisics | stuff
      //              V
      if (entity->yVel > 0) {
        entity->yPos = colliders[i]->y - entity->collider.h;
        entity->onGround = 1;
      } else if (entity->yVel < 0) {
        entity->yPos = colliders[i]->y + colliders[i]->h;
      }

      // entity->yPos -= entity->yVel;
      if (entity->isPhysics)
        entity->yVel = 0;
      entity->collider.y = entity->yPos;
    }
  // printf("onGround: %d\n", entity->onGround);
  if (colliders != NULL) {
    free(colliders);
  }
}
void Entity_handleEvent(Entity *entity, SDL_Event *e) {
  // MAK
  if (e->type == SDL_KEYDOWN && e->key.repeat == 0) {
    entity->currentControlType = MAK;
    if (!entity->isPhysics) {
      switch (e->key.keysym.sym) {
      case SDLK_SPACE:
      case SDLK_UP:
      case SDLK_w:
        entity->yVel -= entity->entityVelocity;
        entity->up = 1;
        break;
      case SDLK_DOWN:
      case SDLK_s:
        entity->yVel += entity->entityVelocity;
        entity->down = 1;
        break;
      case SDLK_RIGHT:
      case SDLK_d:
        entity->xVel += entity->entityVelocity;
        entity->right = 1;
        break;
      case SDLK_LEFT:
      case SDLK_a:
        entity->xVel -= entity->entityVelocity;
        entity->left = 1;
        break;
      }
    } else {
      switch (e->key.keysym.sym) {
      case SDLK_SPACE:
              if(entity->onGround==1)
                 entity->jump = 1;
              break;
      case SDLK_UP:
      case SDLK_w:
        entity->up = 1;
        break;
      case SDLK_DOWN:
      case SDLK_s:
        entity->down = 1;

        break;
      case SDLK_RIGHT:
      case SDLK_d:
        entity->right = 1;
        break;
      case SDLK_LEFT:
      case SDLK_a:
        entity->left = 1;
        break;
      }
    }
  } else if (e->type == SDL_KEYUP && e->key.repeat == 0) {
    if (!entity->isPhysics) {
      switch (e->key.keysym.sym) {
      case SDLK_SPACE:
      case SDLK_UP:
      case SDLK_w:
        entity->yVel += entity->entityVelocity;
        entity->up = 0;
        break;
      case SDLK_DOWN:
      case SDLK_s:
        entity->yVel -= entity->entityVelocity;
        entity->down = 0;
        break;
      case SDLK_RIGHT:
      case SDLK_d:
        entity->xVel -= entity->entityVelocity;
        entity->right = 0;
        break;
      case SDLK_LEFT:
      case SDLK_a:
        entity->xVel += entity->entityVelocity;
        entity->left = 0;
        break;
      }
    }

    else {
      switch (e->key.keysym.sym) {
      case SDLK_SPACE:
              entity->jump = 0;
              break;
      case SDLK_UP:
      case SDLK_w:
        entity->up = 0;
        break;
      case SDLK_DOWN:
      case SDLK_s:
        entity->down = 0;

        break;
      case SDLK_RIGHT:
      case SDLK_d:
        entity->right = 0;
        break;
      case SDLK_LEFT:
      case SDLK_a:
        entity->left = 0;
        break;
      }
    }
  } else if (e->type == SDL_JOYAXISMOTION) {
    if (e->jaxis.which == 0) // Checking the gamepad at index 0
    {
      if (e->jaxis.axis == 0) // Checking the x axis (0=left joystick X, 1=left
                              // joystick Y, 2=right joystick X, 3=right
                              // joystick Y) Y AXIS IS INVERTED FROM X
      {
          //printf("e->jaxis.value = %d, entity->right = %.02f\n", e->jaxis.value, entity->right);
        if (e->jaxis.value > JOYSTICK_DEADZONE) {
          entity->currentControlType = GAMEPAD;
          entity->right =  maxf((float)e->jaxis.value/30000, 1);
          entity->left = 0;
            
        } else if (e->jaxis.value < -JOYSTICK_DEADZONE) {
          entity->currentControlType = GAMEPAD;
            entity->left = maxf((float)e->jaxis.value/-30000, 1);
          entity->right = 0;
        } else {
          if (entity->currentControlType == GAMEPAD) {
            entity->left = 0;
            entity->right = 0;
          }
        }
      }else if (e->jaxis.axis == 1){ //LEFT STICK Y AXIS
          if (e->jaxis.value > JOYSTICK_DEADZONE) {
          entity->currentControlType = GAMEPAD;
          entity->down =  maxf((float)e->jaxis.value/30000, 1);
          entity->up = 0;
            
        } else if (e->jaxis.value < -JOYSTICK_DEADZONE) {
          entity->currentControlType = GAMEPAD;
            entity->up = maxf((float)e->jaxis.value/-30000, 1);
          entity->down = 0;
        } else {
          if (entity->currentControlType == GAMEPAD) {
            entity->up = 0;
            entity->down = 0;
          }
        }
      }
    }
  } else if (e->type == SDL_JOYBUTTONDOWN) {
    entity->currentControlType = GAMEPAD;
    switch (e->jbutton.button) { // Make enum with buttons?
    case 0:                      // 0 = A button (On linux and switch controller, this is the B button. On Mac it is the proper A button)
            if (entity->onGround==1)
            entity->jump = 1;
      break;
    }
  } else if (e->type == SDL_JOYBUTTONUP) {
    switch (e->jbutton.button) {
    case 0:
            entity->jump = 0; // 0 = A button
      break;
    }
  }
}

void Entity_updateCollider(Entity *entity) {
  entity->collider.x = entity->xPos;
  entity->collider.y = entity->yPos;
  entity->collider.w = entity->collider.w;
  entity->collider.h = entity->collider.h;
}

bool Entity_checkCollision(Entity *entity, SDL_FRect rect) {
  if (entity->collider.x + entity->collider.w > rect.x &&
      entity->collider.x < rect.x + rect.w &&
      entity->collider.y + entity->collider.h > rect.y &&
      entity->collider.y < rect.y + rect.h) {
    return true;
  }
  return false;
}

// Getters
void Entity_getPosition(Entity *entity, int *x, int *y) {
  *x = entity->xPos;
  *y = entity->yPos;
}

bool Entity_onGround(Entity *entity) {
  return (entity->onGround == 1) ? true : false;
}
