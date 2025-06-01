#ifndef PARTICLE_H
#define PARTICLE_H
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>
#include <stdio.h>
#include <stdlib.h>
typedef enum PARTICLE_TYPE { EXPLOSION, DIRECTIONAL } PARTICLE_TYPE;
typedef struct ParticleTemplate {
  Uint16 particleAmount;
  float particleDuration;
  float fadeDuration;
  float spawnRadius;
  float directionalSpread;
  float baseVelocity;
  float velocityVariance;
  float gravity;
  float durationalScale;
  bool hasCollision;
  SDL_Color particleColor;
} ParticleTemplate;

typedef struct particle {
  SDL_FRect p;
  SDL_FPoint moveDir;
  SDL_FPoint moveVelocity;
} particle;

typedef struct particleInstance {
  ParticleTemplate *particleTemplate;
  particle *particles; // array of particles
  SDL_FPoint pos;      // epicentre or beginning of the effect
} particleInstance;

void Particle_init(ParticleTemplate *p, Uint16 particleAmount,
                   float particleDuration, float fadeDuration,
                   float spawnRadius, float directionalSpread,
                   float baseVelocity, float velocityVariance, float gravity,
                   float durationalScale, bool hasCollision, SDL_Color color);

void Particle_deinit(ParticleTemplate *p);

particleInstance Particle_spawnInstance(ParticleTemplate *p, SDL_FPoint *loc);
void Particle_updateInstance(particleInstance *p);
void Particle_renderInstance(particleInstance *p, SDL_Renderer *renderer);
void Particle_destroyInstance(particleInstance *p);
#endif
