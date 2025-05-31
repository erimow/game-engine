#ifndef PARTICLE_H
#define PARTICLE_H
#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <stdio.h>
typedef enum PARTICLE_TYPE { RECTANCLE, CIRCLE } PARTICLE_TYPE;
typedef struct particle {
  PARTICLE_TYPE particle_type;
  Uint16 particleAmount;
  Uint16 particleDuration;
  bool hadGravity;
  bool hasCollision;
} particle;

void Particle_init(particle *p, Uint16 particleAmount, Uint16 particleDuration,
                   bool hasGravity, bool hasCollision);
void Particle_deinit(particle *p);
#endif
