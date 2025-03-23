//
//  BackgroundEntity.c
//  CSDL
//
//  Created by Erik Morris on 7/22/24.
//

#include "BackgroundEntity.h"
#include "Camera.h"
#include "Entity.h"

void BackgroundEntity_init(BackgroundEntity *bge, Entity *entity, int amount) {
  bge->amount = amount;
  bge->entity = malloc(sizeof(Entity) * amount);
  // printf("sizeof(*entity)=%ld, sizeof(Entity)=%ld\n", sizeof(*entity),
  // sizeof(Entity));

  if (bge->entity == NULL) {
    printf("bge->entity is equal to NULL!!!\n");
  }

  for (int i = 0; i < amount; i++) {
    bge->entity[i] = *entity;
    // bge->entity[i].isPhysics = entity->isPhysics;
    // bge->entity[i].maxXVel = entity->maxXVel;
    // bge->entity[i].clip = entity->clip;
    // bge->entity[i].clipLength = entity->clipLength;
    // bge->entity[i].spriteSheet = entity->spriteSheet;
    // bge->entity[i].entityRotation = entity->entityRotation;
    // bge->entity[i].collider = entity->collider;
    bge->entity[i].xPos = rand() % SCREEN_WIDTH;
    bge->entity[i].yPos = rand() % SCREEN_HEIGHT;
    bge->entity[i].depth = (((float)(rand() % (50)) / 10) + 1.2f);
    bge->entity[i].xVel = ((float)(rand() % (40)) / 10) + .5f;
    bge->entity[i].yVel = 0;
    bge->entity[i].left =
        (rand() % 2); // USING 'LEFT' TO DETERMINE MOVE DIRECTION
    if (bge->entity[i].left == 1)
      bge->entity[i].xVel = -bge->entity[i].xVel;
    else
      bge->entity[i].right = 1;
    bge->entity[i].spriteRenderRect.w =
        entity->spriteRenderRect.w / (bge->entity[i].depth / 3);
    bge->entity[i].spriteRenderRect.h =
        entity->spriteRenderRect.h / (bge->entity[i].depth / 3);
    bge->entity[i].up =
        ((rand() %
          (TARGET_FPS))); // USING UP FOR ANIMATION OFFSET // CHANGE THIS, AND
                          // ADJUST BASED ON MOVE SPEED // ACTUALLY MAKE AN
                          // ANIMATION SPEED VARIABLE ON TOP OF OFFSET
    // bge->entity[i].animationSpeed = bge->entity[i].xVel
    // bge->entity[i].currentAnimationFrame = 0;
  }

  for (int i = 0; i < bge->amount - 1; i++) {
    for (int j = 0; j < bge->amount - i - 1; j++) {
      if (bge->entity[j].depth < bge->entity[j + 1].depth) {
        // Swap entities
        Entity temp = bge->entity[j];
        bge->entity[j] = bge->entity[j + 1];
        bge->entity[j + 1] = temp;
      }
    }
  }
}
void BackgroundEntity_free(BackgroundEntity *bge) {
  for (int i = 0; i < bge->amount; i++) {
    Entity_free(&bge->entity[i], false);
  }
  free(bge->entity);
  bge->entity = NULL;
  bge->amount = 0;
}
void BackgroundEntity_update(BackgroundEntity *bge, SDL_Renderer *renderer,
                             Camera *camera, int frameCount,
                             Uint16 animationInterval) {

  for (int i = 0; i < bge->amount; i++) {
    SDL_RendererFlip flip =
        (bge->entity[i].left == 1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    Entity_move(&bge->entity[i], NULL, 0);
    if (bge->entity[i].xPos >
        (Camera_getObjectXOffset(camera) / bge->entity[i].depth) + SCREEN_WIDTH)
      bge->entity[i].xPos -= SCREEN_WIDTH + bge->entity[i].spriteRenderRect.w;
    else if (bge->entity[i].xPos <
             (Camera_getObjectXOffset(camera) / bge->entity[i].depth) -
                 bge->entity[i].spriteRenderRect.w)
      bge->entity[i].xPos += SCREEN_WIDTH + bge->entity[i].spriteRenderRect.w;
    if (bge->entity[i].yPos >
        (Camera_getObjectYOffset(camera) / bge->entity[i].depth) +
            SCREEN_HEIGHT)
      bge->entity[i].yPos -= SCREEN_HEIGHT + bge->entity[i].spriteRenderRect.h;
    else if (bge->entity[i].yPos <
             (Camera_getObjectYOffset(camera) / bge->entity[i].depth) -
                 bge->entity[i].spriteRenderRect.h)
      bge->entity[i].yPos += SCREEN_HEIGHT + bge->entity[i].spriteRenderRect.h;
    // Implement y scrolling?
    bge->entity[i].yPos = (int)bge->entity[i].yPos;
    //        printf("Entity %d: xPos = %f, yPos = %f\n", i,
    //        bge->entity[i].xPos, bge->entity[i].yPos);
    if (bge->entity[i].clipLength > 1) {
      if ((frameCount + (int)bge->entity[i].up) %
              (animationInterval - abs((int)bge->entity[i].xVel * 4)) ==
          0) {
        bge->entity[i]
            .currentAnimationFrame++; // TEMP using entity[i].right to hold the
                                      // previous/current animation frame
        if (bge->entity[i].currentAnimationFrame >= bge->entity[i].clipLength)
          bge->entity[i].currentAnimationFrame = 0;
      }
    }
    Entity_render(&bge->entity[i], renderer, NULL,
                  bge->entity[i].currentAnimationFrame, NULL, flip, camera,
                  bge->entity[i].depth);
  }
}
