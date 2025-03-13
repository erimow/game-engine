#include "../Engine/Button.h"
#include "../Engine/constants.h"
#include "../Engine/context.h"
#include "card.h"
#include "deck.h"

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum Phase { deal, play, predict, scoring } Phase;
const static Uint8 cardGap = 15;
static SDL_FRect handLocation = {
    (float)SCREEN_WIDTH / 8, SCREEN_HEIGHT - (float)SCREEN_HEIGHT / 4,
    SCREEN_WIDTH - (float)SCREEN_WIDTH / 4, (float)SCREEN_HEIGHT / 4};
static SDL_FRect playLocation = {
    ((float)SCREEN_WIDTH / 2) -
        (float)((CARDPXWIDTH * 4) + (float)cardGap * 3) / 2,
    ((float)SCREEN_HEIGHT / 2) - (float)CARDPXHEIGHT / 2,
    (float)(CARDPXWIDTH * 4) + (float)cardGap * 3, (float)CARDPXHEIGHT};
static SDL_FPoint mousePos;
//----------------------------------------------------------------------
static Card *cardBeingHeld = NULL; // user playing stuff
static Card **cardSelected = NULL; // user playing stuff
static Uint8 selectedCount = 0;
static Uint8 round = 8;       // round, cards dealt
static Uint8 userPlaying = 0; // the player the user is playing
static Uint8 playerCount;
static Phase currentPhase; // phase of game
static Deck deck;
static Player *players = NULL;
static Uint8 playerPlaying = 0; // keep track of player whos turn it is
static Uint8 playerStartingRound =
    0; // keep track of player who started round or other non prediction
static Uint8 playerStartingPrediction =
    0; // keep track of player who initiated prediction
static Texture *playerText = NULL; // text displaying player stats and what not
static Button playbutt; // play button to be displayed for userPlaying
static Button predictionButtons[9]; // probably need to switch to a pointer for
                                    // adjustability
static Texture roundText;           // text in corner displaying current round
// static Texture currentPredictionText;
static Texture phaseOrTurnText; // Text at top of screen

static void maingamescene_loadAssets(
    void *ct) { //-------------------------------------------ASSET-LOADING
  context *ctx = (context *)ct;
  if (!Texture_loadFromFile(&deck.spriteSheet, ctx->renderer,
                            "Art/CardSpritesheet.png"))
    printf("Could not load CardSpritesheet\n"); // loading in the
                                                // cardspritesheet
  Button_initAndLoad(&playbutt, ctx->renderer, 15,
                     ((float)SCREEN_WIDTH / 2) - 15, 120, 50,
                     "Art/ButtonBackground.png", ctx->gFont, "Play", 4,
                     (SDL_Color){5, 5, 5, 255});
  for (int i = 0; i < round + 1; i++) {
    char text[2];
    snprintf(text, 2, "%d", i);
    Button_initAndLoad(&predictionButtons[i], ctx->renderer,
                       handLocation.x + handLocation.w + 15 +
                           ((i % 2) * (((float)SCREEN_WIDTH / 16) - 7.5f)),
                       (float)SCREEN_HEIGHT / 3 +
                           ((i / 2) * (((float)SCREEN_HEIGHT / 16) + 7.5f)),
                       ((float)SCREEN_WIDTH / 16) - 20,
                       ((float)SCREEN_HEIGHT / 16) - 5,
                       "Art/ButtonBackground.png", ctx->gFont, text, 1,
                       (SDL_Color){5, 5, 5, 255});
  }
  Texture_init_andLoadFromRenderedText(
      &roundText, ctx->renderer, ctx->gFont, (SDL_FRect){15, 15, 250, 60},
      "Round of 8", 10, (SDL_Color){255, 255, 255, 255});
  // Texture_init_andLoadFromRenderedText(
  //     &currentPredictionText, ctx->renderer, ctx->gFont, (SDL_FRect){15, 75,
  //     200, 30}, "Current prediction: -1", 22, (SDL_Color){255, 255, 255,
  //     255});
  Texture_init_andLoadFromRenderedText(
      &phaseOrTurnText, ctx->renderer, ctx->gFont,
      (SDL_FRect){((float)SCREEN_WIDTH / 2) - 125, 15, 250, 60},
      "Prediction Phase", 16, (SDL_Color){255, 255, 255, 255});
}

