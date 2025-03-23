//
//  main.c
//  CSDL
//
//  Created by Erik Morris on 7/16/24.
//

#include "Engine/BackgroundEntity.h"
#include "Engine/Button.h"
#include "Engine/Camera.h"
#include "Engine/Entity.h"
#include "Engine/Texture.h"
#include "Engine/Tilemap.h"
#include "Engine/Timer.h"
#include "Engine/constants.h"
#include <SDL2/SDL.h>
#ifdef __linux__
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#elif defined __APPLE__
#include <SDL2_image/SDL_image.h>
#include <SDL2_mixer/SDL_mixer.h>
#include <SDL2_ttf/SDL_ttf.h>
#endif
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif /* ifdef __EMSCRIPTEN__ */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct context {
  /* Important stuff */
  int ticksPerFrame;
  SDL_Window *window;     // freed
  SDL_Renderer *renderer; // freed
  int tilemap_tilesPerGrid;
  int width, height;
  SDL_Rect screensize;

  /* Textures/Fonts */
  TTF_Font *gFont;     // freed
  Texture fontTexture; // freed

  Texture fpsTexture; // freed

  /* Entities */
  Entity player;         // freed
  Entity testObject;     // freed
  Entity mapEntities[1]; // freed

  Entity fish;             // freed
  BackgroundEntity bgFish; // freed

  /* Other */
  Button butt;           // freed
  SDL_Joystick *gamePad; // freed
  Timer fps;             // no need to free
  Timer capTimer;
  int frameCount;

  Camera camera; // freed

  /* TILE MAP */
  Texture tilemapSpriteSheet; // Freed by tilemap
  Tilemap tilemap;            // freed

  /* Music/Sounds */
  Mix_Chunk *soundEffect; // freed
  Mix_Music *gameMusic;   // freed

  // Other ctx stuff
  SDL_FRect fpsLoc;
  SDL_Color fpsCol;

  float cameraOffsetOffsetx, cameraOffsetOffsety;
  float camMoveSpeed;

  //    Uint8 tileSize = TILE_SIZE;

  Uint8 currentPlayerSprite;

  // player stuff
  double playerRotation;
  bool onNub;
  bool lastMovedRight;

  bool isButtPressed;
  bool quit;
} context;

