
#pragma once

#include <vector>

#include "maths_types.hpp"


struct Particle
{
  float ttl;

  float size;
  vec4 colour;

  vec2 position;
  float rotation;

  vec2 velocity;
  float rot_vel;
};


Particle MakeParticle(vec2 location, vec2 vel, float size,
  const vec4 &col, float ttl);

Particle UpdateParticle(const Particle &p, float dt);

std::vector<float> MakeParticleVertexes(const Particle &p);

std::vector<float> MakeParticleVertexes(const std::vector<Particle> &particle_list);
