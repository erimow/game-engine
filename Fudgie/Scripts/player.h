#include "../Engine/Button.h"
#include "../Engine/constants.h"
#include "../Engine/context.h"
#include "card.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef PLAYER_H
#define PLAYER_H
#define MAX_CARDS 10

typedef enum { NONE, DEFENSIVE, BALANCED, AGGRESIVE } STYLE;
typedef enum { NADA, EASY, NORMAL, HARD } DIFFICULTY;
typedef struct {
  Card *hand[MAX_CARDS];
  uint8_t numCardsInHand;
  int8_t currentPrediction;
  Uint8 currentRoundHandsWon;
  uint8_t points;
  STYLE style;
  DIFFICULTY difficulty;
} Player;

void Player_Init(Player *p);
void Player_InitPlayers(Player *p, uint8_t numPlayers);
void Player_AddCard(Player *p, Card *c);
void Player_PlayCard(Player *p);
void Player_UpdateHand(Player *p, SDL_FPoint *mousePos,
                       SDL_FRect *handLocation);
void Player_RenderHand(Player *p, SDL_Renderer *renderer,
                       SDL_FRect *handLocation, SDL_FRect *playLocation);
void Player_PrintHand(Player *p);

bool Player_UserPredict(context *ctx, Player *players, Uint8 *playerCount,
                        Uint8 *playerPlaying, Uint8 *round,
                        Uint8 *playerStartingPrediction,
                        Uint8 *playerStartingRound,
                        Uint8 *combinedRoundPredictions, Texture *playerText,
                        Texture *phaseOrTurnText, Button *predictionButtons);
bool Player_CPUPredict(context *ctx, Player *players, Uint8 *playerCount,
                       Uint8 *playerPlaying, Uint8 *round,
                       Uint8 *playerStartingPrediction,
                       Uint8 *playerStartingRound,
                       Uint8 *combinedRoundPredictions, Texture *playerText,
                       Texture *phaseOrTurnText);
bool Player_UserPlay(context *ctx, Player *players, Uint8 *playerCount,
                        Uint8 *playerPlaying, Uint8 *round,
                        Uint8 *playerStartingPrediction,
                        Uint8 *playerStartingRound,
                        Uint8 *combinedRoundPredictions, Uint8 *selectedCount, Texture *playerText,
                        Texture *phaseOrTurnText, Button *playbutt);
bool Player_CPUPlay(context *ctx, Player *players, Uint8 *playerCount,
                    Uint8 *playerPlaying, Uint8 *round,
                    Uint8 *playerStartingPrediction, Uint8 *playerStartingRound,
                    Uint8 *combinedRoundPredictions, Uint8 *selectedCount, Card **cardSelected, Texture *playerText,
                    Texture *phaseOrTurnText);

#endif
