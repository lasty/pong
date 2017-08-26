
#pragma once


#include "maths_types.hpp"

#include <math.h>

constexpr float PI { float{M_PI} };
constexpr float TWO_PI { 2.0f * float{M_PI} };

float degrees(float radians);
float radians(float degrees);


float get_angle(vec2 const & v);
float get_length(vec2 const & v);

vec2 angle_to_vec2(float angle, float length);

vec2 operator+(vec2 const &a, vec2 const &b);
vec2 operator-(vec2 const &a, vec2 const &b);
vec2 operator*(vec2 const &a, float s);
vec2 operator/(vec2 const &a, float s);

vec2 & operator+=(vec2 &a, vec2 const &b);
vec2 & operator-=(vec2 &a, vec2 const &b);
vec2 & operator*=(vec2 &a, float s);
vec2 & operator/=(vec2 &a, float s);

vec4 operator*(vec4 const &a, float s);

const float * gl_data(vec2 const &v);
const float * gl_data(vec3 const &v);
const float * gl_data(vec4 const &v);

void TestMaths();
