#ifndef MAPEDITOR
#define MAPEDITOR
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>
#include "Camera.h"
#include "Tilemap.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Tileset{
    Uint8 tilePixelSideLength;
    Uint16 imageWidth, imageHeight;
    char* filename, filelocation;
}Tileset;

bool ME_LoadMap(char* filename);
bool ME_SaveMap(char* filename, char* location);
bool ME_LoadTileSet(Tileset *ts,char* filename, char* location);


#endif
