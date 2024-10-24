//
//  Tilemap.c
//  CSDL
//
//  Created by Erik Morris on 7/23/24.
//

#include "Tilemap.h"

void Tile_init(Tile* tile, int xGridPos, int yGridPos, int spriteXId, int spriteYId, int tileSize, int mapHeight, float scale, double rotation, SDL_RendererFlip flip){
    tile->spriteXId=spriteXId;
    tile->spriteYId=spriteYId;
    tile->xGridPos=xGridPos;
    tile->yGridPos=yGridPos;
    tile->flip=flip;
    tile->rotation=rotation;
    tile->pos=(SDL_FRect){(xGridPos*tileSize*scale), -((yGridPos-mapHeight)*tileSize*scale), tileSize*scale, tileSize*scale};
}
void Tile_free(Tile* tile){
    
}
void Tilemap_init(Tilemap* tm, Texture* text, float scale, int tilesPerGrid, char* fileName, char* tileTypes, int numTileTypes){
    tm->tileTypesForParse = tileTypes;
    tm->numTileTypesForParse = numTileTypes;
    tm->spriteSheet=text;
    tm->scale = scale;
    tm->tilesPerGrid = tilesPerGrid; //MUST BE A PERFECT SQUARE VALUE ie 9->(3x3) 16->(4x4) etc
    Tilemap_populate(tm, fileName);
//    tm->colliders = Tilemap_getColliders(tm);
}
void Tilemap_free(Tilemap* tm){
    for (int i = 0; i < tm->gridAmount; i++)
    {
        if (tm->tiles[i] != NULL){
            free(tm->tiles[i]);
            tm->tiles[i]=NULL;
        }
    }
    if (tm->tiles != NULL){
        free(tm->tiles);
        tm->tiles=NULL;
    }
    if (tm->spriteSheet!=NULL){
        Texture_free(tm->spriteSheet);
        tm->spriteSheet=NULL;
    }
    if (tm->colliders!=NULL){
        free(tm->colliders);
        tm->colliders=NULL;
    }
    if (tm->tileTypesForParse!=NULL)
    {
        tm->tileTypesForParse = NULL;
        tm->numTileTypesForParse = 0;
    }
}
void Tilemap_populate(Tilemap* tm, char* fileName){
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
       char *content = (char*)malloc((fileSize + 1) * sizeof(char)); // +1 for null terminator
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
           if (line[len - 1] == '\n') line[len - 1] = '\0'; // Remove newline character
           strcpy(content + totalBytesRead, line);
           rows++;
           totalBytesRead += len;
       }
       content[totalBytesRead] = '\0'; // Null-terminate the final string
        
    tm->mapHeight=rows;
    tm->mapWidth=(int)(totalBytesRead/rows);
    printf("mapHeight: %d\nmapWidth: %d\nTotal bytes: %zu\n", tm->mapHeight,
           tm->mapWidth, totalBytesRead);

       fclose(file);
    
    int tileSize = TILE_SIZE;
    tm->gridWidth = ((int)((tm->mapWidth%(int)sqrt(tm->tilesPerGrid)) == 0)) ? (int)((tm->mapWidth/sqrt(tm->tilesPerGrid))) : (int)((tm->mapWidth/sqrt(tm->tilesPerGrid))+1);
    tm->gridHeight = ((int)((tm->mapHeight%(int)sqrt(tm->tilesPerGrid)) == 0)) ? ((int)((tm->mapHeight/sqrt(tm->tilesPerGrid)))) : ((int)((tm->mapHeight/sqrt(tm->tilesPerGrid))+1));
    tm->gridAmount =  tm->gridWidth*tm->gridHeight;
    printf("TilesPerGrid = %d\nGrid Amount = %d\nGrid width = %d, Grid height = %d\n", tm->tilesPerGrid, tm->gridAmount, tm->gridWidth, tm->gridHeight);
    Tile** tiles = NULL;
    tiles = malloc(sizeof(Tile*)*tm->gridAmount);
    if (tiles == NULL){printf("tiles could not be allocatied\n");}
    Uint8* tilesInGrid = malloc(sizeof(Uint8)*tm->gridAmount);
    if (tilesInGrid == NULL)
        printf("tilesInGrid not malloced\n");
    

    for (int i = 0; i < tm->gridAmount; i++){
        tiles[i] = malloc(sizeof(Tile)*tm->tilesPerGrid);
        if (tiles[i]==NULL)
        {
            printf("tiles[%d] is NULL!\n", i);
        }
        tilesInGrid[i] = 0;
    }
    int tileAmount = 0;
    
    int spriteYId = 0; //Gets back tileId from the function call below
    int spriteXId = 0;
    for (int i = 0; i < totalBytesRead; i++)
    {
        char tile = content[i];
        
        if (Tilemap_isTileParseable(tm, tile, &spriteYId))//If a tile object
        {
            //----------------------------------------------------------------------------
            SDL_RendererFlip fl = SDL_FLIP_NONE;
            double angle = 0.0;
            bool tileLeft = false, tileUp = false, tileRight = false, tileDown = false;
            if (i!=0)//check for surrounding tiles
                if (content[i-1]==tile)
                    tileLeft = true;
            if (i-(tm->mapWidth) >= 0)
                if (content[i-tm->mapWidth] == tile)
                    tileUp=true;
            if (i!=totalBytesRead)
                if (content[i+1]==tile)
                    tileRight=true;
            if (i+tm->mapWidth<=totalBytesRead)
                if (content[i+tm->mapWidth]==tile)
                    tileDown=true;
            if (!(tileLeft || tileRight || tileUp || tileDown)) {spriteXId = 0;}
            else if (tileLeft && !(tileRight || tileUp || tileDown)){spriteXId = 4;}
            else if (tileRight && !(tileLeft || tileUp || tileDown)){spriteXId = 4; fl = SDL_FLIP_HORIZONTAL;}
            else if (tileUp && !(tileLeft || tileRight || tileDown)){spriteXId = 4; angle=90.0;}
            else if (tileDown && !(tileLeft || tileRight || tileUp)) {
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
                //fl = SDL_FLIP_HORIZONTAL;
            } else if (tileRight && tileDown && !(tileLeft || tileUp)) {
                spriteXId = 2;
                fl = SDL_FLIP_HORIZONTAL;
                //fl = SDL_FLIP_HORIZONTAL;
            } else if (tileLeft && tileRight && tileUp && !tileDown) {
                spriteXId = 3;
                angle = 90.0;
            } else if (tileLeft && tileRight && tileDown && !tileUp) {
                spriteXId = 1;
                //angle = 270.0;
            } else if (tileUp && tileDown && tileLeft && !tileRight) {
                spriteXId = 3;
                //angle = 90.0;
            } else if (tileUp && tileDown && tileRight && !tileLeft) {
                spriteXId = 3;
                fl = SDL_FLIP_HORIZONTAL;
            } else if (tileLeft && tileRight && tileUp && tileDown) {
                spriteXId = 0; // Assuming a different sprite for all tiles present
            }
            //---------------------------------------------------------------------------
            
            int index = ((int)((i/tm->mapWidth)/sqrt(tm->tilesPerGrid))*(int)((tm->mapWidth/sqrt(tm->tilesPerGrid))+1)) + (int)((i%tm->mapWidth)/sqrt(tm->tilesPerGrid));
            Tile_init(&tiles[index][tilesInGrid[index]++], i%tm->mapWidth, tm->mapHeight-(i/tm->mapWidth), spriteXId, spriteYId, tileSize, tm->mapHeight, tm->scale, angle, fl);
            //printf("tile:%d->x:%d, y:%d\n", i, i%tm->mapWidth, tm->mapHeight-(i/tm->mapWidth));
            //printf("Grid index-> [%d], [%d]\n", index, tilesInGrid[index]-1);
            tileAmount++;

        }
    }

    for (int i =0; i < tm->gridAmount; i++)
    {
        printf("grid[%d] -> %d tiles\n", i, tilesInGrid[i]);
        
        Tile* newTile = realloc(tiles[i], sizeof(Tile)*tilesInGrid[i]);
        if (newTile == NULL && tilesInGrid[i]!=0)
        {
            printf("Could not allocate memory to tile[%d] in Tilemap_populate p.2\n", i);
            free(tiles);
            free(content);
            return;
        }
        tiles[i]=newTile;
    }
    tm->tilesInGrid = tilesInGrid;
    tm->tiles=tiles;
    tm->tileAmount=tileAmount;
    free(content);
}

