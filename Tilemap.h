//
//  Tilemap.h
//  CSDL
//
//  Created by Erik Morris on 7/23/24.
//

#ifndef Tilemap_h
#define Tilemap_h

#include "Camera.h"
#include "Entity.h"
#include "Texture.h"
#include "constants.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

typedef struct {
  int xGridPos, yGridPos;
  int spriteXId, spriteYId;
  double rotation;
  SDL_FRect pos;
  SDL_RendererFlip flip;
} Tile;

typedef struct {
  Tile *tiles;
  Entity *gridEntities;
  Uint8 tilesInGrid;
  Uint8 numGridEntities;
  bool isEntityCheckingGrid;
} Grid;

typedef struct {
  Texture *spriteSheet;
  //    Tile** tiles;
  //    Uint8* tilesInGrid;
  Grid *grid;
  int gridAmount, gridWidth, gridHeight;

  char *tileTypesForParse; // a char array holding the ascii values of tile
                           // types
  int numTileTypesForParse;       // num of tile types
  char *entityTileTypesForParse;  // same but for entity types
  int numEntityTileTypesForParse; // yes
  SDL_FRect *colliders;           // this is generally unused
  int tileAmount;                 // set inside of tilemap_populate
  int tilesPerGrid; // Needs to be cleanly square rootable - Also works best if
                    // the square root is an even number

  int mapWidth, mapHeight;
  float scale;
  Entity *levelEntities;
  int entityAmount;
  bool isGravityTrue, displayGrid;
} Tilemap;

extern void Tile_init(Tile *tile, int xGridPos, int yGridPos, int spriteXId,
                      int spriteYId, int tileSize, int mapHeight, float scale,
                      double rotation, SDL_RendererFlip flip);
extern void Tile_free(Tile *tile);
extern void Tilemap_init(Tilemap *tm, Texture *text, bool isGravityTrue,
                         float scale, int tilesPerGrid, char *fileName,
                         char *tileTypes, int numTileTypes,
                         char *entityTypesForParse, int numEntityTypesForParse,
                         Entity *entityTypes, int entityTypesAmount);
extern void Tilemap_free(Tilemap *tm);
extern void Tilemap_populate(Tilemap *tm, char *file, Entity *entityTypes,
                             int entityTypesAmount);
extern void
Tilemap_render(Tilemap *tm, SDL_Renderer *renderer,
               Camera *camera); // last two were for rendering testing
extern SDL_FRect *Tilemap_getColliders(Tilemap *tm);
extern SDL_FRect **Tilemap_getCollidersAroundEntity(Tilemap *tm, Entity *entity,
                                                    Uint8 *colliderAmount);
extern bool Tilemap_isTileParseable(Tilemap *tm, char tileToParse, int *tileId);
extern bool Tilemap_isEntityTileParseable(Tilemap *tm, char tileToParse,
                                          int *entityId);
extern float Tilemap_getMapWidthPixels(Tilemap *tm);
extern float Tilemap_getMapHeightPixels(Tilemap *tm);
#endif /* Tilemap_h */
