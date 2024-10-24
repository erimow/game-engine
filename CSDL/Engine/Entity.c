//
//  Entity.c
//  CSDL
//
//  Created by Erik Morris on 7/17/24.
//

#include "Entity.h"

void Entity_init(Entity* entity, float xPos, float yPos, float width, float height, float velocity, Uint8 spriteAmount){
    entity->xVel=0;
    entity->yVel=0;
    entity->xPos=xPos;
    entity->yPos=yPos;
    entity->width=width;
    entity->height=height;
    entity->entityRotation=0.0;
    SDL_FRect col = {entity->xPos, entity->yPos, entity->width, entity->height};
    entity->collider=col;
    entity->entityVelocity=velocity;
    Texture_init(&entity->spriteSheet);
    entity->clipLength = spriteAmount;
    entity->clip = (SDL_Rect*)malloc(sizeof(SDL_Rect) * spriteAmount);
    if (entity->clip==NULL)
        printf("clip not properly malloced!\n");
    entity->isPhysics=false;
}
void Entity_initPhysics(Entity* entity, float xPos, float yPos, float width, float height, float velocity, float jumpStr, float grav, float frict, float maxXVel, Uint8 spriteAmount){
    entity->xVel=0;
    entity->yVel=0;
    entity->xPos=xPos;
    entity->yPos=yPos;
    entity->width=width;
    entity->height=height;
    entity->entityRotation=0.0;
    SDL_FRect col = {entity->xPos, entity->yPos, entity->width, entity->height};
    entity->collider=col;
    entity->entityVelocity=velocity;
    //init physics
    entity->jumpStrength=jumpStr;
    entity->gravity=grav;
    entity->friction=frict;
    entity->maxXVel=maxXVel;
    entity->left=0;
    entity->right=0;
    entity->up=0;
    entity->onGround=0;
    Texture_init(&entity->spriteSheet);
    entity->clipLength = spriteAmount;
    entity->clip = (SDL_Rect*)malloc(sizeof(SDL_Rect) * spriteAmount);
    if (entity->clip==NULL)
        printf("clip not properly malloced!\n");
    entity->isPhysics=true;
}
void Entity_free(Entity* entity, bool freeClip){
    entity->xVel=0;
    entity->yVel=0;
    entity->xPos=0;
    entity->yPos=0;
    entity->width=0;
    entity->height=0;
    entity->entityRotation=0.0;
    entity->entityVelocity=0;
    SDL_FRect col = {0,0,0,0}; entity->collider = col;
    //free physics
    entity->jumpStrength=0;
    entity->gravity=0;
    entity->friction=0;
    entity->maxXVel=0;
    entity->left=0;
    entity->right=0;
    entity->up=0;
    entity->onGround=0;
    Texture_free(&entity->spriteSheet);
    entity->isPhysics=false;
    entity->clipLength = 0;
    if (entity->clip!=NULL && freeClip){
        free(entity->clip);
        entity->clip=NULL;
    }
}
bool Entity_setTexture(Entity* entity, SDL_Renderer* renderer, const char* path){
    return Texture_loadFromFile(&entity->spriteSheet, renderer, path);
}

