#pragma once

#include "maths_types.hpp"

#include <iostream>
#include <sstream>


inline std::ostream &operator<<(std::ostream &out, vec2 const &v2)
{
  return out << "vec2{" << v2.x << ", " << v2.y << "}";
}


inline std::ostream &operator<<(std::ostream &out, vec3 const &v3)
{
  return out << "vec3{" << v3.x << ", " << v3.y << ", " << v3.z << "}";
}

/*
inline std::ostream &operator<<(std::ostream &out, vec4 const &v4)
{
  return out << "vec4{" << v4.x << ", " << v4.y
             << ", " << v4.z << ", " << v4.w << "}";
}
*/

inline std::ostream &operator<<(std::ostream &out, col4 const &v4)
{
  return out << "col4{" << v4.r << ", " << v4.g
             << ", " << v4.b << ", " << v4.a << "}";
}


inline std::ostream &operator<<(std::ostream &out, mat4 const &mat)
{
  out << "mat4 [";
  for (int x = 0; x < 4; x++)
  {
    out << "\n";
    for (int y = 0; y < 4; y++)
    {
      out << "\t" << mat.elements[y][x];
    }
  }
  out << " ]";
  return out;
}


//Defined in maths.cpp - outputs to window title, cleared per frame
extern std::ostringstream TRACE;
