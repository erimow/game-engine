//
//  Camera.h
//  CSDL
//
//  Created by Erik Morris on 7/19/24.
//

#ifndef Camera_h
#define Camera_h

#include "constants.h"
#include <SDL2/SDL.h>
#include <stdio.h>

typedef struct {
  float xPos, yPos;
  float xObjOffset, yObjOffset;
  float xCamOffset, yCamOffset;
} Camera;

extern void Camera_init(Camera *camera);
extern void Camera_free(Camera *camera);

extern void Camera_setPosition(Camera *camera, float x, float y);
extern void Camera_setCameraOffset(Camera *camera, float x, float y);
extern void Camera_getPosition(Camera *camera, float *xPos, float *yPos);
extern void
Camera_updateObjectOffset(Camera *camera); // updates the obj offset variables
extern void
Camera_getObjectOffset(Camera *camera, float *xPos,
                       float *yPos); // ObjectOffset is used for offsetting
                                     // objects to the camera while rendering
extern float Camera_getObjectXOffset(Camera *camera);
extern float Camera_getObjectYOffset(Camera *camera);
extern void Camera_getCameraOffset(
    Camera *camera, float *x,
    float *y); // CameraOffset is an offset of the camera. May be obsolite
extern float Camera_getCameraXOffset(Camera *camera);
extern float Camera_getCameraYOffset(Camera *camera);
extern void Camera_setBounds(Camera *camera, SDL_FRect bounds);

#endif /* Camera_h */
