//
//  Tilemap.c
//  CSDL
//
//  Created by Erik Morris on 7/23/24.
//

#include "Tilemap.h"
#include "Camera.h"
#include "constants.h"
#include "efuncs.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <math.h>
#include <stdio.h>

void Tile_init(Tile *tile, int xGridPos, int yGridPos, int spriteXId,
               int spriteYId, int tileSize, int mapHeight, float scale,
               double rotation, SDL_RendererFlip flip) {
  tile->spriteXId = spriteXId;
  tile->spriteYId = spriteYId;
  tile->xGridPos = xGridPos;
  tile->yGridPos = yGridPos;
  tile->flip = flip;
  tile->rotation = rotation;
  tile->pos = (SDL_FRect){(xGridPos * tileSize * scale),
                          -((yGridPos - mapHeight) * tileSize * scale),
                          tileSize * scale, tileSize * scale};
}
void Tile_free(Tile *tile) {}
void Tilemap_init(Tilemap *tm, Texture *text, bool isGravityTrue, float scale,
                  int tilesPerGrid, char *fileName, char *tileTypes,
                  int numTileTypes, char *entityTypesForParse,
                  int numEntityTypesForParse, Entity *entityTypes,
                  int entityTypesAmount) {
  tm->isGravityTrue = isGravityTrue;
  tm->tileTypesForParse = tileTypes;
  tm->numTileTypesForParse = numTileTypes;
  tm->entityTileTypesForParse = entityTypesForParse;
  tm->numEntityTileTypesForParse = numEntityTypesForParse;
  tm->spriteSheet = text;
  tm->scale = scale;
  tm->tilesPerGrid =
      tilesPerGrid; // MUST BE A PERFECT SQUARE VALUE ie 9->(3x3) 16->(4x4) etc
  tm->displayGrid = true;
  tm->levelEntities = NULL;
  tm->grid = NULL;
  tm->entityAmount = 0;
  tm->colliders = NULL;
  Tilemap_populate(tm, fileName, entityTypes, entityTypesAmount);
  //    tm->colliders = Tilemap_getColliders(tm);
}
void Tilemap_free(Tilemap *tm) {
  for (int i = 0; i < tm->gridAmount; i++) {
    if (tm->grid[i].tiles != NULL) {
      free(tm->grid[i].tiles);
      tm->grid[i].tiles = NULL;
      tm->grid[i].tilesInGrid = 0;
    }
    if (tm->grid[i].gridEntities != NULL) {
      free(tm->grid[i].gridEntities);
      tm->grid[i].gridEntities = NULL;
      tm->grid[i].numGridEntities = 0;
      // printf("Freeing in tilemap\n");
    }
  }
  if (tm->grid != NULL) {
    free(tm->grid);
    tm->grid = NULL;
  }
  if (tm->spriteSheet != NULL) {
    Texture_free(tm->spriteSheet);
    tm->spriteSheet = NULL;
  }
  if (tm->colliders != NULL) {
    free(tm->colliders);
    tm->colliders = NULL;
  }
  if (tm->tileTypesForParse != NULL) {
    tm->tileTypesForParse = NULL;
    tm->numTileTypesForParse = 0;
  }
  if (tm->levelEntities != NULL) {
    free(tm->levelEntities);
    tm->levelEntities = NULL;
    tm->entityAmount = 0;
  }
  tm->displayGrid = false;
}
void Tilemap_populate(Tilemap *tm, char *fileName, Entity *entityTypes,
                      int entityTypesAmount) {
  FILE *file = fopen(fileName, "r");
  if (file == NULL) {
    perror("Failed to open file");
    return;
  }

  // Determine file size
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET); // Reset file pointer to the beginning

  // Allocate memory for the file content
  char *content =
      (char *)malloc((fileSize + 1) * sizeof(char)); // +1 for null terminator
  if (content == NULL) {
    perror("Failed to allocate memory");
    fclose(file);
  }

  // Read file content
  int rows = 0;
  size_t totalBytesRead = 0;
  char line[1024];
  while (fgets(line, sizeof(line), file)) {
    size_t len = strlen(line);
    // if (line[len - 1] == '\n')
    // line[len - 1] = '\0'; // Remove newline character
    strcpy(content + totalBytesRead, line);
    rows++;
    totalBytesRead += len - 1; // remove newline character or null terminator
  }
  content[totalBytesRead] = '\0'; // Null-terminate the final string
  // printf("content=%s\n", content);

  tm->mapHeight = rows;
  tm->mapWidth = (int)(totalBytesRead / rows);
  printf("mapHeight: %d\nmapWidth: %d\nTotal bytes: %zu\n", tm->mapHeight,
         tm->mapWidth, totalBytesRead);

  fclose(file);

  int tileSize = TILE_SIZE;
  tm->gridWidth = ((int)((tm->mapWidth % (int)sqrt(tm->tilesPerGrid)) == 0))
                      ? (int)((tm->mapWidth / sqrt(tm->tilesPerGrid)))
                      : (int)((tm->mapWidth / sqrt(tm->tilesPerGrid)) + 1);
  tm->gridHeight = ((int)((tm->mapHeight % (int)sqrt(tm->tilesPerGrid)) == 0))
                       ? ((int)((tm->mapHeight / sqrt(tm->tilesPerGrid))))
                       : ((int)((tm->mapHeight / sqrt(tm->tilesPerGrid)) + 1));
  tm->gridAmount = tm->gridWidth * tm->gridHeight;
  printf("TilesPerGrid = %d\nGrid Amount = %d\nGrid width = %d, Grid height = "
         "%d\n",
         tm->tilesPerGrid, tm->gridAmount, tm->gridWidth, tm->gridHeight);
  Grid *grid = NULL;
  grid = malloc((sizeof(Grid) * tm->gridAmount));
  // printf("sizeof(grid):%ld, * gridamount: %d = %ld\n", sizeof(Grid),
  // tm->gridAmount, sizeof(Grid) * tm->gridAmount);
  if (grid == NULL) {
    printf("grid could not be allocatied\n");
  }
  //    Tile** tiles = NULL;
  //    tiles = malloc(sizeof(Tile*)*tm->gridAmount);
  //    if (tiles == NULL){printf("tiles could not be allocatied\n");}
  //    Uint8* tilesInGrid = malloc(sizeof(Uint8)*tm->gridAmount);
  //    if (tilesInGrid == NULL)
  //        printf("tilesInGrid not malloced\n");

  for (int i = 0; i < tm->gridAmount; i++) {
    grid[i].tiles = malloc(sizeof(Tile) * tm->tilesPerGrid);
    if (grid[i].tiles == NULL) {
      printf("tiles[%d] is NULL!\n", i);
    }
    grid[i].tilesInGrid = 0;
    grid[i].numGridEntities = 0;
    grid[i].isEntityCheckingGrid = false;
    grid[i].gridEntities = malloc(sizeof(Entity) * tm->tilesPerGrid);
    if (grid[i].gridEntities == NULL) {
      printf("Could not malloc grid[%d].gridEntities\n", i);
    }
  }

  // printF("Tiles per grid = %d\n", tm->tilesPerGrid);
  printf("done\n");
  int tileAmount = 0;

  int spriteYId = 0; // Gets back tileId from the function call below
  int spriteXId = 0;
  int entityId = 0;
  // allocate levelEntites
  //    int entityMallocSize = 5;
  //    tm->levelEntities=malloc(sizeof(Entity)*entityMallocSize);
  //    if (tm->levelEntities==NULL){
  //        printf("Could not malloc tm->levelEntities\n");
  //    }
  for (int i = 0; i < totalBytesRead; i++) {
    char tile = content[i];

    if (Tilemap_isTileParseable(tm, tile, &spriteYId)) // If a tile object
    {
      //----------------------------------------------------------------------------
      SDL_RendererFlip fl = SDL_FLIP_NONE;
      double angle = 0.0;
      bool tileLeft = false, tileUp = false, tileRight = false,
           tileDown = false;
      if (i != 0) // check for surrounding tiles
        if (content[i - 1] == tile)
          tileLeft = true;
      if (i - (tm->mapWidth) >= 0)
        if (content[i - tm->mapWidth] == tile)
          tileUp = true;
      if (i != totalBytesRead - 1 && i % tm->mapWidth < tm->mapWidth - 1)
        if (content[i + 1] == tile)
          tileRight = true;
      if (i + tm->mapWidth <= totalBytesRead - 1)
        if (content[i + tm->mapWidth] == tile)
          tileDown = true;
      if (!(tileLeft || tileRight || tileUp || tileDown)) {
        spriteXId = 0;
      } else if (tileLeft && !(tileRight || tileUp || tileDown)) {
        spriteXId = 4;
      } else if (tileRight && !(tileLeft || tileUp || tileDown)) {
        spriteXId = 4;
        fl = SDL_FLIP_HORIZONTAL;
      } else if (tileUp && !(tileLeft || tileRight || tileDown)) {
        spriteXId = 4;
        angle = 90.0;
      } else if (tileDown && !(tileLeft || tileRight || tileUp)) {
        spriteXId = 4;
        angle = 90.0;
        fl = SDL_FLIP_HORIZONTAL;
      } else if (tileLeft && tileRight && !(tileUp || tileDown)) {
        spriteXId = 5;
      } else if (tileUp && tileDown && !(tileLeft || tileRight)) {
        spriteXId = 5;
        angle = 90.0;
      } else if (tileLeft && tileUp && !(tileRight || tileDown)) {
        spriteXId = 2;
        angle = 90.0;
      } else if (tileLeft && tileDown && !(tileRight || tileUp)) {
        spriteXId = 2;
      } else if (tileRight && tileUp && !(tileLeft || tileDown)) {
        spriteXId = 2;
        angle = 180.0;
        // fl = SDL_FLIP_HORIZONTAL;
      } else if (tileRight && tileDown && !(tileLeft || tileUp)) {
        spriteXId = 2;
        fl = SDL_FLIP_HORIZONTAL;
        // fl = SDL_FLIP_HORIZONTAL;
      } else if (tileLeft && tileRight && tileUp && !tileDown) {
        spriteXId = 3;
        angle = 90.0;
      } else if (tileLeft && tileRight && tileDown && !tileUp) {
        spriteXId = 1;
        // angle = 270.0;
      } else if (tileUp && tileDown && tileLeft && !tileRight) {
        spriteXId = 3;
        // angle = 90.0;
      } else if (tileUp && tileDown && tileRight && !tileLeft) {
        spriteXId = 3;
        fl = SDL_FLIP_HORIZONTAL;
      } else if (tileLeft && tileRight && tileUp && tileDown) {
        spriteXId = 0; // Assuming a different sprite for all tiles present
      }
      //---------------------------------------------------------------------------
      //&tiles[index][tilesInGrid[index]++]
      int index = (tm->mapWidth % (int)sqrt(tm->tilesPerGrid) != 0)
                      ? ((int)((i / tm->mapWidth) / sqrt(tm->tilesPerGrid)) *
                         (int)((tm->mapWidth / sqrt(tm->tilesPerGrid)) + 1)) +
                            (int)((i % tm->mapWidth) / sqrt(tm->tilesPerGrid))
                      : ((int)((i / tm->mapWidth) / sqrt(tm->tilesPerGrid)) *
                         (int)((tm->mapWidth / sqrt(tm->tilesPerGrid)))) +
                            (int)((i % tm->mapWidth) / sqrt(tm->tilesPerGrid));
      // printf("tile:%d->x:%d, y:%d\n", i, i % tm->mapWidth,
      //        tm->mapHeight - (i / tm->mapWidth));
      // printf("Grid index-> [%d], [%d]\n", index, grid[index].tilesInGrid);
      Tile_init(&grid[index].tiles[grid[index].tilesInGrid++], i % tm->mapWidth,
                tm->mapHeight - (i / tm->mapWidth), spriteXId, spriteYId,
                tileSize, tm->mapHeight, tm->scale, angle, fl);

      tileAmount++;
    } else if (Tilemap_isEntityTileParseable(tm, tile, &entityId)) {
      int index = (tm->mapWidth % (int)sqrt(tm->tilesPerGrid) != 0)
                      ? ((int)((i / tm->mapWidth) / sqrt(tm->tilesPerGrid)) *
                         (int)((tm->mapWidth / sqrt(tm->tilesPerGrid)) + 1)) +
                            (int)((i % tm->mapWidth) / sqrt(tm->tilesPerGrid))
                      : ((int)((i / tm->mapWidth) / sqrt(tm->tilesPerGrid)) *
                         (int)((tm->mapWidth / sqrt(tm->tilesPerGrid)))) +
                            (int)((i % tm->mapWidth) / sqrt(tm->tilesPerGrid));
      //            if (grid[index].numGridEntities == 0){
      //                grid[index].gridEntities = malloc(sizeof(Entity));
      //                if (grid[index].gridEntities==NULL){
      //                    printf("Reallocing tm->grid[%d] failed!\n", index);
      //                }
      //            }

      //            grid[index].gridEntities = realloc(grid[index].gridEntities,
      //            sizeof(Entity)*grid[index].numGridEntities+1); if
      //            (grid[index].gridEntities==NULL){
      //                printf("Reallocing tm->grid[%d] failed!\n", index);
      //            }
      //            if (entityMallocSize == tm->entityAmount){
      //                entityMallocSize*=2;
      //                tm->levelEntities = realloc(tm->levelEntities,
      //                sizeof(Entity)*entityMallocSize);
      //                if(tm->levelEntities==NULL)
      //                {
      //                    printf("Reallocing tm->levelEntites failed!\n");
      //                }
      //            }
      //            tm->levelEntities[tm->entityAmount] = entityTypes[entityId];
      //            tm->levelEntities[tm->entityAmount].xPos =
      //            (i%tm->mapWidth)*tileSize*tm->scale;
      //            tm->levelEntities[tm->entityAmount].yPos =
      //            (i/tm->mapWidth)*tileSize*tm->scale; tm->entityAmount++;
      grid[index].gridEntities[grid[index].numGridEntities] =
          entityTypes[entityId];
      grid[index].gridEntities[grid[index].numGridEntities].xPos =
          (i % tm->mapWidth) * tileSize * tm->scale;
      grid[index].gridEntities[grid[index].numGridEntities].yPos =
          (i / tm->mapWidth) * tileSize * tm->scale;
      grid[index].numGridEntities++;
    }
  }

  //    if (entityMallocSize!=tm->entityAmount){
  //        tm->levelEntities = realloc(tm->levelEntities,
  //        sizeof(Entity)*tm->entityAmount);
  //    }

  for (int i = 0; i < tm->gridAmount; i++) {
    // printf("grid[%d] -> %d tiles\n", i, grid[i].tilesInGrid);
    // Reallocing tiles in each grid
    if (grid[i].tilesInGrid != 0 && grid[i].tilesInGrid != tm->tilesPerGrid) {
      Tile *newTile =
          realloc(grid[i].tiles, sizeof(Tile) * grid[i].tilesInGrid);
      if (newTile == NULL && grid[i].tilesInGrid != 0) {
        printf(
            "Could not allocate memory to tile[%d] in Tilemap_populate p.2\n",
            i);
        free(grid);
        free(content);
        return;
      }
      grid[i].tiles = newTile;
    } else if (grid[i].tilesInGrid == 0) {
      free(grid[i].tiles);
      grid[i].tiles = NULL;
    }
    // Reallocing grid entities
    if (grid[i].numGridEntities != 0 &&
        grid[i].numGridEntities != tm->tilesPerGrid) {
      grid[i].gridEntities = realloc(grid[i].gridEntities,
                                     sizeof(Entity) * grid[i].numGridEntities);
      if (grid[i].gridEntities == NULL && grid[i].numGridEntities != 0) {
        printf("Could not realloc gridEntities. Grid # -> %i\n", i);
      }
    } else if (grid[i].numGridEntities == 0) {
      free(grid[i].gridEntities);
      grid[i].gridEntities = NULL;
    }
  }
  //    tm->tilesInGrid = tilesInGrid;
  //    tm->tiles=tiles;
  tm->grid = grid;
  tm->tileAmount = tileAmount;
  free(content);
}

