#include "../Engine/uiobject.h"
#include "../Engine/constants.h"
#include "../Engine/context.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// ==============================================================================================================================
// ==============================================================================================================================
// ==============================================================================================================================


static void maingamescene_loadAssets(
    void *ct) { //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA ASSET-LOADING AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
  context *ctx = (context *)ct;
}



static void maingamescene_start(
    void *ct) { //SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS START SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
  context *ctx = (context *)ct;
}



static void maingamescene_update(
    context *ctx) { //UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU UPDATE UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
}



static void maingamescene_render(
    SDL_Renderer *renderer) { // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR RENDER RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    SDL_RenderClear(renderer);
}



static void getMousePos(SDL_Event *e);
static void maingamescene_events(
    SDL_Event *e) { // EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE  EVENTS EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
}

static void
maingamescene_stop() { // SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS STOP SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
}

// static void getMousePos(SDL_Event *e) {
//   if (e->type == SDL_EVENT_MOUSE_MOTION) {
//     // Get mouse position
//     float x, y;
//     SDL_GetMouseState(&x, &y);
//     mousePos.x = x;
//     mousePos.y = y;
//   }
// }