static void maingamescene_start(
    void *ct) { //---------------------------------------------------START
  context *ctx = (context *)ct;
  playerCount = ctx->numPlayas;

  Deck_init(&deck);
  Deck_scramble(&deck);
  if (players != NULL) {
    free(players);
  }
  // mallocs
  players = (Player *)malloc(sizeof(Player) * ctx->numPlayas);
  Player_InitPlayers(players, ctx->numPlayas);
  playerText = (Texture *)malloc(sizeof(Texture) * ctx->numPlayas);
  cardSelected = (Card **)malloc(sizeof(Card *) * ctx->numPlayas);
  for (int i = 0; i < ctx->numPlayas; i++) {
    cardSelected[i] = NULL;
    char t[50];
    snprintf(t, 50, "Player %d: -1", i + 1);
    if (playerPlaying != i)
      Texture_init_andLoadFromRenderedText(
          &playerText[i], ctx->renderer, ctx->gFont,
          (SDL_FRect){15, 75 + (i * 30), 200, 30}, t, 11,
          (SDL_Color){255, 255, 255, 255});
    else {
      Texture_init_andLoadFromRenderedText(
          &playerText[i], ctx->renderer, ctx->gFont,
          (SDL_FRect){15, 75 + (i * 30), 230, 35}, t, 11,
          (SDL_Color){255, 255, 255, 255});
    }
  }
  Deck_deal(&deck, players, ctx->numPlayas, round);
  currentPhase = predict;
}
static void maingamescene_update(
    context *ctx) { //--------------------------------------------------UPDATE
  switch (currentPhase) {
  case deal:
    Deck_scramble(&deck);
    Deck_deal(&deck, players, ctx->numPlayas, --round);
    char j[11];
    snprintf(j, 11, "Round of %d", round);
    Texture_init_andLoadFromRenderedText(&roundText, ctx->renderer, ctx->gFont,
                                         (SDL_FRect){15, 15, 250, 60}, j, 10,
                                         (SDL_Color){255, 255, 255, 255});
    currentPhase = predict;
    Texture_init_andLoadFromRenderedText(
        &phaseOrTurnText, ctx->renderer, ctx->gFont,
        (SDL_FRect){((float)SCREEN_WIDTH / 2) - 125, 15, 250, 60},
        "Predict Phase", 13, (SDL_Color){255, 255, 255, 255});
    break;
  case predict:
    if (playerPlaying == userPlaying) {
      for (int i = 0; i < round + 1; i++) {
        if (predictionButtons[i].isButtPressed) {
          players[userPlaying].currentPrediction = i;
          predictionButtons[i].isButtPressed = false;
          char t[22];
          snprintf(t, 22, "Player %d: %d", userPlaying, i);
          Texture_init_andLoadFromRenderedText(
              &playerText[userPlaying], ctx->renderer, ctx->gFont,
              (SDL_FRect){15, 75 + (playerPlaying * 30), 200, 30}, t, 11,
              (SDL_Color){255, 255, 255, 255});
          // Texture_init_andLoadFromRenderedText(
          // &phaseOrTurnText, ctx->renderer, ctx->gFont,
          // (SDL_FRect){((float)SCREEN_WIDTH/2)-125, 15, 250, 60}, "Player 1's
          // Turn", 15, (SDL_Color){255, 255, 255, 255});
          playerPlaying =
              (playerPlaying + 1 == playerCount) ? 0 : playerPlaying + 1;
          if (playerPlaying == playerStartingPrediction) {

            currentPhase = play;
            Texture_init_andLoadFromRenderedText(
                &phaseOrTurnText, ctx->renderer, ctx->gFont,
                (SDL_FRect){((float)SCREEN_WIDTH / 2) - 125, 15, 250, 60},
                "Play Phase", 10, (SDL_Color){255, 255, 255, 255});
            Uint8 highestPredictionByPlayer = 0;
            for (int i = playerStartingPrediction + 1;
                 i < playerStartingPrediction + playerCount; i++) {
              if (players[i % playerCount].currentPrediction >
                  players[highestPredictionByPlayer].currentPrediction) {
                highestPredictionByPlayer = i % playerCount;
              }
            }
            playerStartingPrediction =
                (playerStartingPrediction + 1) % playerCount;
            playerPlaying = highestPredictionByPlayer;
            playerStartingRound = playerPlaying;
          }
          snprintf(t, 22, "Player %d: %d", playerPlaying,
                   players[playerPlaying].currentPrediction);
          Texture_init_andLoadFromRenderedText(
              &playerText[playerPlaying], ctx->renderer, ctx->gFont,
              (SDL_FRect){15, 75 + (playerPlaying * 30), 230, 35}, t, 11,
              (SDL_Color){255, 255, 255, 255});
        }
      }
    } else {
      if (ctx->frameCount % 2000 == 0) {
        char t[22];
        players[playerPlaying].currentPrediction =
            rand() % (round + 1); // makes random prediction
        snprintf(t, 22, "Player %d: %d", playerPlaying,
                 players[playerPlaying].currentPrediction);
        Texture_init_andLoadFromRenderedText(
            &playerText[playerPlaying], ctx->renderer, ctx->gFont,
            (SDL_FRect){15, 75 + (playerPlaying * 30), 200, 30}, t, 11,
            (SDL_Color){255, 255, 255, 255});
        playerPlaying =
            (playerPlaying + 1 == playerCount) ? 0 : playerPlaying + 1;
        if (playerPlaying == playerStartingPrediction) {
          Uint8 highestPredictionByPlayer = 0;
          for (int i = playerStartingPrediction + 1;
               i < playerStartingPrediction + playerCount; i++) {
            if (players[i % playerCount].currentPrediction >
                players[highestPredictionByPlayer].currentPrediction) {
              highestPredictionByPlayer = i % playerCount;
            }
          }
          playerStartingPrediction =
              (playerStartingPrediction + 1) % playerCount;
          playerPlaying = highestPredictionByPlayer;
          playerStartingRound = playerPlaying;

          currentPhase = play;
          Texture_init_andLoadFromRenderedText(
              &phaseOrTurnText, ctx->renderer, ctx->gFont,
              (SDL_FRect){((float)SCREEN_WIDTH / 2) - 125, 15, 250, 60},
              "Play Phase", 10, (SDL_Color){255, 255, 255, 255});
        }
        snprintf(t, 22, "Player %d: %d", playerPlaying,
                 players[playerPlaying].currentPrediction);
        Texture_init_andLoadFromRenderedText(
            &playerText[playerPlaying], ctx->renderer, ctx->gFont,
            (SDL_FRect){15, 75 + (playerPlaying * 30), 230, 35}, t, 11,
            (SDL_Color){255, 255, 255, 255});
      }
    }
    break;
  case play:
    if (selectedCount == playerCount &&
        playbutt.isButtPressed) { // breaks out and switches to scoring phase if
                                  // in render,
                                  // selectedCount counts to playerCount
      currentPhase = scoring;
      Texture_init_andLoadFromRenderedText(
          &phaseOrTurnText, ctx->renderer, ctx->gFont,
          (SDL_FRect){((float)SCREEN_WIDTH / 2) - 125, 15, 250, 60},
          "Scoring Phase", 13, (SDL_Color){255, 255, 255, 255});
      break;
    }
    if (playerPlaying == userPlaying) {
      for (int i = 0; i < players[userPlaying].numCardsInHand; i++) {
        Card *a = players[userPlaying].hand[i];
        if (a->isHeld) {
          a->pos.x = mousePos.x - a->whenHeldMousePos.x;
          a->pos.y = mousePos.y - a->whenHeldMousePos.y;
          if (players[userPlaying].numCardsInHand > 1 &&
              (a->pos.y + ((float)CARDPXHEIGHT / 2)) > handLocation.y) {
            if (i != 0) {
              if (a->pos.x + ((float)CARDPXWIDTH / 2) <
                  players[userPlaying].hand[i - 1]->pos.x +
                      ((float)CARDPXWIDTH / 2)) {
                players[userPlaying].hand[i] = players[userPlaying].hand[i - 1];
                players[userPlaying].hand[i - 1] = a;
              }
            }
            if (i != players[userPlaying].numCardsInHand - 1) {
              if (a->pos.x + ((float)CARDPXWIDTH / 2) >
                  players[userPlaying].hand[i + 1]->pos.x +
                      ((float)CARDPXWIDTH / 2)) {
                players[userPlaying].hand[i] = players[userPlaying].hand[i + 1];
                players[userPlaying].hand[i + 1] = a;
              }
            }
          }
        }
      }
      if (playbutt.isButtPressed) {
        char t[22];
        snprintf(t, 22, "Player %d: %d", playerPlaying,
                 players[playerPlaying].currentPrediction);
        Texture_init_andLoadFromRenderedText(
            &playerText[playerPlaying], ctx->renderer, ctx->gFont,
            (SDL_FRect){15, 75 + (playerPlaying * 30), 200, 30}, t, 11,
            (SDL_Color){255, 255, 255, 255});
        playerPlaying =
            (playerPlaying + 1 == playerCount) ? 0 : playerPlaying + 1;
        snprintf(t, 22, "Player %d: %d", playerPlaying,
                 players[playerPlaying].currentPrediction);
        if (selectedCount != playerCount)
          Texture_init_andLoadFromRenderedText(
              &playerText[playerPlaying], ctx->renderer, ctx->gFont,
              (SDL_FRect){15, 75 + (playerPlaying * 30), 230, 35}, t, 11,
              (SDL_Color){255, 255, 255, 255});
        // phase switches to scoring in render func
      }
    } else {
      if (ctx->frameCount % 2000 == 0) {
        Uint8 ran = rand() % round;
        players[playerPlaying].hand[ran]->isSelected = true;
        cardSelected[playerPlaying] = players[playerPlaying].hand[ran];
        char t[22];
        snprintf(t, 22, "Player %d: %d", playerPlaying,
                 players[playerPlaying].currentPrediction);
        Texture_init_andLoadFromRenderedText(
            &playerText[playerPlaying], ctx->renderer, ctx->gFont,
            (SDL_FRect){15, 75 + (playerPlaying * 30), 200, 30}, t, 11,
            (SDL_Color){255, 255, 255, 255});
        playerPlaying =
            (playerPlaying + 1 == playerCount) ? 0 : playerPlaying + 1;
        snprintf(t, 22, "Player %d: %d", playerPlaying,
                 players[playerPlaying].currentPrediction);
        if (selectedCount + 1 != playerCount)
          Texture_init_andLoadFromRenderedText(
              &playerText[playerPlaying], ctx->renderer, ctx->gFont,
              (SDL_FRect){15, 75 + (playerPlaying * 30), 230, 35}, t, 11,
              (SDL_Color){255, 255, 255, 255});
        // phase switches to scoring in render func
      }
    }
    break;
  case scoring:
    // playbutt.isButtPressed = false; //reset stuff
    break;
  }
}
static void maingamescene_render(
    SDL_Renderer *renderer) { // -------------------------------------- RENDER

  // UI
  switch (currentPhase) {
  case deal:
    break;
  case predict:
    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    SDL_RenderRect(renderer, &handLocation);
    SDL_RenderRect(renderer, &playLocation);
    Player_RenderHand(&players[userPlaying], renderer, &handLocation,
                      &playLocation);

    Texture_render(&phaseOrTurnText, renderer, NULL, NULL, 0.0, NULL,
                   SDL_FLIP_NONE);
    Texture_render(&roundText, renderer, NULL, NULL, 0.0, NULL, SDL_FLIP_NONE);
    if (playerPlaying == userPlaying)
      for (int i = 0; i < round + 1; i++) {
        Button_render(&predictionButtons[i], renderer);
      }
    for (int i = 0; i < playerCount; i++) {
      Texture_render(&playerText[i], renderer, NULL, NULL, 0.0, NULL,
                     SDL_FLIP_NONE);
    }
    break;
  case play:
    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    SDL_RenderRect(renderer, &handLocation);
    SDL_RenderRect(renderer, &playLocation);
    Player_RenderHand(&players[userPlaying], renderer, &handLocation,
                      &playLocation);
    selectedCount = 0; // terrible way to do this, but is what it is.
    float gap =
        (playLocation.w - (CARDPXWIDTH * playerCount)) / (playerCount + 1);
    for (int i = playerStartingRound; i < playerStartingRound + playerCount;
         i++) {
      if (cardSelected[i % playerCount] != NULL) {
        cardSelected[i % playerCount]->pos.x = (playLocation.x) +
                                               (gap * ((i % playerCount) + 1)) +
                                               (selectedCount * CARDPXWIDTH);
        cardSelected[i % playerCount]->pos.y = playLocation.y;
        Card_Render(cardSelected[i % playerCount], renderer);
        selectedCount++;
      }
    }

    // UI STUFF
    if (cardSelected[userPlaying] != NULL && userPlaying == playerPlaying)
      Button_render(&playbutt, renderer);

    Texture_render(&phaseOrTurnText, renderer, NULL, NULL, 0.0, NULL,
                   SDL_FLIP_NONE);
    // Texture_render(&currentPredictionText, renderer, NULL, NULL, 0.0, NULL,
    // SDL_FLIP_NONE);
    Texture_render(&roundText, renderer, NULL, NULL, 0.0, NULL, SDL_FLIP_NONE);
    for (int i = 0; i < playerCount; i++) {
      Texture_render(&playerText[i], renderer, NULL, NULL, 0.0, NULL,
                     SDL_FLIP_NONE);
    }
    break;
  case scoring:
    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    SDL_RenderRect(renderer, &handLocation);
    SDL_RenderRect(renderer, &playLocation);
    Player_RenderHand(&players[userPlaying], renderer, &handLocation,
                      &playLocation);

    for (int i = 0; i < playerCount; i++) {
      Card_Render(cardSelected[i], renderer);
    }

    Texture_render(&phaseOrTurnText, renderer, NULL, NULL, 0.0, NULL,
                   SDL_FLIP_NONE);
    // Texture_render(&currentPredictionText, renderer, NULL, NULL, 0.0, NULL,
    // SDL_FLIP_NONE);
    Texture_render(&roundText, renderer, NULL, NULL, 0.0, NULL, SDL_FLIP_NONE);
    for (int i = 0; i < playerCount; i++) {
      Texture_render(&playerText[i], renderer, NULL, NULL, 0.0, NULL,
                     SDL_FLIP_NONE);
    }
    break;
  }
}

