//
//  Camera.c
//  CSDL
//
//  Created by Erik Morris on 7/19/24.
//

#include "Camera.h"
#include "constants.h"

void Camera_init(Camera *camera) {
  camera->xPos = 0;
  camera->yPos = 0;
  camera->xObjOffset = 0;
  camera->yObjOffset = 0;
  camera->xCamOffset = 0;
  camera->yCamOffset = 0;
}

void Camera_free(Camera *camera) {
  camera->xPos = 0;
  camera->yPos = 0;
  camera->xObjOffset = 0;
  camera->yObjOffset = 0;
  camera->xCamOffset = 0;
  camera->yCamOffset = 0;
}

void Camera_setPosition(Camera *camera, float x, float y) {
  camera->xPos = x;
  camera->yPos = y;
  Camera_updateObjectOffset(camera);
}
void Camera_updateObjectOffset(
    Camera *camera) // updates the obj offset variables
{
  camera->xObjOffset =
      (camera->xPos - (float)SCREEN_WIDTH / 2) + camera->xCamOffset;
  camera->yObjOffset =
      (camera->yPos - (float)SCREEN_HEIGHT / 2) + camera->yCamOffset;
}

void Camera_setCameraOffset(Camera *camera, float x, float y) {
  camera->xCamOffset = x;
  camera->yCamOffset = y;
}

void Camera_getPosition(Camera *camera, float *xPos, float *yPos) {
  *xPos = camera->xPos;
  *yPos = camera->yPos;
}

void Camera_getObjectOffset(Camera *camera, float *xPos, float *yPos) {
  *xPos = camera->xObjOffset;
  *yPos = camera->yObjOffset;
}
float Camera_getObjectXOffset(Camera *camera) { return camera->xObjOffset; }
float Camera_getObjectYOffset(Camera *camera) { return camera->yObjOffset; }

void Camera_getCameraOffset(Camera *camera, float *x, float *y) {
  *x = camera->xCamOffset;
  *y = camera->yCamOffset;
}
float Camera_getCameraXOffset(Camera *camera) { return camera->xCamOffset; }
float Camera_getCameraYOffset(Camera *camera) { return camera->yCamOffset; }

void Camera_setBounds(Camera *camera, SDL_FRect bounds) {
  if (camera->xObjOffset <= bounds.x) {
    camera->xObjOffset = bounds.x;
  } else if (camera->xObjOffset + SCREEN_WIDTH >= bounds.x + bounds.w) {
    camera->xObjOffset = bounds.x + bounds.w - SCREEN_WIDTH;
  }
  if (camera->yObjOffset <= bounds.y) {
    camera->yObjOffset = bounds.y;
  } else if (camera->yObjOffset + SCREEN_HEIGHT >= bounds.y + bounds.h) {
    camera->yObjOffset = bounds.y + bounds.h - SCREEN_HEIGHT;
  }
}
/*void Camera_setBounds(Camera *camera, SDL_FRect bounds) {
  if (camera->xObjOffset <= bounds.x + camera->xObjOffset) {
    camera->xObjOffset = bounds.x + camera->xObjOffset;
  } else if (camera->xObjOffset + SCREEN_WIDTH >=
             bounds.x + camera->xObjOffset + bounds.w) {
    camera->xObjOffset =
        bounds.x + camera->xObjOffset + bounds.w - SCREEN_WIDTH;
  }
  if (camera->yObjOffset <= bounds.y + camera->yObjOffset) {
    camera->yObjOffset = bounds.y + camera->yObjOffset;
  } else if (camera->yObjOffset + SCREEN_HEIGHT >=
             bounds.y + camera->yObjOffset + bounds.h) {
    camera->yObjOffset =
        bounds.y + camera->yObjOffset + bounds.h - SCREEN_HEIGHT;
  }*/
