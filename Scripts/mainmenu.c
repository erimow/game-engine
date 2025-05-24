#include "../Engine/uiobject.h"
#include "../Engine/Texture.h"
#include "../Engine/constants.h"
#include "../Engine/context.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

static Texture titleText;
static Button playButton;

static void mainmenuscene_loadAssets(void *ct) {
  context *ctx = (context*)ct;
  Texture_init_andLoadFromRenderedText(
      &titleText, ctx->renderer, ctx->gFont,
      (SDL_FRect){((float)SCREEN_WIDTH / 2) - (float)SCREEN_WIDTH / 4,
                  (float)SCREEN_HEIGHT / 6, (float)SCREEN_WIDTH / 2,
                  (float)SCREEN_WIDTH / 4},
      "Game!", 5, (SDL_Color){255, 255, 255, 255});
    Button_initAndLoad(
        &playButton, ctx->renderer,
        (SCREEN_WIDTH/2)-200, (float)SCREEN_HEIGHT / 1.5,
        400, (float)SCREEN_HEIGHT / 7, "Art/ButtonBackground.png",
        ctx->gFont, "Play", 4, (SDL_Color){255, 255, 255, 255});
}

static void mainmenuscene_start(void *ct) {}

static void mainmenuscene_update(context *ctx) {
    if (playButton.isButtPressed) {
      SDL_Log("Switching to game scene\n");
      SceneManager_switchScene(&ctx->sm, GAME, ctx);
  }
}

static void mainmenuscene_render(SDL_Renderer *renderer) {
  // SDL_SetRenderDrawColor(renderer, );
  // SDL_RenderClear(renderer);
  Texture_render(&titleText, renderer, NULL, NULL, 0.0, NULL, SDL_FLIP_NONE);
    Button_render(&playButton, renderer);
}

static void mainmenuscene_stop() {
  Texture_free(&titleText);
  Button_free(&playButton);
}

static void mainmenuscene_handleEvents(SDL_Event *e) {
  Button_handleEvent(&playButton, e);
}