bool loadMedia(context *ctx) {

  bool success = true;

  ctx->screensize.x = 0;
  ctx->screensize.y = 0;
  ctx->screensize.w = ctx->width;
  ctx->screensize.h = ctx->height;
  // initialize everything!
  // Texture_init(&test);
  Texture_init(&ctx->fontTexture);
  Texture_init(&ctx->fpsTexture);
  Texture_init(&ctx->tilemapSpriteSheet);
  Timer_init(&ctx->fps);
  Timer_init(&ctx->capTimer);
  Button_init(&ctx->butt, 10, 10, 25, 25);
  // Entity_init(&player, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 100, 100, 10);
  // //Normal entity init
  Entity_initPhysics(&ctx->player, (float)SCREEN_WIDTH / 2 - 50,
                     (float)SCREEN_HEIGHT / 2 - 50, 100, 100, 100, 100,
                     (SDL_FPoint){0, 0}, 0.8f, 5.0f, 0.3f, .95f, 20.0f, 3);
  Entity_initPhysics(&ctx->testObject, 300, 0, 100, 100, 100, 100,
                     (SDL_FPoint){0, 0}, .8f, 12.0f, 0.3f, .95f, 8.0f, 1);
  Entity_init(&ctx->fish, 300, 300, 100, 100, 100, 100, (SDL_FPoint){0, 0}, 0.0,
              2);

  Camera_init(&ctx->camera);

  if (!Texture_loadFromFile(&ctx->tilemapSpriteSheet, ctx->renderer,
                            "Art/TileSpritesheet.png")) {
    printf("Failed to load TileSpritesheet texture image!\n");
    success = false;
  }

  if (!Entity_setTexture(&ctx->player, ctx->renderer,
                         "Art/StarfishSpriteSheet.png")) {
    printf("Failed to load Starfish.png texture image!\n");
    success = false;
  }
  ctx->player.clip[0] = (SDL_Rect){2, 5, 25, 22};
  ctx->player.clip[1] = (SDL_Rect){34, 5, 25, 22};
  ctx->player.clip[2] = (SDL_Rect){66, 5, 25, 22};

  if (!Entity_setTexture(&ctx->testObject, ctx->renderer, "Art/RelicArt.png")) {
    printf("Failed to load RelicArt.png!\n");
    success = false;
  }

  if (!Entity_setTexture(&ctx->fish, ctx->renderer,
                         "Art/BackgroundFishSpriteSheet.png")) {
    printf("Failed to load background fish!\n");
    success = false;
  }
  ctx->testObject.clip[0] = (SDL_Rect){2, 5, 25, 22};
  ctx->mapEntities[0] = ctx->testObject;
  Tilemap_init(&ctx->tilemap, &ctx->tilemapSpriteSheet, true, 3.75f,
               ctx->tilemap_tilesPerGrid, "Art/map.txt", "SX", 2, "E", 1,
               ctx->mapEntities, 1);
  ctx->fish.clip[0] = (SDL_Rect){0, 0, 32, 32};
  ctx->fish.clip[1] = (SDL_Rect){32, 0, 32, 32};
  BackgroundEntity_init(
      &ctx->bgFish, &ctx->fish,
      10); // CURRENTLY NEED TO INIT AFTER LOADING ENTITY TEXTURE. NEED TO
           // MAKE ENTITY'S TEXTURE A POINTER DONT FORGET

  ctx->gFont = TTF_OpenFont("Fonts/tuffy_regular.ttf",
                            56); // Location and font size;
  if (ctx->gFont != NULL) {
    SDL_Color fontCol = {0, 255, 122, 255};
    if (!Texture_loadFromRenderedText(
            &ctx->fontTexture, ctx->renderer, ctx->gFont,
            "Starfish Adventures the Epilogue", fontCol)) {
      printf("Failed to load Font texture!\n");
      success = false;
    }
    fontCol.g = 0;
    fontCol.b = 0;
    fontCol.r = 0;
    if (!Button_loadTextures(&ctx->butt, ctx->renderer,
                             "Art/ButtonBackground.png", "Music", ctx->gFont,
                             fontCol)) {
      printf("Failed to load button texture!\n");
      success = false;
    }
  }
  ctx->soundEffect = Mix_LoadWAV("Sounds/low.wav");
  if (ctx->soundEffect == NULL) {
    printf("Could not set soundEffect sound! Error: %s\n", Mix_GetError());
  }
  //    gameMusic = Mix_LoadMUS("Sounds/game - music 1.wav");
  ctx->gameMusic = Mix_LoadMUS("Sounds/The Penguin God.wav");
  if (ctx->gameMusic == NULL) {
    printf("Could not set gameMusic! Error: %s\n", Mix_GetError());
  }

  printf("Done loading media\n");
  return success;
}

// START GAME LOOP
void startGameloop(context *ctx) {
  Timer_start(&ctx->fps);
  Texture_setColor(&ctx->player.spriteSheet, 10, 255, 10);
}