void Entity_render(Entity* entity, SDL_Renderer* renderer, SDL_Rect* clip, Uint8 clipToRender, SDL_FPoint* center, SDL_RendererFlip flip, float xOffset, float yOffset){
    SDL_FRect pos = {entity->xPos-(xOffset), entity->yPos-(yOffset), entity->width, entity->height};
    if(clip!=NULL){
        Texture_render(&entity->spriteSheet, renderer, clip, &pos, entity->entityRotation, center, flip);
    }
    else{
        Texture_render(&entity->spriteSheet, renderer, &entity->clip[clipToRender], &pos, entity->entityRotation, center, flip);
    }
}
void Entity_setPosition(Entity* entity, int x, int y){
    entity->xPos=x;
    entity->yPos=y;
}
void Entity_setRotation(Entity* entity, double rotation){
    entity->entityRotation=rotation;
}
void Entity_setBaseVelocity(Entity* entity, float newVelocity){
    entity->entityVelocity = newVelocity;
}
void Entity_move(Entity* entity, SDL_FRect* colliders, int size){
    if (entity->isPhysics)
    {
        entity->xVel += ((entity->right*entity->entityVelocity) - (entity->left * entity->entityVelocity));
        entity->xVel *= entity->friction;
        
        if (entity->xVel >= entity->maxXVel)
            entity->xVel = entity->maxXVel;
        if (entity->xVel <= -entity->maxXVel)
            entity->xVel = -entity->maxXVel;
        if (entity->xVel<.1f && entity->xVel>-.1f)
            entity->xVel = 0;
        
        entity->yVel -= (entity->onGround * entity->up * entity->jumpStrength) - entity->gravity;
        if (entity->yVel<0)
            entity->onGround=0;
         
        if (entity->yVel!=0)
            entity->onGround = 0;
    }
    entity->xPos += entity->xVel;
    entity->collider.x=entity->xPos;
    for (int i = 0; i<size; i++)
        if (Entity_checkCollision(entity, colliders[i]))
        {
//            entity->xPos -= entity->xVel;
            if (entity->xVel<0)
                entity->xPos = colliders[i].x+colliders[i].w;
            else if (entity->xVel>0)
                entity->xPos = colliders[i].x-entity->width;
            entity->xVel = 0;
            entity->collider.x=entity->xPos;
        }
    entity->yPos += entity->yVel;
    entity->collider.y=entity->yPos;
    for (int i = 0; i<size; i++)
        if (Entity_checkCollision(entity, colliders[i]))
        {
            //for phyisics | stuff
            //             V
            if (entity->yVel>0)
            {
                entity->yPos = colliders[i].y-entity->height;
                entity->onGround=1;
            }
            else if (entity->yVel<0)
            {
                entity->yPos = colliders[i].y+colliders[i].h;
            }
            
            //entity->yPos -= entity->yVel;
            
            entity->yVel=0;
            entity->collider.y=entity->yPos;
        }
    //printf("onGround: %d\n", entity->onGround);
    if (colliders!=NULL)
        free(colliders);
}
void Entity_handleEvent(Entity* entity, SDL_Event* e){
    if (e->type == SDL_KEYDOWN && e->key.repeat == 0)
    {
        if (!entity->isPhysics)
        {
            switch (e->key.keysym.sym) {
                case SDLK_SPACE:
                case SDLK_UP:
                case SDLK_w:
                    entity->yVel -= entity->entityVelocity;
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    entity->yVel += entity->entityVelocity;
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    entity->xVel += entity->entityVelocity;
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    entity->xVel -= entity->entityVelocity;
                    break;
            }
        }
        else
        {
            switch (e->key.keysym.sym) {
                case SDLK_SPACE:
                case SDLK_UP:
                case SDLK_w:
                    entity->up=1;
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    entity->right=1;
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    entity->left=1;
                    break;
            }
        }
    }
    else if( e->type == SDL_KEYUP && e->key.repeat == 0 )
        {
            if (!entity->isPhysics)
            {
                switch( e->key.keysym.sym )
                {
                    case SDLK_SPACE:
                    case SDLK_UP:
                    case SDLK_w:
                        entity->yVel += entity->entityVelocity;
                        break;
                    case SDLK_DOWN:
                    case SDLK_s:
                        entity->yVel -= entity->entityVelocity;
                        break;
                    case SDLK_RIGHT:
                    case SDLK_d:
                        entity->xVel -= entity->entityVelocity;
                        break;
                    case SDLK_LEFT:
                    case SDLK_a:
                        entity->xVel += entity->entityVelocity;
                        break;
                        
                }
            }
            
            else
            {
                switch( e->key.keysym.sym )
                {
                    case SDLK_SPACE:
                    case SDLK_UP:
                    case SDLK_w:
                        entity->up=0;
                        break;
                    case SDLK_DOWN:
                    case SDLK_s:
                        
                        break;
                    case SDLK_RIGHT:
                    case SDLK_d:
                        entity->right=0;
                        break;
                    case SDLK_LEFT:
                    case SDLK_a:
                        entity->left=0;
                        break;
                        
                }
            }
        }
}
bool Entity_checkCollision(Entity* entity, SDL_FRect rect){
    if (entity->collider.x+entity->width > rect.x && entity->collider.x < rect.x+rect.w && entity->collider.y+entity->height > rect.y && entity->collider.y < rect.y+rect.h){
        return true;
    }
    return false;
}



// Getters
void Entity_getPosition(Entity* entity, int* x, int* y){
    *x = entity->xPos;
    *y = entity->yPos;
}

bool Entity_onGround(Entity* entity){
    return (entity->onGround==1) ? true : false;
}