void Tilemap_render(Tilemap *tm, SDL_Renderer *renderer, Camera *camera) {
  int tileSize = TILE_SIZE;
  int screenWidth = SCREEN_WIDTH;
  int screenHeight = SCREEN_HEIGHT;
  float xOffset = Camera_getObjectXOffset(camera);
  float yOffset = Camera_getObjectYOffset(camera);
  float camOffX = Camera_getCameraXOffset(camera);
  float camOffY = Camera_getCameraYOffset(camera);
  SDL_Point CameraMinTilePos = {
      (int)((((xOffset - camOffX) / tileSize) / tm->scale)),
      (int)((((yOffset - camOffY) / tileSize) / tm->scale))};
  SDL_Point CameraMinGridPos = {
      (int)((CameraMinTilePos.x) / sqrt(tm->tilesPerGrid)),
      (int)((CameraMinTilePos.y) / sqrt(tm->tilesPerGrid))};
  SDL_Point CameraMaxTilePos = {
      (int)(((((xOffset - camOffX) + screenWidth) / tileSize) / tm->scale)),
      (int)(((((yOffset - camOffY) + screenHeight) / tileSize) / tm->scale))};
  SDL_Point CameraMaxGridPos = {
      (int)((CameraMaxTilePos.x) / sqrt(tm->tilesPerGrid)),
      (int)((CameraMaxTilePos.y) / sqrt(tm->tilesPerGrid))};
  if (CameraMinGridPos.x < 0)
    CameraMinGridPos.x = 0;
  if (CameraMinGridPos.y < 0)
    CameraMinGridPos.y = 0;
  if (CameraMaxGridPos.x >= tm->gridWidth)
    CameraMaxGridPos.x = tm->gridWidth - 1;
  if (CameraMaxGridPos.y >= tm->gridHeight)
    CameraMaxGridPos.y = tm->gridHeight - 1;
  // printf("CameraTilePos = (%d, %d) -- CameraGridPos = (%d,
  // %d)\nCameraMaxTilePos = (%d, %d) -- CameraMaxGridPos = (%d, %d)\n",
  // CameraMinTilePos.x, CameraMinTilePos.y, CameraMinGridPos.x,
  // CameraMinGridPos.y, CameraMaxTilePos.x, CameraMaxTilePos.y,
  // CameraMaxGridPos.x, CameraMaxGridPos.y);

  for (int i = (CameraMinGridPos.y * tm->gridWidth) + CameraMinGridPos.x;
       i <= (CameraMaxGridPos.y * tm->gridWidth) + CameraMaxGridPos.x; i++) {
    if (((i % (tm->gridWidth)) > CameraMaxGridPos.x) ||
        i % tm->gridWidth < CameraMinGridPos.x)
      i += (tm->gridWidth - ((CameraMaxGridPos.x - CameraMinGridPos.x) + 1));
    for (int l = 0; l < tm->grid[i].tilesInGrid; l++) {
      SDL_FRect pos = tm->grid[i].tiles[l].pos;
      pos.x = pos.x - xOffset;
      pos.y = pos.y - yOffset;
      Texture_render(tm->spriteSheet, renderer,
                     &((SDL_Rect){tm->grid[i].tiles[l].spriteXId * tileSize,
                                  tm->grid[i].tiles[l].spriteYId * tileSize,
                                  tileSize, tileSize}),
                     &pos, tm->grid[i].tiles[l].rotation, NULL,
                     tm->grid[i].tiles[l].flip);
    }
    Uint8 colliderAmount = 0;
    SDL_FRect **surroundingColliders = NULL;
    for (int l = 0; l < tm->grid[i].numGridEntities; l++) {
      surroundingColliders = Tilemap_getCollidersAroundEntity(
          tm, &tm->grid[i].gridEntities[l], &colliderAmount);
      Entity_move(&tm->grid[i].gridEntities[l], surroundingColliders,
                  colliderAmount);
      Entity_render(&tm->grid[i].gridEntities[l], renderer,
                    tm->grid[i].gridEntities[l].clip, -1, NULL, SDL_FLIP_NONE,
                    camera, 1);
    }
    if (tm->displayGrid) {
      SDL_FPoint gridLines[4];
      int x1 = ((i % tm->gridWidth) * tileSize * tm->scale *
                sqrt(tm->tilesPerGrid)) -
               xOffset;
      int x2 = x1 + (sqrt(tm->tilesPerGrid) * tileSize * tm->scale);

      int y1 = ((i / tm->gridWidth) * tileSize * tm->scale *
                sqrt(tm->tilesPerGrid)) -
               yOffset; // Need data loss from int
      int y2 = y1 + (sqrt(tm->tilesPerGrid) * tileSize * tm->scale);

      gridLines[0].x = x1;
      gridLines[0].y = y1;
      gridLines[1].x = x1;
      gridLines[1].y = y2;
      gridLines[2].x = x1;
      gridLines[2].y = y1;
      gridLines[3].x = x2;
      gridLines[3].y = y1;

      if (!tm->grid[i].isEntityCheckingGrid)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0xFF); // setting to red
      else {
        SDL_SetRenderDrawColor(renderer, 0, 255, 170,
                               0xFF); // setting to green-blue
        tm->grid[i].isEntityCheckingGrid = false;
      }

      SDL_RenderDrawLinesF(renderer, gridLines, 4);
    }
  }
}

