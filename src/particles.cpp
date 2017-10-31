
#include "particles.hpp"

#include "maths.hpp"


float RandomSpread(float spread)
{
  float accum = RandomFloat(-spread, spread) + RandomFloat(-spread, spread);
  return accum / 2.0f;
}


vec2 RandomVec2(float spread)
{
  return {RandomSpread(spread), RandomSpread(spread)};
}


Particle::Particle(vec2 location, vec2 vel, float size, const vec4 &col, float ttl)
: ttl(ttl)
, size(size)
, colour(col)
, position(location)
, velocity(vel)
{
  this->ttl += RandomFloat(-0.2f, 0.3f);
  this->size += RandomFloat(0.0f, 5.0f);

  position += RandomVec2(10.0f);
  rotation = RandomFloat(0, TWO_PI);

  this->velocity += RandomVec2(1.0f);
  rot_vel = RandomSpread(0.1f);
}


Particle UpdateParticle(const Particle &p, float dt)
{
  Particle out = p;

  out.ttl -= dt;
  out.position += p.velocity;
  out.rotation += p.rot_vel;

  return out;
}

constexpr int PARTICLE_VERTEX_SIZE = (3 * (2 + 4));
#include <cassert>

std::vector<float> MakeParticleVertexes(const Particle &p)
{
  std::vector<float> out;
  out.reserve(PARTICLE_VERTEX_SIZE);

  float alpha = p.ttl < 0.8f ? p.ttl / 0.8f : 1.0f;

  for (int i = 0; i < 3; i++)
  {
    float angle = (i / 3.0f) * TWO_PI;
    angle += p.rotation;
    float x = cosf(angle) * p.size;
    float y = sinf(angle) * p.size;

    out.push_back(x + p.position.x);
    out.push_back(y + p.position.y);
    out.push_back(p.colour.r);
    out.push_back(p.colour.g);
    out.push_back(p.colour.b);
    out.push_back(alpha * p.colour.a);
  }

  assert(out.size() == PARTICLE_VERTEX_SIZE);

  return out;
}


std::vector<float> MakeParticleVertexes(const std::vector<Particle> &particle_list)
{
  std::vector<float> out;
  out.reserve(particle_list.size() * PARTICLE_VERTEX_SIZE);

  for (const Particle &particle : particle_list)
  {
    const auto v_data = MakeParticleVertexes(particle);
    out.insert(out.end(), v_data.begin(), v_data.end());
  }

  assert(out.size() == (particle_list.size() * PARTICLE_VERTEX_SIZE));

  return out;
}
