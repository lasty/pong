
#include "maths.hpp"

#include <iostream>
#include <iomanip>

#include "maths_collisions.hpp"
#include "maths_utils.hpp"


//Vectors should be packed for use by opengl functions
static_assert(sizeof(vec2) == sizeof(float) * 2, "vec2 not packed correctly");
static_assert(sizeof(vec3) == sizeof(float) * 3, "vec3 not packed correctly");
static_assert(sizeof(vec4) == sizeof(float) * 4, "vec4 not packed correctly");


std::ostringstream TRACE;


float degrees(float radians)
{
  return radians * 180.0f / PI;
}


float radians(float degrees)
{
  return degrees * PI / 180.0f;
}


float get_angle(vec2 const &v)
{
  return atan2f(v.y, v.x);
}


float get_length(vec2 const &v)
{
  return sqrtf(v.x * v.x + v.y * v.y);
}

vec2 angle_to_vec2(float angle, float length)
{
  return {cosf(angle) * length, sinf(angle) * length};
}


vec2 operator+(vec2 const &a, vec2 const &b)
{
  return {a.x + b.x, a.y + b.y};
}


vec2 operator-(vec2 const &a, vec2 const &b)
{
  return {a.x - b.x, a.y - b.y};
}


vec2 operator*(vec2 const &a, float s)
{
  return {a.x * s, a.y * s};
}


vec2 operator/(vec2 const &a, float s)
{
  return {a.x / s, a.y / s};
}


vec2 &operator+=(vec2 &a, vec2 const &b)
{
  a.x += b.x;
  a.y += b.y;
  return a;
}


vec2 &operator-=(vec2 &a, vec2 const &b)
{
  a.x -= b.x;
  a.y -= b.y;
  return a;
}


vec2 &operator*=(vec2 &a, float s)
{
  a.x *= s;
  a.y *= s;
  return a;
}


vec2 &operator/=(vec2 &a, float s)
{
  a.x /= s;
  a.y /= s;
  return a;
}


vec4 operator*(vec4 const &a, float s)
{
  return {a.x * s, a.y * s, a.z * s, a.w * s};
}


vec2 vec_abs(vec2 const &v)
{
  return {
    static_cast<float>(fabs(v.x)),
    static_cast<float>(fabs(v.y))};
}


float dot(vec2 const &a, vec2 const &b)
{
  return (a.x * b.x) + (a.y * b.y);
}


float distance(vec2 const &v1, vec2 const &v2)
{
  vec2 diff = v2 - v1;
  return sqrt(diff.x * diff.x + diff.y * diff.y);
}


float distance_squared(vec2 const &v1, vec2 const &v2)
{
  vec2 diff = v2 - v1;
  return diff.x * diff.x + diff.y * diff.y;
}


vec2 get_normal(vec2 const &p1, vec2 const &p2)
{
  float dx = p2.x - p1.x;
  float dy = p2.y - p1.y;

  vec2 normal{-dy, dx};
  return normalize(normal);
}


vec2 normalize(vec2 const &v)
{
  return (v / (get_length(v)));
}


vec2 reflect(vec2 const &I, vec2 const &N)
{
  //const vec2 N = normalize(normal);

  return I - N * dot(N, I) * 2.0f;
}


vec2 nearest_point_on_line(vec2 const &v, vec2 const &w, vec2 const &p)
{
  const float line_length = distance_squared(v, w); // i.e. |w-v|^2 -  avoid a sqrt

  // v == w case
  if (line_length == 0.0) return v;
  //return distance(p, v);


  // Consider the line extending the segment, parameterized as v + t (w - v).
  // We find projection of point p onto the line.
  // It falls where t = [(p-v) . (w-v)] / |w-v|^2
  // We clamp t from [0,1] to handle points outside the segment vw.
  float t = dot(p - v, w - v) / line_length;
  //t = std::max(0.0f, std::min(1.0f, t));

  const vec2 projection = v + (w - v) * t; // Projection falls on the segment

  return projection;
  //return distance(p, projection);
}