void gameLoop(void *arg) {
  context *ctx = SDL_static_cast(context *, arg);
  Timer_start(&ctx->capTimer);
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
#ifdef __EMSCRIPTEN__
    if (e.type == SDL_KEYDOWN) {
      if (e.key.keysym.sym == SDLK_END) {
        emscripten_cancel_main_loop();
      }
    }

#else
    if (e.type == SDL_QUIT) {
      ctx->quit = true;
    }
#endif
    if (e.type == SDL_KEYDOWN) {
      if (e.key.keysym.sym == SDLK_i) {
        ctx->cameraOffsetOffsety -= ctx->camMoveSpeed;
      }
      if (e.key.keysym.sym == SDLK_j) {
        ctx->cameraOffsetOffsetx -= ctx->camMoveSpeed;
      }
      if (e.key.keysym.sym == SDLK_k) {
        ctx->cameraOffsetOffsety += ctx->camMoveSpeed;
      }
      if (e.key.keysym.sym == SDLK_l) {
        ctx->cameraOffsetOffsetx += ctx->camMoveSpeed;
      }
      if (e.key.keysym.sym == SDLK_g) { // Tilemap grid testing
        ctx->tilemap.displayGrid = !ctx->tilemap.displayGrid;
      }
    }
    Button_handleEvent(&ctx->butt, &e, &ctx->isButtPressed);
    Entity_handleEvent(&ctx->player, &e);
    Camera_setCameraOffset(&ctx->camera, ctx->cameraOffsetOffsetx,
                           ctx->cameraOffsetOffsety);
  }

  if (Mix_PlayingMusic() == 0) {
    if (ctx->isButtPressed) {
      Mix_PlayMusic(ctx->gameMusic, -1);
    }
  } else {
    if (!ctx->isButtPressed && (Mix_PausedMusic() == 0)) {
      Mix_PauseMusic();
    } else if (ctx->isButtPressed && Mix_PausedMusic() == 1) {
      Mix_ResumeMusic();
    }
  }
  // background entity testing
  // if (ctx->frameCount % 1200 == 0) {
  //   for (int i = 0; i < 10; i++) {
  //     printf("BGE %d: clip pointer -> %p, clip size -> %d\n", i,
  //            ctx->bgFish.entity[i].clip, ctx->bgFish.entity[i].clipLength);
  //   }
  // }

  // ACTUAL GAME STUFF
  bool checkForLanding = false;
  if (ctx->player.onGround == 0)
    checkForLanding = true;
  Uint8 colliderAmount = 0;
  SDL_FRect **surroudningColliders = Tilemap_getCollidersAroundEntity(
      &ctx->tilemap, &ctx->player, &colliderAmount);
  Entity_move(&ctx->player, surroudningColliders, colliderAmount);

  // printf("colliderAmount for player -> %d\n", colliderAmount);

  if (ctx->player.onGround == 1 && checkForLanding) {
    Mix_PlayChannel(-1, ctx->soundEffect, 0);
    ctx->playerRotation += ctx->player.xVel / 2;
    // printf("Player xGridPos: %d, Player yGridPos: %d\n",
    // (int)((player.xPos/tileSize)/tilemap.scale),
    // (int)(tilemap.mapHeight-((player.yPos/tileSize)/tilemap.scale)));
  } else if (ctx->player.onGround == 1)
    ctx->playerRotation += ctx->player.xVel / 2;
  Uint8 TcolliderAmount = 0;
  SDL_FRect **testsurroudningColliders = Tilemap_getCollidersAroundEntity(
      &ctx->tilemap, &ctx->testObject, &TcolliderAmount);
  Entity_move(&ctx->testObject, testsurroudningColliders, TcolliderAmount);

  Camera_setPosition(&ctx->camera,
                     ctx->player.xPos + ctx->player.spriteRenderRect.w / 2,
                     ctx->player.yPos + ctx->player.spriteRenderRect.h / 2);
  // printf(
  //     "Camera xPos: %f, yPos: %f\n xOff: %f, yOff: %f \n xCam: %f, yCam: %f
  //     \n", ctx->camera.xPos, ctx->camera.yPos, ctx->camera.xObjOffset,
  //     ctx->camera.yObjOffset, ctx->camera.xCamOffset,
  //     ctx->camera.yCamOffset);
   Camera_setBounds(&ctx->camera,
   (SDL_FRect){0, 0, Tilemap_getMapWidthPixels(&ctx->tilemap),
   Tilemap_getMapHeightPixels(&ctx->tilemap)});

  // player rotation stuff
  if (ctx->player.xVel > 0) {
    ctx->lastMovedRight = true;
  } else if (ctx->player.xVel < 0) {
    ctx->lastMovedRight = false;
  }

  if ((((int)ctx->playerRotation % 360 < -195 &&
        (int)ctx->playerRotation % 360 > -270) ||
       ((((int)ctx->playerRotation % 360 > 90 &&
          (int)ctx->playerRotation % 360 < 165)))) &&
      !ctx->lastMovedRight && ctx->player.onGround) {
    if (!ctx->onNub) {
      ctx->currentPlayerSprite = 1;
      ctx->onNub = true;
    }
  } else if ((((int)ctx->playerRotation % 360 > 100 &&
               (int)ctx->playerRotation % 360 < 180) ||
              ((((int)ctx->playerRotation % 360 < -180 &&
                 (int)ctx->playerRotation % 360 > -260)))) &&
             ctx->lastMovedRight && ctx->player.onGround) {
    if (!ctx->onNub) {
      ctx->currentPlayerSprite = 2;
      ctx->onNub = true;
    }
  } else {
    ctx->currentPlayerSprite = 0;
    ctx->onNub = false;
  }

  Entity_setRotation(&ctx->player, ctx->playerRotation);
  // end player rotation stuff

  // FPS Stuff
  Uint32 avgFps = ctx->frameCount / (Timer_getTicks(&ctx->fps) / 1000.f);
  char fpsText[50] = "";
  snprintf(fpsText, sizeof(fpsText), "fps: %d",
           avgFps); // Feeds int into char buffer

  // START OF RENDERING
  SDL_SetRenderDrawColor(ctx->renderer, 100, 200, 255, 0xFF); // setting to blue
  SDL_RenderClear(ctx->renderer);

  // OBJECT RENDERING
  // Texture_render(&test, renderer, NULL, &imageLoc, 0.0, NULL, SDL_FLIP_NONE);
  BackgroundEntity_update(&ctx->bgFish, ctx->renderer, &ctx->camera,
                          ctx->frameCount, 50);

  Tilemap_render(&ctx->tilemap, ctx->renderer, &ctx->camera);

  Entity_render(&ctx->testObject, ctx->renderer, &ctx->player.clip[0], -1, NULL,
                SDL_FLIP_NONE, &ctx->camera, 1);

  Entity_render(&ctx->player, ctx->renderer, NULL, ctx->currentPlayerSprite,
                NULL, SDL_FLIP_NONE, &ctx->camera, 1);

  // UI RENDERING
  if (!Texture_loadFromRenderedText(&ctx->fpsTexture, ctx->renderer, ctx->gFont,
                                    fpsText, ctx->fpsCol)) {
    printf("Couldn't render fps text!!\n");
  } else {
    Texture_render(&ctx->fpsTexture, ctx->renderer, NULL, &ctx->fpsLoc, 0.0,
                   NULL, SDL_FLIP_NONE);
  }
  Button_render(&ctx->butt, ctx->renderer);

  // printf("%f\n",camera.yPos);   PRINT CAMERA POS

  // RENDER
  SDL_RenderPresent(ctx->renderer);
  ctx->frameCount++;

  // FPS CAP
  int frameTicks = Timer_getTicks(&ctx->capTimer);
  if (frameTicks < ctx->ticksPerFrame) {
    SDL_Delay(ctx->ticksPerFrame - frameTicks);
  }
}
/*---------------------------------- END GAME LOOP
 * ----------------------------------*/