void Tilemap_render(Tilemap* tm, SDL_Renderer* renderer, float xOffset, float yOffset, float camOffX, float camOffY){
    int tileSize = TILE_SIZE;
    int screenWidth = SCREEN_WIDTH;
    int screenHeight = SCREEN_HEIGHT;
    SDL_Point CameraMinTilePos = {(int)((((xOffset-camOffX)/tileSize)/tm->scale)), (int)((((yOffset-camOffY)/tileSize)/tm->scale))};
    SDL_Point CameraMinGridPos = {(int)((CameraMinTilePos.x)/sqrt(tm->tilesPerGrid)), (int)((CameraMinTilePos.y)/sqrt(tm->tilesPerGrid))};
    SDL_Point CameraMaxTilePos = {(int)(((((xOffset-camOffX) + screenWidth)/tileSize)/tm->scale)), (int)(((((yOffset-camOffY) + screenHeight)/tileSize)/tm->scale))};
    SDL_Point CameraMaxGridPos = {(int)((CameraMaxTilePos.x)/sqrt(tm->tilesPerGrid)), (int)((CameraMaxTilePos.y)/sqrt(tm->tilesPerGrid))};
    if (CameraMinGridPos.x < 0) CameraMinGridPos.x = 0;
    if (CameraMinGridPos.y < 0) CameraMinGridPos.y = 0;
    if (CameraMaxGridPos.x >= tm->gridWidth) CameraMaxGridPos.x = tm->gridWidth-1;
    if(CameraMaxGridPos.y >= tm->gridHeight) CameraMaxGridPos.y = tm->gridHeight-1;
    //printf("CameraTilePos = (%d, %d) -- CameraGridPos = (%d, %d)\nCameraMaxTilePos = (%d, %d) -- CameraMaxGridPos = (%d, %d)\n", CameraMinTilePos.x, CameraMinTilePos.y, CameraMinGridPos.x, CameraMinGridPos.y, CameraMaxTilePos.x, CameraMaxTilePos.y, CameraMaxGridPos.x, CameraMaxGridPos.y);

    
    
    for (int i = (CameraMinGridPos.y*tm->gridWidth)+CameraMinGridPos.x; i<=(CameraMaxGridPos.y*tm->gridWidth)+CameraMaxGridPos.x; i++){
        if(((i%(tm->gridWidth))>CameraMaxGridPos.x) || i%tm->gridWidth<CameraMinGridPos.x) i+=(tm->gridWidth-((CameraMaxGridPos.x-CameraMinGridPos.x)+1));
        for (int l = 0; l<tm->tilesInGrid[i]; l++){
            SDL_FRect pos = tm->tiles[i][l].pos;
            pos.x = pos.x-xOffset; pos.y=pos.y-yOffset;
            Texture_render(tm->spriteSheet, renderer, &((SDL_Rect){tm->tiles[i][l].spriteXId*tileSize, tm->tiles[i][l].spriteYId*tileSize, tileSize, tileSize}), &pos, tm->tiles[i][l].rotation, NULL, tm->tiles[i][l].flip);
        }
    }
    
//    for (int i = 0; i<tm->gridAmount; i++){
//        for (int l = 0; l<tm->tilesInGrid[i]; l++){
//            SDL_FRect pos = tm->tiles[i][l].pos;
//            pos.x = pos.x-xOffset; pos.y=pos.y-yOffset;
//            Texture_render(tm->spriteSheet, renderer, &((SDL_Rect){tm->tiles[i][l].spriteXId*tileSize, tm->tiles[i][l].spriteYId*tileSize, tileSize, tileSize}), &pos, tm->tiles[i][l].rotation, NULL, tm->tiles[i][l].flip);
//        }
//    }
}

