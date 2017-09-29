
#include "particles.hpp"

#include "maths.hpp"


float RandomSpread(float spread)
{
  return RandomFloat(-spread, spread);
}


vec2 RandomVec2(float spread)
{
  return {RandomSpread(spread), RandomSpread(spread)};
}


Particle MakeParticle(vec2 location, vec2 vel, float size,
  const vec4 &col, float ttl)
{
  location += RandomVec2(10.0f);
  vel += RandomVec2(1.0f);

  size += RandomFloat(0.0f, 5.0f);
  ttl += RandomFloat(-0.5f, 0.5f);

  float rotation = RandomFloat(0, TWO_PI);
  float rot_vel = RandomFloat(-10.0f, 10.0f);

  return {ttl, size, col, location, rotation, vel, rot_vel};
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

  float alpha = p.ttl < 2.0f ? p.ttl / 2.0f : 1.0f;

  out.push_back(0.0f * p.size + p.position.x);
  out.push_back(-1.0f * p.size + p.position.y);
  out.push_back(p.colour.r);
  out.push_back(p.colour.g);
  out.push_back(p.colour.b);
  out.push_back(alpha);

  out.push_back(-1.0f * p.size + p.position.x);
  out.push_back(1.0f * p.size + p.position.y);
  out.push_back(p.colour.r);
  out.push_back(p.colour.g);
  out.push_back(p.colour.b);
  out.push_back(alpha);

  out.push_back(1.0f * p.size + p.position.x);
  out.push_back(1.0f * p.size + p.position.y);
  out.push_back(p.colour.r);
  out.push_back(p.colour.g);
  out.push_back(p.colour.b);
  out.push_back(alpha);

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