bool init(context *ctx) {
  bool success = false;
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
  } else {

    ctx->window = SDL_CreateWindow("Erik test", SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED, ctx->width,
                                   ctx->height, SDL_WINDOW_SHOWN);
    if (ctx->window == NULL) {
      printf("Window was not loaded! SDL Error : %s\n", SDL_GetError());
    } else {
      ctx->renderer =
          SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_ACCELERATED);
      if (ctx->renderer == NULL) {
        printf("Renderer was not loaded! SDL Error : %s\n", SDL_GetError());
      } else {
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
          printf("SDL_image could not be initialized! SDL_image Error: %s\n",
                 IMG_GetError());
        }

        else {
          // Init audio stuff
          if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            printf("Mix could not init! SDL_Mix Error: %s\n", Mix_GetError());
          } else {
            // Initialize SDL_ttf
            if (TTF_Init() == -1) {
              printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n",
                     TTF_GetError());
            } else {
              // LOAD MEDIA
              if (!loadMedia(ctx)) {
                printf("Could not load texture image!\n");
              } else {
                // CHECK FOR JOYSTICKS AND SET TEXTURE FILTERING

                // Set texture filtering to linear
                if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
                  printf("Warning: Linear texture filtering not enabled!");
                }

                // Check for joysticks
                if (SDL_NumJoysticks() < 1) {
                  printf("Warning: No joysticks connected!\n");
                } else {
                  // Load joystick
                  ctx->gamePad = SDL_JoystickOpen(0);
                  if (ctx->gamePad == NULL) {
                    printf("Warning: Unable to open game controller! SDL "
                           "Error: %s\n",
                           SDL_GetError());
                  }
                }
                success = true;
              }
            }
          }
        }
      }
    }
  }
  return success;
}