SDL_FRect *Tilemap_getColliders(Tilemap *tm) {
  SDL_FRect *colliders = malloc(sizeof(SDL_FRect) * tm->tileAmount);
  if (colliders == NULL) {
    perror("Could not allocate memory for colliders in tm");
    return NULL;
  }
  for (int i = 0; i < tm->gridAmount; i++) {
    for (int l = 0; l < tm->grid[i].tilesInGrid; l++) {
      colliders[i] = tm->grid[i].tiles[l].pos;
    }
  }
  return colliders;
}

SDL_FRect **Tilemap_getCollidersAroundEntity(Tilemap *tm, Entity *entity,
                                             Uint8 *colliderAmount) {
  Uint8 tileSize = TILE_SIZE;
  Uint16 tileScaledSize = tm->scale * tileSize;
  Uint8 squaredTilesPerGrid = (Uint16)sqrt(tm->tilesPerGrid);
  Uint32 gridPixelLength = (squaredTilesPerGrid * tileScaledSize);

  if (entity->collider.w > gridPixelLength ||
      entity->collider.h > gridPixelLength) {
    printf("Entity is larger than grid!!\n");
  }

  Uint8 entityTileWidth = divideUp(entity->collider.w, tileScaledSize);
  Uint8 entityTileHeight = divideUp(entity->collider.h, tileScaledSize);
  Uint8 maxColliders =
      ((entityTileHeight + 1) * (entityTileWidth + 1)) +
      ((entityTileHeight + 1) * 2) + ((entityTileWidth + 1) * 2) +
      4; //+1's are to account for entity being inside different tiles at once
  // printf("Max colliders %d\n", maxColliders);
  /// DOUBLE CHECK ALL OF THIS SHIT ALSO CHECK IF THE EFUNC IS WORKING PROPERLY
  /// | MAYBE AN ISSUE WITH THE UNSIGNED INTS??
  /// I THINK THERES AN ISSUE WITH HOW I'M CHECKING FOR TILES AROUND THE ENITITY
  SDL_FRect **rects =
      malloc(sizeof(SDL_FRect *) *
             maxColliders); // Need to adjust this based on tilesize and
                            // scale compared to entity
  if (!rects) {
    printf("getCollidersAroundEntity memory not initialized propery.\n");
    return NULL;
  }
  // printf("Tilesize = %d\n", tileSize);

  SDL_Point EntityTilePos = (SDL_Point){
      (int)(((entity->xPos + (entity->collider.w / 2)) / (tileSize) /
             tm->scale)),
      (int)((((entity->yPos + (entity->collider.h / 2)) / tileSize) /
             tm->scale))};
  SDL_Point EntityLeftSideTilePos = (SDL_Point){
      (int)(((entity->xPos + 1) / (tileSize) / tm->scale)),
      (int)((((entity->yPos + (entity->collider.h / 2)) / tileSize) /
             tm->scale))};
  SDL_Point EntityRightSideTilePos = (SDL_Point){
      (int)(((entity->xPos + (entity->collider.w) - 1) / (tileSize) /
             tm->scale)),
      (int)((((entity->yPos + (entity->collider.h / 2)) / tileSize) /
             tm->scale))};
  SDL_Point EntityTopSideTilePos =
      (SDL_Point){(int)(((entity->xPos + (entity->collider.w / 2)) /
                         (tileSize) / tm->scale)),
                  (int)((((entity->yPos + 1) / tileSize) / tm->scale))};
  SDL_Point EntityBottomSideTilePos = (SDL_Point){
      (int)(((entity->xPos + (entity->collider.w / 2)) / (tileSize) /
             tm->scale)),
      (int)((((entity->yPos - 1 + (entity->collider.h)) / tileSize) /
             tm->scale))};
  // printf("bottomX=%d, bottomY=%d\n", EntityBottomSideTilePos.x,
  //        EntityBottomSideTilePos.y);

  SDL_Point EntityGridPos =
      (SDL_Point){(int)((EntityTilePos.x) / squaredTilesPerGrid),
                  (int)((EntityTilePos.y) / squaredTilesPerGrid)};

  int entityGridIndex = ((EntityGridPos.y * tm->gridWidth) + EntityGridPos.x);
  Uint8 rectCount = 0;
  // printf("PLAYER[%d]: X = %d, Y = %d\n", entityGridIndex, EntityGridPos.x,
  //        EntityGridPos.y);

  // FINDING POSITION IN GRID
  Uint8 entityPosInGrid = 0; // 1 = TL, 2 = TR, 3 = BL, 4 = BR
  if (EntityTilePos.x % squaredTilesPerGrid < (squaredTilesPerGrid / 2))
    if (EntityTilePos.y % squaredTilesPerGrid < (squaredTilesPerGrid / 2))
      entityPosInGrid = 1;
    else
      entityPosInGrid = 3;
  else if (EntityTilePos.y % squaredTilesPerGrid < (squaredTilesPerGrid / 2))
    entityPosInGrid = 2;
  else
    entityPosInGrid = 4;
  // printf("Grid section -> %d\n", entityPosInGrid);

  // GRID CHECKING
  bool isGridLeft = false, isGridRight = false, isGridUp = false,
       isGridDown = false;
  if (((entityGridIndex % tm->gridWidth) != 0)) // left
    isGridLeft = true;
  if ((entityGridIndex % (tm->gridWidth) != (tm->gridWidth - 1))) // right
    isGridRight = true;
  if (((entityGridIndex - (tm->gridWidth) >= 0))) // up
    isGridUp = true;
  if (((entityGridIndex + (tm->gridWidth) <= tm->gridAmount))) // down
    isGridDown = true;

  for (int l = 0; l < 4; l++) {
    int ind = -1;
    if (l == 0)
      ind = entityGridIndex;
    else if (l == 1) { // left or right
      switch (entityPosInGrid) {
      case 1:
      case 3:
        if (isGridLeft)
          ind = entityGridIndex - 1;
        break;
      case 2:
      case 4:
        if (isGridRight)
          ind = entityGridIndex + 1;
        break;
      }
    } else if (l == 2) {
      switch (entityPosInGrid) {
      case 1:
      case 2:
        if (isGridUp)
          ind = entityGridIndex - tm->gridWidth;
        break;

      case 3:
      case 4:
        if (isGridDown)
          ind = entityGridIndex + tm->gridWidth;
        break;
      }

    } else if (l == 3) {
      switch (entityPosInGrid) {
      case 1:
        if (isGridUp && isGridLeft)
          ind = (entityGridIndex - tm->gridWidth) - 1;
        break;
      case 2:
        if (isGridUp && isGridRight)
          ind = (entityGridIndex - tm->gridWidth) + 1;
        break;
      case 3:
        if (isGridDown && isGridLeft)
          ind = (entityGridIndex + tm->gridWidth) - 1;
        break;
      case 4:
        if (isGridDown && isGridRight)
          ind = (entityGridIndex + tm->gridWidth) + 1;
        break;
      }
    }
    // if (gridPixelLength > entity->width &&
    //     gridPixelLength> entity->height) {
    // }

    if (ind != -1 && ind < tm->gridAmount) {
      for (int i = 0; i < tm->grid[ind].tilesInGrid; i++) {
        //  printf("checking grid[%d] - playergrid[%d] - playerx: %d, playery:
        //  %d || tilex: % d, tiley
        // : % d\n ", ind, entityGridIndex,
        //   (int)((((entity->xPos + (entity->width / 2)) / (tileSize) /
        //           tm->scale))),
        // (int)(((((entity->yPos + (entity->height / 2)) / tileSize) /
        //         tm->scale))),
        // tm->tiles[ind][i].xGridPos, tm->tiles[ind][i].yGridPos);

        if (!(tm->grid[ind].tiles[i].xGridPos < EntityLeftSideTilePos.x - 1 ||
              tm->grid[ind].tiles[i].xGridPos > EntityRightSideTilePos.x + 1 ||
              tm->grid[ind].tiles[i].yGridPos <
                  (tm->mapHeight - EntityBottomSideTilePos.y) - 1 ||
              tm->grid[ind].tiles[i].yGridPos >
                  (tm->mapHeight - EntityTopSideTilePos.y) + 1)) {
          /*printf("maxColliders=%d, rectCount=%d, grid[%d].tiles[%d]\n",*/
          /*       maxColliders, rectCount, ind, i);*/
          // printf("rectcount=%d\n", rectCount);
          rects[rectCount++] = &tm->grid[ind].tiles[i].pos;
          // tm->tiles[ind][i].spriteYId=1; For collision testing
        }
      }
      tm->grid[ind].isEntityCheckingGrid = true;
    }
  }
  if (rectCount == 0) {
    free(rects);
    return NULL;
  } else if (rectCount < maxColliders) {
    SDL_FRect **newRect = realloc(
        rects, sizeof(SDL_FRect *) *
                   rectCount); // WARNING, does not free rects if rectCount == 0
    if (newRect == NULL) {
      free(rects);
      rects = NULL;
      printf("getCollidersAroundEntity memory not initialized propery.\n");
      return NULL;
    }
    rects = newRect;
  }
  *colliderAmount = rectCount;
  // printf("colliderAmount= %d\nnewRect pointer: %p\n", rectCount, rects);
  return rects;
}

bool Tilemap_isTileParseable(Tilemap *tm, char tileToParse, int *tileId) {
  *tileId = 0;
  for (int i = 0; i < tm->numTileTypesForParse; i++) {
    if (tm->tileTypesForParse[i] == tileToParse)
      return true;
    *tileId = *tileId + 1;
  }
  return false;
}

bool Tilemap_isEntityTileParseable(Tilemap *tm, char tileToParse,
                                   int *entityId) {
  *entityId = 0;
  for (int i = 0; i < tm->numEntityTileTypesForParse; i++) {
    if (tm->entityTileTypesForParse[i] == tileToParse)
      return true;
    *entityId = *entityId + 1;
  }
  return false;
}

float Tilemap_getMapWidthPixels(Tilemap *tm) {
  return (tm->mapWidth - 1) * tm->scale * TILE_SIZE;
}
float Tilemap_getMapHeightPixels(Tilemap *tm) {
  return (tm->mapHeight) * tm->scale * TILE_SIZE;
}