static void getMousePos(SDL_Event *e);
static void maingamescene_events(
    SDL_Event *e) { // -------------------------------------------------  EVENTS
  getMousePos(e);
  switch (currentPhase) {
  case deal:
    break;
  case predict:
    if (playerPlaying == userPlaying)
      for (int i = 0; i < round + 1; i++) {
        Button_handleEvent(&predictionButtons[i], e);
      }
    break;
  case play:
    for (int i = players[userPlaying].numCardsInHand - 1; i >= 0; i--) {
      Card_HandleEvents(players[userPlaying].hand[i], e, mousePos,
                        &playLocation, &cardBeingHeld,
                        &cardSelected[userPlaying]);
    }
    if (cardSelected[userPlaying] != NULL && userPlaying == playerPlaying)
      Button_handleEvent(&playbutt, e);
    break;

  case scoring:
    break;
  }
}

static void
maingamescene_stop() { // --------------------------------------------STOP
  Button_free(&playbutt);
  for (int i = 0; i < round + 1; i++) {
    Button_free(&predictionButtons[i]);
  }
  for (int i = 0; i < playerCount; i++) {
    Texture_free(&playerText[i]);
  }
  free(playerText);
  free(cardSelected);
  Texture_free(&phaseOrTurnText);
  // Texture_free(&currentPredictionText);
  Texture_free(&roundText);
  Texture_free(&deck.spriteSheet);
  free(players);
}

static void getMousePos(SDL_Event *e) {
  if (e->type == SDL_EVENT_MOUSE_MOTION) {
    // Get mouse position
    float x, y;
    SDL_GetMouseState(&x, &y);
    mousePos.x = x;
    mousePos.y = y;
  }
}