vec2 nearest_point_on_line_segment(vec2 const &v, vec2 const &w, vec2 const &p)
{
  const float line_length = distance_squared(v, w); // i.e. |w-v|^2 -  avoid a sqrt

  // v == w case
  if (line_length == 0.0) return v;
  //return distance(p, v);


  // Consider the line extending the segment, parameterized as v + t (w - v).
  // We find projection of point p onto the line.
  // It falls where t = [(p-v) . (w-v)] / |w-v|^2
  // We clamp t from [0,1] to handle points outside the segment vw.
  float t = dot(p - v, w - v) / line_length;
  t = std::max(0.0f, std::min(1.0f, t));

  const vec2 projection = v + (w - v) * t; // Projection falls on the segment

  return projection;
  //return distance(p, projection);
}


vec2 get_intersection(vec2 ps1, vec2 pe1, vec2 ps2, vec2 pe2)
{
  // Get A,B,C of first line - points : ps1 to pe1
  float A1 = pe1.y - ps1.y;
  float B1 = ps1.x - pe1.x;
  float C1 = A1 * ps1.x + B1 * ps1.y;

  // Get A,B,C of second line - points : ps2 to pe2
  float A2 = pe2.y - ps2.y;
  float B2 = ps2.x - pe2.x;
  float C2 = A2 * ps2.x + B2 * ps2.y;

  // Get delta and check if the lines are parallel
  float delta = A1 * B2 - A2 * B1;
  if (delta == 0)
    throw std::runtime_error("Lines are parallel");

  // now return the Vector2 intersection point
  return {
    (B2 * C1 - B1 * C2) / delta,
    (A1 * C2 - A2 * C1) / delta};
}


bool in_range(float beg, float end, float p)
{
  return (p >= beg and p <= end);
}


float clamp(float min, float max, float val)
{
  return std::min(std::max(min, val), max);
}


const float *gl_data(vec2 const &v)
{
  return reinterpret_cast<const float *>(&v);
}

const float *gl_data(vec3 const &v)
{
  return reinterpret_cast<const float *>(&v);
}

const float *gl_data(vec4 const &v)
{
  return reinterpret_cast<const float *>(&v);
}


float RandomFloat()
{
  return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}


float RandomFloat(const float r1, const float r2)
{
  const float range = r2 - r1;
  return (r1 + (RandomFloat() * range));
}


int RandomInt(int r1, int r2)
{
  const int range = r2 - r1;
  return (r1 + (rand() % range));
}


vec4 RandomRGB()
{
  return {RandomFloat(), RandomFloat(), RandomFloat(), 1.0f};
}


vec4 RandomRGBA()
{
  return {RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat()};
}


bool BoundingBoxCollides(const BoundingBox &a, const BoundingBox &b)
{
  if (&a == &b) return false;

  return (a.top_left.x < b.bottom_right.x) and
    (a.bottom_right.x > b.top_left.x) and
    (a.top_left.y < b.bottom_right.y) and
    (a.bottom_right.y > b.top_left.y);
}


bool Collides(const Ball &ball, Line const &line)
{
  vec2 collision_point = nearest_point_on_line_segment(line.p1, line.p2, ball.position);

  float dist = distance(collision_point, ball.position);

  return (dist < ball.radius);
}


bool Collides(const Ball &ball, const Block &block)
{
  if (not BoundingBoxCollides(ball.bounds, block.bounds)) return false;

  for (auto const &line : block.geometry)
  {
    if (Collides(ball, line)) return true;
  }
  return false;
}


bool Collides(const Ball &b1, const vec2 &point)
{
  float dist = distance(b1.position, point);

  return (dist <= b1.radius);
}


bool Collides(const Ball &b1, const Ball &b2)
{
  if (&b1 == &b2) return false;

  float radii = b1.radius + b2.radius;

  float dist = get_length(b2.position - b1.position);

  return (dist <= radii);
}


mat4 mat4_identity()
{
  mat4 m{0.0f};
  m.elements[0][0] = 1.0f;
  m.elements[1][1] = 1.0f;
  m.elements[2][2] = 1.0f;
  m.elements[3][3] = 1.0f;

  return m;
}

mat4 mat4_zero()
{
  mat4 m{0.0f};

  return m;
}
