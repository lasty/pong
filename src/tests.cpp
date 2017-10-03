
#include <iostream>
using std::cout;
using std::endl;

#include "maths.hpp"
#include "maths_utils.hpp"


std::ostringstream TRACE;


vec2 GetGLStyle(const float *data)
{
  using std::cout;
  using std::endl;

  cout << "GetGLStyle got data :" << data[0] << ", " << data[1] << endl;
  vec2 out{data[0], data[1]};
  return out;
}


void TestVec2()
{

  cout << "\n\n==== Testing Vec2\n"
       << endl;

  if (true)
  {
    vec2 in{3, 6};
    cout << "in = " << in << endl;

    //vec2 out = GetGLStyle(gl_data(in));
    vec2 out = GetGLStyle(gl_data(in));

    cout << "out = " << out << endl;

    cout << "sizeof(vec2) is " << sizeof(vec2) << " which is " << sizeof(vec2) / sizeof(float) << " floats." << endl;
    cout << "\n===\n"
         << endl;
  }

  if (true)
  {
    vec2 v{1, 2};
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
    cout << "\n===\n"
         << endl;
  }

  if (true)
  {
    vec2 v1{10, 5};
    cout << "v1 = " << v1 << endl;
    cout << "get angle (v1) = " << get_angle(v1) << endl;
    cout << "get length (v1) = " << get_length(v1) << endl;
    cout << "v2 = angle_to_vec2(PI/6.0, 100)" << endl;
    vec2 v2 = angle_to_vec2(PI / 6.0, 100);
    cout << "v2 = " << v2 << endl;
    cout << "get angle (v2) = " << get_angle(v2) << endl;
    cout << "get length (v2) = " << get_length(v2) << endl;
    cout << "\n===\n"
         << endl;
  }

  if (true)
  {
    vec2 v1{10, 5};
    vec2 v2{3, 4};

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


    cout << "\n===\n"
         << endl;
  }

  if (true)
  {
    vec2 zero{};
    vec2 tri{3, 4};
    cout << "tri = " << tri << endl;
    cout << "get_length(tri) = " << get_length(tri) << endl;

    cout << "\nzero = " << zero << endl;
    cout << "distance(zero, tri) = " << distance(zero, tri) << endl;
    cout << "distance_squared(zero, tri) = " << distance_squared(zero, tri) << endl;
  }
}


void TestMat4()
{
  cout << "\n\n==== Testing Matrix 4x4\n"
       << endl;

  if (true)
  {
    mat4 ident = mat4_identity();

    cout << "itent = " << ident << endl
         << endl;

    mat4 zero = mat4_zero();

    float *elem = (float *)zero.elements;
    elem[0] = 1;
    elem[1] = 2;

    cout << "zero = " << zero << endl
         << endl;
  }
}


void TestMaths()
{
  cout.precision(2);
  cout << std::fixed;

  cout << "\n\n==== Testing Maths()\n"
       << endl;

  TestVec2();

  TestMat4();
}

int main()
{
  TestMaths();

  return EXIT_SUCCESS;
}