void quit(context *ctx) {
  printf("Freeing\n");
  Texture_free(&ctx->fontTexture);
  // Texture_free(&test);
  Texture_free(&ctx->fpsTexture);
  Entity_free(&ctx->testObject, true);
  Entity_free(&ctx->player, true);
  Entity_free(&ctx->fish, true);
  Entity_free(&ctx->mapEntities[0], false);
  BackgroundEntity_free(&ctx->bgFish);
  Camera_free(&ctx->camera);
  TTF_CloseFont(ctx->gFont);
  ctx->gFont = NULL;
  ctx->gamePad = NULL;
  Mix_FreeChunk(ctx->soundEffect);
  Mix_FreeMusic(ctx->gameMusic);
  ctx->soundEffect = NULL;
  ctx->gameMusic = NULL;
  Button_free(&ctx->butt);
  Tilemap_free(&ctx->tilemap);
  SDL_DestroyRenderer(ctx->renderer);
  SDL_DestroyWindow(ctx->window);
  ctx->window = NULL;
  ctx->renderer = NULL;
  Mix_Quit();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  printf("Application finished\n");
}

int main(int argc, char *argv[]) {
#ifdef __LINUX__
  printf("Working linux\n");
#endif
#ifdef __APPLE__
  printf("Working apple\n");
#endif
  context ctx;
  // CONTEXT STUFF
    ctx.quit=false;
  /* Important stuff */
  ctx.ticksPerFrame = 1000.0f / TARGET_FPS;
  ctx.window = NULL;   // freed
  ctx.renderer = NULL; // freed
  ctx.tilemap_tilesPerGrid = TILES_PER_GRID;
  /* Textures/Fonts */
  ctx.gFont = NULL; // freed

  /* Music/Sounds */
  ctx.soundEffect = NULL; // freed
  ctx.gameMusic = NULL;   // freed

  ctx.width = SCREEN_WIDTH;
  ctx.height = SCREEN_HEIGHT;

  // fps stuff
  ctx.fpsLoc = (SDL_FRect){SCREEN_WIDTH - 50, 0, 50, 25};
  ctx.fpsCol = (SDL_Color){0, 0, 0, 255};

  // camera stuff
  ctx.camMoveSpeed = 100;
  ctx.cameraOffsetOffsetx = 0;
  ctx.cameraOffsetOffsety = 0;

  // player stuff
  ctx.playerRotation = 0.0;
  ctx.onNub = false;
  ctx.lastMovedRight = false;
  ctx.currentPlayerSprite = 0;

  // button
  ctx.isButtPressed = false;
  // END CONTEXT STUFF

  srand((unsigned int)time(NULL));
  if (!init(&ctx))
    printf("Init not loaded properly!");
  else {
    startGameloop(&ctx);
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(gameLoop, &ctx, -1, 1);
#else
    while (!ctx.quit)
      gameLoop(&ctx);
#endif /* ifdef __EMSCRIPTEN__ */
  }
  quit(&ctx);

  return 0;
}
