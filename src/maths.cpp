
#include "maths.hpp"

#include <iostream>
#include <iomanip>


#include "maths_utils.hpp"


//Vectors should be packed for use by opengl functions
static_assert(sizeof(vec2) == sizeof(float) * 2, "vec2 not packed correctly");
static_assert(sizeof(vec3) == sizeof(float) * 3, "vec3 not packed correctly");
static_assert(sizeof(vec4) == sizeof(float) * 4, "vec4 not packed correctly");


float degrees(float radians)
{
  return radians * 180.0f / PI;
}


float radians(float degrees)
{
  return degrees * PI / 180.0f;
}


float get_angle(vec2 const & v)
{
  return atan2f(v.y, v.x);
}


float get_length(vec2 const & v)
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


vec2 & operator+=(vec2 &a, vec2 const &b)
{
  a.x += b.x;
  a.y += b.y;
  return a;
}


vec2 & operator-=(vec2 &a, vec2 const &b)
{
  a.x -= b.x;
  a.y -= b.y;
  return a;
}


vec2 & operator*=(vec2 &a, float s)
{
  a.x *= s;
  a.y *= s;
  return a;
}


vec2 & operator/=(vec2 &a, float s)
{
  a.x /= s;
  a.y /= s;
  return a;
}


vec4 operator*(vec4 const &a, float s)
{
  return { a.x * s, a.y * s, a.z * s, a.w * s };
}


vec2 vec_abs(vec2 const &v)
{
  return {fabs(v.x), fabs(v.y)};
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

  vec2 normal { -dy, dx };
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
  const float line_length = distance_squared(v, w);  // i.e. |w-v|^2 -  avoid a sqrt

  // v == w case
  if (line_length == 0.0) return v;
  //return distance(p, v);


  // Consider the line extending the segment, parameterized as v + t (w - v).
  // We find projection of point p onto the line.
  // It falls where t = [(p-v) . (w-v)] / |w-v|^2
  // We clamp t from [0,1] to handle points outside the segment vw.
  float t = dot(p - v, w - v) / line_length;
  t = std::max(0.0f, std::min(1.0f, t));

  const vec2 projection = v + (w - v) * t;  // Projection falls on the segment

  return projection;
  //return distance(p, projection);
}


bool in_range(float beg, float end, float p)
{
  return (p >= beg and p <= end);
}



const float * gl_data(vec2 const &v)
{
  return reinterpret_cast<const float*>(&v);
}

const float * gl_data(vec3 const &v)
{
  return reinterpret_cast<const float*>(&v);
}

const float * gl_data(vec4 const &v)
{
  return reinterpret_cast<const float*>(&v);
}


#ifdef TEST_MATHS

#include "gl.hpp"

vec2 GetGLStyle(const GLfloat* data)
{
  using std::cout;
  using std::endl;

  cout << "GetGLStyle got data :" << data[0] << ", " << data[1] << endl;
  vec2 out { data[0], data[1] };
  return out;
}


void TestMaths()
{
  using std::cout;
  using std::endl;

  cout.precision(2);
  cout << std::fixed;

  cout << "Testing Maths()" << endl;

  //if(false)
  {
    vec2 in { 3, 6 };
    cout << "in = " << in << endl;

    //vec2 out = GetGLStyle(gl_data(in));
    vec2 out = GetGLStyle(gl_data(in));

    cout << "out = " << out << endl;

    cout << "sizeof(vec2) is " << sizeof(vec2) << " which is " << sizeof(vec2) / sizeof(float) << " floats." << endl;
    cout << "\n===\n" << endl;

  }

  if (false)
  {
    vec2 v {1, 2};
    cout << "v = " << v << endl;
    cout << "get angle (v) = " << get_angle(v) << endl;
    cout << "get length (v) = " << get_length(v) << endl;

    vec2 w;
    cout << "w (uninitialized) = " << w << endl;

    w = angle_to_vec2(0.0f, 10.0f);
    cout << "w angle 0, should point right?  -  " << w << endl;
    cout << "get angle (w) = " << get_angle(w) << endl;
    cout << "get length (w) = " << get_length(w) << endl;

    cout << "sizeof(vec2) is " << sizeof(vec2) << " which is " << sizeof(vec2) / sizeof(float) << " floats." << endl;
    cout << "\n===\n" << endl;
  }

  if (false)
  {
    vec2 v1{10,5};
    cout << "v1 = " << v1 << endl;
    cout << "get angle (v1) = " << get_angle(v1) << endl;
    cout << "get length (v1) = " << get_length(v1) << endl;
    cout << "v2 = angle_to_vec2(PI/6.0, 100)" << endl;
    vec2 v2 = angle_to_vec2(PI/6.0, 100);
    cout << "v2 = " << v2 << endl;
    cout << "get angle (v2) = " << get_angle(v2) << endl;
    cout << "get length (v2) = " << get_length(v2) << endl;
    cout << "\n===\n" << endl;
  }

  if (false)
  {
    vec2 v1 {10,5};
    vec2 v2 {3,4};

    vec2 v3 = v1 + v2;

    cout << "v1 = " << v1 << endl;
    cout << "v2 = " << v2 << endl;
    cout << "v3 = v1+v2 = " << v3 << endl;

    vec2 m1 = v1 * 2;
    cout << "m1 = v1 * 2 = " << m1 << endl;
    cout << "get_length(v1) = " << get_length(v1) << endl;
    cout << "get_length(m1) = " << get_length(m1) << endl;

    cout << "m1+= v2 = " << (m1 += v2) << endl;
    cout << "m1=  " << m1 << endl;


    cout << "\n===\n" << endl;
  }

  if (false)
  {
    cout << "\n===\n" << endl;
  }

}


#endif //TEST_MATHS
