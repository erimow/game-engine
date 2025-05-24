//
//  main.c
//  CSDL
//
//  Created by Erik Morris on 7/16/24.
//

// #include "Engine/BackgroundEntity.h"
// #include "Engine/uiobject.h"
// #include "Engine/Camera.h"
// #include "Engine/Entity.h"
#include "Engine/Texture.h"
// #include "Engine/Tilemap.h"
#include "Engine/Timer.h"
#include "Engine/constants.h"
#include "Engine/context.h"
#include "Scripts/game.c"
//#include <SDL2/SDL.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string.h>
#ifdef __linux__
//#include <SDL2/SDL_mixer.h>
//#include <SDL2/SDL_pixels.h>
//#include <SDL2/SDL_rect.h>
//#include <SDL2/SDL_render.h>
#elif defined __APPLE__
// #include <SDL2_image/SDL_image.h>
// #include <SDL2_mixer/SDL_mixer.h>
// #include <SDL2_ttf/SDL_ttf.h>
#endif
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif /* ifdef __EMSCRIPTEN__ */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

bool loadMedia(context *ctx) {
  bool success = true;

  ctx->gFont = TTF_OpenFont("Fonts/tuffy_regular.ttf",
                            102); // Location and font size;
  if(ctx->gFont == NULL){
    printf("Could not load font!\n");
  }


  printf("Done loading media\n");
  return success;
}

// START GAME LOOP
void startGameloop(context *ctx) {
  Timer_start(&ctx->fps);
  Game_Start(ctx);
}

void gameLoop(void *arg) {
  context *ctx = SDL_static_cast(context *, arg);
  Timer_start(&ctx->capTimer);
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    Game_Events(ctx, &e); // Calls the events function in the game file
    #ifdef __EMSCRIPTEN__
    if (e.type == SDL_EVENT_KEY_DOWN) {
      if (e.key.scancode == SDLK_END) {
        emscripten_cancel_main_loop();
      }
    }

    #else
    if (e.type == SDL_EVENT_QUIT) {
      ctx->quit = true;
    }
    #endif
  }

  // if (Mix_PlayingMusic() == 0) {
  //   if (ctx->isButtPressed) {
  //     Mix_PlayMusic(ctx->gameMusic, -1);
  //   }
  // } else {
  //   if (!ctx->isButtPressed && (Mix_PausedMusic() == 0)) {
  //     Mix_PauseMusic();
  //   } else if (ctx->isButtPressed && Mix_PausedMusic() == 1) {
  //     Mix_ResumeMusic();
  //   }
  //}

  // ACTUAL GAME STUFF

  // FPS Stuff
  Uint32 avgFps = ctx->frameCount / (Timer_getTicks(&ctx->fps) / 1000.f);
  char fpsText[50] = "";
  snprintf(fpsText, sizeof(fpsText), "fps: %d",
           avgFps); // Feeds int into char buffer
  //
  //
  Game_Update(ctx); // calls update in the game.c file

  // START OF RENDERING
  SDL_SetRenderDrawColor(ctx->renderer, 0b00001111, 0b00001111, 0b00001111,
                         0xFF);
  SDL_RenderClear(ctx->renderer);

  // OBJECT RENDERING
  Game_Render(ctx);

  //RENDER FPS
  Texture_init_andLoadFromRenderedText(&ctx->fpsTexture, ctx->renderer, ctx->gFont, (SDL_FRect){SCREEN_WIDTH-75, 5, 70, 15}, fpsText, 9,(SDL_Color){200,200,200,255});
  Texture_render(&ctx->fpsTexture,ctx->renderer,NULL,&(SDL_FRect){SCREEN_WIDTH-75, 5, 70, 15}, 0.0, NULL, SDL_FLIP_NONE);

  // RENDER
  SDL_RenderPresent(ctx->renderer);
  ctx->frameCount++;

  // FPS CAP
  #ifdef TARGET_FPS
  int frameTicks = Timer_getTicks(&ctx->capTimer);
  if (frameTicks < ctx->ticksPerFrame) {
    SDL_Delay(ctx->ticksPerFrame - frameTicks);
  }
  #endif
}
/*---------------------------------- END GAME LOOP
 * ----------------------------------*/

bool init(context *ctx) {
  bool success = false;
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
  } else {
    if(!SDL_CreateWindowAndRenderer("Fudgie", ctx->width, ctx->height, 0, &ctx->window, &ctx->renderer)){
      SDL_Log("Could not make window and renderer: %s", SDL_GetError());
    }
  }  {
    // Initialize SDL_ttf
    if (!TTF_Init()) {
      printf("SDL_ttf could not initialize! SDL_ttf Error: \n"
      );
    } else {
      // LOAD MEDIA
      if (!loadMedia(ctx)) {
        printf("Could not load texture image!\n");
      } /*else {
      // CHECK FOR JOYSTICKS AND SET TEXTURE FILTERING

      // Set texture filtering to linear
      // if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
      //   printf("Warning: Linear texture filtering not enabled!");
      // }

      // Check for joysticks
      // if (SDL_NumJoysticks() < 1) {
      //   printf("Warning: No joysticks connected!\n");
      // } else {
      //   // Load joystick
      //   ctx->gamePad = SDL_JoystickOpen(0);
      //   if (ctx->gamePad == NULL) {
      //     printf("Warning: Unable to open game controller! SDL "
      //            "Error: %s\n",
      //            SDL_GetError());
      //   }
    }*/
    success = true;
    }
  }

  return success;
}

void quit(context *ctx) {
  printf("Freeing\n");
  Game_Stop(ctx);
  // Texture_free(&ctx->fontTexture);
  // Texture_free(&test);
  Texture_free(&ctx->fpsTexture);
  TTF_CloseFont(ctx->gFont);
  ctx->gFont = NULL;
  // ctx->gamePad = NULL;
  // Mix_FreeChunk(ctx->soundEffect);
  // Mix_FreeMusic(ctx->gameMusic);
  // ctx->soundEffect = NULL;
  // ctx->gameMusic = NULL;
  // Button_free(&ctx->butt);
  SDL_DestroyRenderer(ctx->renderer);
  SDL_DestroyWindow(ctx->window);
  ctx->window = NULL;
  ctx->renderer = NULL;
  // Mix_Quit();
  TTF_Quit();
  // IMG_Quit();
  SDL_Quit();
  printf("Application finished\n");
}

int main(int argc, char *argv[]) {
  srand((unsigned int)time(NULL));
  #ifdef __LINUX__
  printf("Working linux\n");
  #endif
  #ifdef __APPLE__
  printf("Working apple\n");
  #endif
  context ctx; // Defining context
  // CONTEXT STUFF
  ctx.quit = false;
  /* Important stuff */
  #ifdef TARGET_FPS
  ctx.ticksPerFrame = 1000.0f / TARGET_FPS;
  #endif
  ctx.window = NULL;   // freed
  ctx.renderer = NULL; // freed
  ctx.width = SCREEN_WIDTH;
  ctx.height = SCREEN_HEIGHT;
  /* Textures/Fonts */
  ctx.gFont = NULL; // freed

  // /* Music/Sounds */
  // ctx.soundEffect = NULL; // freed
  // ctx.gameMusic = NULL;   // freed

  // fps stuff
  // ctx.fpsLoc = (SDL_FRect){SCREEN_WIDTH - 50, 0, 50, 25};
  // ctx.fpsCol = (SDL_Color){200, 200, 200, 255};

  // button
  // ctx.isButtPressed = false;
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
