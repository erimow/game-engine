//
//  Camera.c
//  CSDL
//
//  Created by Erik Morris on 7/19/24.
//

#include "Camera.h"

void Camera_init(Camera* camera){
    camera->xPos = 0;
    camera->yPos = 0;
    camera->xCamOffset = 0;
    camera->yCamOffset = 0;
}

void Camera_free(Camera* camera){
    camera->xPos = 0;
    camera->yPos = 0;
    camera->xCamOffset = 0;
    camera->yCamOffset = 0;
}

void Camera_followEntity(Camera* camera, Entity* entity){
    camera->xPos = entity->xPos+(entity->width/2);
    camera->yPos = entity->yPos+(entity->height/2);
}

void Camera_setPosition(Camera* camera, float x, float y){
    camera->xPos = x;
    camera->yPos = y;
}

void Camera_setCameraOffset(Camera* camera, float x, float y){
    camera->xCamOffset = x;
    camera->yCamOffset = y;
}

void Camera_getPosition(Camera* camera, float* xPos, float*yPos){
    *xPos=camera->xPos;
    *yPos=camera->yPos;
}

void Camera_getObjectOffset(Camera* camera, float* xPos, float*yPos){
    *xPos=(camera->xPos-SCREEN_WIDTH/2)+camera->xCamOffset;
    *yPos=(camera->yPos-SCREEN_HEIGHT/2)+camera->yCamOffset;
}

void Camera_getCameraOffset(Camera* camera, float* x, float*y){
    *x = camera->xCamOffset;
    *y = camera->yCamOffset;
}