SDL_FRect* Tilemap_getColliders(Tilemap* tm){
    SDL_FRect* colliders = malloc(sizeof(SDL_FRect)*tm->tileAmount);
    if (colliders==NULL){
        perror("Could not allocate memory for colliders in tm");
        return NULL;
    }
    for (int i = 0; i<tm->gridAmount; i++){
        for (int l = 0; l<tm->tilesInGrid[i]; l++){
            colliders[i]=tm->tiles[i][l].pos;
        }
    }
    return colliders;
}

SDL_FRect* Tilemap_getCollidersAroundEntity(Tilemap* tm, Entity* entity, Uint8* colliderAmount){
    SDL_FRect* rects = malloc(sizeof(SDL_FRect)*9);
    if (!rects){printf("getCollidersAroundEntity memory not initialized propery.\n"); return NULL;}
    Uint8 tileSize = TILE_SIZE;
    SDL_Point EntityTilePos = {(int)(((entity->xPos+(entity->width/2))/(tileSize)/tm->scale)), (int)((((entity->yPos+(entity->height/2))/tileSize)/tm->scale))};
    SDL_Point EntityGridPos = {(int)((EntityTilePos.x)/sqrt(tm->tilesPerGrid)), (int)((EntityTilePos.y)/sqrt(tm->tilesPerGrid))};
    int entityGridIndex = ((EntityGridPos.y* tm->gridWidth) + EntityGridPos.x);
    Uint8 rectCount = 0;
    //printf("PLAYER[%d]: X = %d, Y = %d\n",entityGridIndex, EntityGridPos.x, EntityGridPos.y);
    
    //FINDING POSITION IN GRID
    Uint8 entityPosInGrid =  0;//1 = TL, 2 = TR, 3 = BL, 4 = BR
    if(EntityTilePos.x%(int)sqrt(tm->tilesPerGrid) < (sqrt(tm->tilesPerGrid)/2))
        if (EntityTilePos.y%(int)sqrt(tm->tilesPerGrid) < (sqrt(tm->tilesPerGrid)/2))
            entityPosInGrid = 1;
        else
            entityPosInGrid = 3;
    else
        if (EntityTilePos.y%(int)sqrt(tm->tilesPerGrid) < (sqrt(tm->tilesPerGrid)/2))
            entityPosInGrid = 2;
        else
            entityPosInGrid = 4;
    //printf("Grid section -> %d\n", entityPosInGrid);
    

    //GRID CHECKING
    bool isGridLeft = false, isGridRight = false, isGridUp = false, isGridDown = false;
    if (((entityGridIndex%tm->gridWidth) != 0)) //left
        isGridLeft=true;
    if ((entityGridIndex%(int)(tm->gridWidth) != (tm->gridWidth-1))) //right
        isGridRight=true;
    if (((entityGridIndex-(tm->gridWidth) >= 0))) //up
        isGridUp=true;
    if (((entityGridIndex+(tm->gridWidth) <= tm->gridAmount))) //down
        isGridDown=true;
    
    for (int l = 0; l<4; l++){
        int ind = -1;
        if (l==0) ind = entityGridIndex;
        else if (l==1){ //left or right
            switch (entityPosInGrid) {
                case 1:
                case 3:
                    if(isGridLeft)
                        ind = entityGridIndex-1;
                    break;
                case 2:
                case 4:
                    if(isGridRight)
                        ind = entityGridIndex+1;
                    break;
            }
        }
        else if (l==2){
            switch (entityPosInGrid) {
                case 1:
                case 2:
                    if(isGridUp)
                        ind = entityGridIndex-tm->gridWidth;
                    break;
                    
                case 3:
                case 4:
                    if(isGridDown)
                        ind = entityGridIndex+tm->gridWidth;
                    break;
            }
            
        }
        else if (l==3){
            switch (entityPosInGrid) {
                case 1:
                    if (isGridUp && isGridLeft)
                        ind = (entityGridIndex-tm->gridWidth)-1;
                    break;
                case 2:
                    if (isGridUp && isGridRight)
                        ind = (entityGridIndex-tm->gridWidth)+1;
                    break;
                case 3:
                    if (isGridDown && isGridLeft)
                        ind = (entityGridIndex+tm->gridWidth)-1;
                    break;
                case 4:
                    if(isGridDown && isGridRight)
                        ind = (entityGridIndex+tm->gridWidth)+1;
                    break;
            }
        }
        
        
        if (ind!=-1 && ind < tm->gridAmount)
            for (int i = 0; i<tm->tilesInGrid[ind]; i++){
                //printf("checking grid[%d] - playergrid[%d] - playerx: %d, playery: %d || tilex: %d, tiley: %d\n", ind, entityGridIndex, (int)((((entity->xPos+(entity->width/2))/(tileSize)/tm->scale))), (int)(((((entity->yPos+(entity->height/2))/tileSize)/tm->scale))), tm->tiles[ind][i].xGridPos, tm->tiles[ind][i].yGridPos);
            
                if (!(tm->tiles[ind][i].xGridPos < EntityTilePos.x-1 || tm->tiles[ind][i].xGridPos > EntityTilePos.x+1 || tm->tiles[ind][i].yGridPos < (tm->mapHeight-EntityTilePos.y)-1 || tm->tiles[ind][i].yGridPos > (tm->mapHeight-EntityTilePos.y)+1))
                {
                    rects[rectCount++] = tm->tiles[ind][i].pos;
                    //tm->tiles[ind][i].spriteYId=1; For collision testing
                }
            }
        
    }
    
    if (rectCount<9){
        SDL_FRect* newRect = realloc(rects, sizeof(SDL_FRect)*rectCount);
        if (newRect == NULL) {
            free(rects);
            printf("getCollidersAroundEntity memory not initialized propery.\n");
            return NULL;
        }
        rects = newRect;
    }
    *colliderAmount=rectCount;
    //printf("colliderAmount= %d\nnewRect pointer: %p\n", rectCount, rects);
    return rects;
}

bool Tilemap_isTileParseable(Tilemap* tm, char tileToParse, int* tileId){
    *tileId = 0;
    for (int i = 0; i<tm->numTileTypesForParse; i++){
        if (tm->tileTypesForParse[i] == tileToParse)
            return true;
        *tileId=*tileId+1;
    }
    return false;
}

