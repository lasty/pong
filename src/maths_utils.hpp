#pragma once

#include "maths_types.hpp"

#include <iostream>
#include <sstream>


inline std::ostream & operator<<(std::ostream &out, vec2 const & v2)
{
  return out << "vec2{" << v2.x << ", " << v2.y <<"}";
}


inline std::ostream & operator<<(std::ostream &out, vec3 const & v3)
{
  return out << "vec3{" << v3.x << ", " << v3.y << ", " << v3.z << "}";
}


inline std::ostream & operator<<(std::ostream &out, vec4 const & v4)
{
  return out << "vec4{" << v4.x << ", " << v4.y
    << ", " << v4.z << ", " << v4.w << "}";
}


extern std::stringstream TRACE;
