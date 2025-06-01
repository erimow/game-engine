#include "particle.h"
#include <SDL3/SDL_oldnames.h>
void Particle_init(ParticleTemplate *p, Uint16 particleAmount,
                   float particleDuration, float fadeDuration,
                   float spawnRadius, float directionalSpread,
                   float baseVelocity, float velocityVariance, float gravity,
                   float durationalScale, bool hasCollision, SDL_Color color) {
  p->particleAmount = particleAmount;
  p->particleDuration = particleDuration;
  p->fadeDuration = fadeDuration;
  p->spawnRadius = spawnRadius;
  p->directionalSpread = directionalSpread;
  p->baseVelocity = baseVelocity;
  p->velocityVariance = velocityVariance;
  p->gravity = gravity;
  p->durationalScale = durationalScale;
  p->hasCollision = hasCollision;
  p->particleColor = color;

  // error handling kinda
  if (fadeDuration > particleDuration)
    SDL_Log("particle fadeDuration should not exceed particleDuration!!!");
}

void Particle_deinit(ParticleTemplate *p) {}

particleInstance Particle_spawnInstance(ParticleTemplate *p, SDL_FPoint *loc) {
  particleInstance part;
  part.particles = malloc(sizeof(SDL_FRect) * p->particleAmount);

  return part;
}
void Particle_updateInstance(particleInstance *p) {}
void Particle_renderInstance(particleInstance *p, SDL_Renderer *renderer) {
  for (int i = 0; i < p->particleTemplate->particleAmount; i++) {
    SDL_RenderFillRect(renderer, &p->particles[i].p);
  }
}
void Particle_destroyInstance(particleInstance *p) { free(p->particles); }
